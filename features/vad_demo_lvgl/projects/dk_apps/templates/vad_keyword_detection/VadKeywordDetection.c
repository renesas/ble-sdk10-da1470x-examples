/**
 ****************************************************************************************
 *
 *
 * @file VadKeywordDetection.c
 *
 * @brief main task for DSpotter
 *
 * Copyright (c) 2022 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor. By using this Software
 * you agree that Dialog Semiconductor retains all intellectual property and proprietary
 * rights in and to this Software and any use, reproduction, disclosure or distribution
 * of the Software without express written permission or a license agreement from Dialog
 * Semiconductor is strictly prohibited. This Software is solely for use on or in
 * conjunction with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR AS
 * REQUIRED BY LAW, THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE PROVIDED
 * IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR BY LAW, IN NO EVENT SHALL DIALOG
 * SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE.
 *
 ****************************************************************************************
 */
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <time.h>
#include "include/base_types.h"
#include "include/DSpotterSDKApi_Const.h"
#include "include/DSpotterSDKApi.h"
#include "include/RingBuffer.h"
#include "include/CybModelInfor.h"
#include "include/Convert2TransferBuffer.h"
#include "Model/CybModel1.h" // The packed model file.
#include "Util/PortFunction.h"
#include "AudioRecord.h"
#include "hw_sys.h"
#include "hw_uart.h"
#include "sys_clock_mgr.h"

// AUDIO
#include "periph_setup.h"
#include "Helper_func.h"

// AUDIO

#define NOT_SHOW_MULTI_PRONUNCIATION
//#define SUPPORT_UART_DUMP_RECORD

#define MAX_COMMAND_TIME        (5000 / 10)			// Trigger and command must be spoke within 5000ms.
#define DSPOTTER_FRAME_SAMPLE   480				// DSpotter compute every 30ms, it is 480 samples for 16KHz sampling rate.
#define DSPOTTER_FRAME_SIZE     DSPOTTER_FRAME_SAMPLE * 2       // 16 bits(two bytes) per sample.
#define AGC_VOLUME_SCALE 800					// The AGC volume scale percentage for recognition. It depends on original microphone data.
#define COMMAND_STAGE_TIME_MIN  6000				// When no result at command recognition stage, the minimum recording time in ms.
#define COMMAND_STAGE_TIME_MAX  8000				// When no result at command recognition stage, the maximum recording time in ms.
#define WAIKUP_COMMAND_TIMEOUT  4000

HANDLE InitRecognition(HANDLE hCybModel, int nGroupIndex, BYTE *lpbyDSpotterMem, int nDSpotterMemSize);
void ReleaseRecognition(HANDLE *phDSpotter);
void PrintGroupCommandList(HANDLE hCybModel, int nGroupIndex);

bool DSpotterEnabled = false;

extern OS_TASK template_task_h;
extern OS_TASK audio_task_h;

vad_stats_t vad_stats;

void VadKeywordDetection_task(void *pvParameters)
{
	HANDLE hCybModel = NULL;
	HANDLE hDSpotter = NULL;
	BYTE *lpbyDSpotterMem = NULL; // The memory for DSpotter engine.
	int nDSpotterMemSize = 0;
	int nGroupCount;
	int nActiveGroupIndex;
	int nRet = DSPOTTER_ERR_NeedMoreSample;
	int nRecordFrameCount = 0;
	uint8_t path_idx = 0;

	vad_stats.VDV = 0;
	vad_stats.NDV = 0;

	printf("\r\nDSpotter version: %s\r\n", DSpotter_VerInfo());

	// Initial model manager, list all commands, calculate the memory requirement.
	hCybModel = CybModelInit((const BYTE *)&uCYModel1Begin, NULL, 0, NULL);
	nGroupCount = CybModelGetGroupCount(hCybModel);
	for (int nGroup = 0; nGroup < nGroupCount; nGroup++)
	{
		BYTE *lppbyModel[1];
		int nMemSize;
		lppbyModel[0] = (BYTE *)CybModelGetGroup(hCybModel, nGroup);
		nMemSize = DSpotter_GetMemoryUsage_Multi((BYTE *)CybModelGetBase(hCybModel), lppbyModel, 1, MAX_COMMAND_TIME);
		printf("The DSpotter model group %d needed working memory size = %d\r\n", nGroup, nMemSize);
		if (nDSpotterMemSize < nMemSize)
			nDSpotterMemSize = nMemSize;

		printf("The DSpotter command list of group index %d: \r\n", nGroup);
		PrintGroupCommandList(hCybModel, nGroup);
	}
	printf("\r\n");

	// Allocate
	lpbyDSpotterMem = PortMalloc(nDSpotterMemSize);
	if (lpbyDSpotterMem == NULL)
	{
		printf("Fail to allocate %d memory for DSpotter.\r\n", nDSpotterMemSize);
		CybModelRelease(hCybModel);
		return;
	}

	nActiveGroupIndex = 0;
	hDSpotter = InitRecognition(hCybModel, nActiveGroupIndex, lpbyDSpotterMem, nDSpotterMemSize);
	if (hDSpotter == NULL)
	{
		CybModelRelease(hCybModel);
		PortFree(lpbyDSpotterMem);
		return;
	}
	DSpotterEnabled = true;
	int nCommandRecordSample = 0;
	int nCommandRecognizeLimit = COMMAND_STAGE_TIME_MIN;
	short *lpsRecordSample = PortMalloc(DSPOTTER_FRAME_SIZE);
#if defined(SUPPORT_UART_DUMP_RECORD) && (CONFIG_RETARGET_UART_BAUDRATE == HW_UART_BAUDRATE_921600)
	uint8_t *lpbyTxBuffere = NULL;
#endif
	uint32_t notif;

#ifndef VAD_PERIPH
	OS_TASK_NOTIFY(audio_task_h, AUDIO_START_NOTIF,  OS_NOTIFY_SET_BITS);
#endif

#ifdef dg_configLCD_GUI
	Lcd_text_api("");
#endif

	while (true)
	{
Audio_start:

                // Block until get AUDIO_DATA_NOTIF from analog_mic_mem_cb
                OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);

                if (notif & AUDIO_START_NOTIF){
                        AudioRecordStart(path_idx, OS_GET_CURRENT_TASK(), AUDIO_DATA_NOTIF, AUDIO_DATA_LOST_NOTIF); //55us from VAD CB
                        goto Audio_start;
                }
                if (notif & AUDIO_STOP_NOTIF){
                       AudioRecordStop(path_idx);
#ifdef VAD_PERIPH
                       re_enable_vad();
#endif
                       goto Audio_start;
                }

                if (notif & AUDIO_DATA_LOST_NOTIF)
                        printf("Warning: data lost!\r\n");



		nRecordFrameCount++;

		if(nRecordFrameCount % 20 == 0)
		        printf(".");

#ifdef USE_LEDS
		led_operation ( nRecordFrameCount,  nActiveGroupIndex);
#endif

// Get record data.
		AudioRecordGetData(lpsRecordSample, DSPOTTER_FRAME_SIZE); // 71us

// DSpotter AddSample // 4ms RAM,  9.6 ms FLASH, CLK 160, Highest priority DSpotter task
                __NOP();
#ifdef USE_MARKER_PIN
		hw_gpio_pad_latch_enable(MARKER_PIN1);
		hw_gpio_set_active(MARKER_PIN1);
#endif

		// DSpotter AddSample
		nRet = DSpotter_AddSample(hDSpotter, lpsRecordSample, DSPOTTER_FRAME_SAMPLE);
#ifdef USE_MARKER_PIN
		hw_gpio_set_inactive(MARKER_PIN1);
		hw_gpio_pad_latch_disable(MARKER_PIN1);
#endif
                __NOP();

//#ifdef USE_MARKER_PIN
//        hw_gpio_pad_latch_enable(MARKER_PIN0);
//        hw_gpio_set_active(MARKER_PIN0);
//        hw_gpio_pad_latch_disable(MARKER_PIN0);
//#endif
#if defined(SUPPORT_UART_DUMP_RECORD) && (CONFIG_RETARGET_UART_BAUDRATE == HW_UART_BAUDRATE_921600)     // 14.7ms RAM & FLASH
                if (lpbyTxBuffere == NULL)
                        lpbyTxBuffere = PortMalloc(DSPOTTER_FRAME_SIZE * 5 / 4);
                Convert2TransferBuffer((BYTE *)lpsRecordSample, DSPOTTER_FRAME_SIZE, lpbyTxBuffere, DSPOTTER_FRAME_SIZE * 5 / 4, eFourByteDataOneChecksum);
                hw_sys_pd_com_enable();
                HW_GPIO_PAD_LATCH_ENABLE(SER1_TX);
                HW_GPIO_SET_PIN_FUNCTION(SER1_TX);
                hw_uart_send(HW_UART2, lpbyTxBuffere, DSPOTTER_FRAME_SIZE * 5 / 4, NULL, NULL);
                while (hw_uart_is_busy(SER1_UART))
                {
                        __NOP();
                }
                HW_GPIO_PAD_LATCH_DISABLE(SER1_TX);
//                hw_sys_pd_com_disable();
#endif
//#ifdef USE_MARKER_PIN
//        hw_gpio_pad_latch_enable(MARKER_PIN0);
//        hw_gpio_set_inactive(MARKER_PIN0);
//        hw_gpio_pad_latch_disable(MARKER_PIN0);
//#endif
		if (nRet == DSPOTTER_SUCCESS)
		{
			char szCommand[64];
			int nCmdIndex = -1, nCmdScore, nCmdSGDiff, nCmdEnergy, nMapID = -1;

			DSpotter_GetResultScore(hDSpotter, &nCmdScore, &nCmdSGDiff, NULL);
			nCmdIndex = DSpotter_GetResult(hDSpotter);
			nCmdEnergy = DSpotter_GetCmdEnergy(hDSpotter);
			CybModelGetCommandInfo(hCybModel, nActiveGroupIndex, nCmdIndex, szCommand, sizeof(szCommand), &nMapID, NULL);
#ifdef NOT_SHOW_MULTI_PRONUNCIATION
			if (strchr(szCommand, '^') != NULL)
				strchr(szCommand, '^')[0] = '\0';
#endif
			printf("\r\nGet command(%d) : %s, Score=%d, SG_Diff=%d, Energy=%d, MapID=%d\r\n\r\n",
				   nCmdIndex, szCommand, nCmdScore, nCmdSGDiff, nCmdEnergy, nMapID);

			DSpotter_Reset(hDSpotter);

			nCommandRecordSample = 0;
			nCommandRecognizeLimit = COMMAND_STAGE_TIME_MIN;

			if (nActiveGroupIndex == 0 && nGroupCount > 1)
			{
#ifdef dg_configLCD_GUI
			        Lcd_text_api("Yes, I am listening...");
#endif
				// After trigger word recognized, switch to command recognition mode
				ReleaseRecognition(&hDSpotter);
				nActiveGroupIndex = 1;
				hDSpotter = InitRecognition(hCybModel, nActiveGroupIndex, lpbyDSpotterMem, nDSpotterMemSize);
			}
			else
			{
			        AudioRecordStop(path_idx);

				// After command word recognized, do something defined by user.
				switch (nMapID)
				{
				case 701:
				        printf("\r\nLights ON");
#ifdef dg_configLCD_GUI
				        Lcd_light_api(true);
#endif
#ifdef USE_LEDS
				        leds_off();
				        toggle_db_led(HW_LED_MSK_LED_1,true);
				        hw_clk_delay_usec(500000);
                                        toggle_db_led(HW_LED_MSK_LED_1,false);
#endif
				        break;
				case 702:
					printf("\r\nLights OFF");
#ifdef dg_configLCD_GUI
					Lcd_light_api(false);
#endif
#ifdef USE_LEDS
                                        leds_off();
                                        toggle_db_led(HW_LED_MSK_LED_3,true);
                                        hw_clk_delay_usec(500000);
                                        toggle_db_led(HW_LED_MSK_LED_3,false);
#endif
					break;
				default:
					printf("\r\nUNKNOWN COMMAND");
					break;
				}
				vad_stats.VDV++;
				print_stats();

				ReleaseRecognition(&hDSpotter);
				nActiveGroupIndex = 0;
				hDSpotter = InitRecognition(hCybModel, nActiveGroupIndex, lpbyDSpotterMem, nDSpotterMemSize);
				nRecordFrameCount = 0;
#ifdef VAD_PERIPH
				re_enable_vad();
#else
				AudioRecordStart(path_idx, OS_GET_CURRENT_TASK(), AUDIO_DATA_NOTIF, AUDIO_DATA_LOST_NOTIF);
#endif
			}
		} //if (nRet == DSPOTTER_SUCCESS)

		else if (nRet == DSPOTTER_ERR_Expired)
		{
			printf("\r\nThe trial version DSpotter reach the max trial usage count, please reset system.\r\n");
			break;
		}

		else if (nRet == DSPOTTER_ERR_NeedMoreSample && nActiveGroupIndex == 1)
		{
			// Check timeout for command recognition mode
			nCommandRecordSample += (int)(DSPOTTER_FRAME_SIZE / sizeof(short));     // +480
			if (nCommandRecordSample > 16000 / 1000 * nCommandRecognizeLimit)       // 96000
			{
				if (DSpotter_IsKeywordAlive(hDSpotter))                         // Give extra time to spot command if detection is in progress.
				{
					if (nCommandRecognizeLimit < COMMAND_STAGE_TIME_MAX)
					{
						nCommandRecognizeLimit += 500;
						continue;
					}
				}

				AudioRecordStop(path_idx);
				printf("\r\nTimeout for command stage, switch to trigger stage.\r\n");
				vad_stats.NDV++;
				print_stats();
				nCommandRecognizeLimit = COMMAND_STAGE_TIME_MIN;

				ReleaseRecognition(&hDSpotter);
				nActiveGroupIndex = 0;
				hDSpotter = InitRecognition(hCybModel, nActiveGroupIndex, lpbyDSpotterMem, nDSpotterMemSize);
				nRecordFrameCount = 0;
#ifdef USE_LEDS
				leds_off();
                                toggle_db_led(HW_LED_MSK_LED_2,true);
                                hw_clk_delay_usec(500000);
                                toggle_db_led(HW_LED_MSK_LED_2,false);
#endif
#ifdef dg_configLCD_GUI
//                                Lcd_text_api("Timeout");
#endif

#ifdef VAD_PERIPH
                                re_enable_vad();
#else
                                AudioRecordStart(path_idx, OS_GET_CURRENT_TASK(), AUDIO_DATA_NOTIF, AUDIO_DATA_LOST_NOTIF);
#endif
			}
		}

#ifdef VAD_PERIPH
		else if (nRet == DSPOTTER_ERR_NeedMoreSample && nActiveGroupIndex == 0) // VAD active but no wake-up is spoken
		{
		        if(30*nRecordFrameCount > WAIKUP_COMMAND_TIMEOUT){        // seconds to wait for command
#ifdef dg_configLCD_GUI
//		                Lcd_text_api("Timeout");
#endif
		                printf("\r\nComTimeOut");
		                vad_stats.NDV++;
		                print_stats();
		                AudioRecordStop(path_idx);
		                ReleaseRecognition(&hDSpotter);
                                nActiveGroupIndex = 0;
                                hDSpotter = InitRecognition(hCybModel, nActiveGroupIndex, lpbyDSpotterMem, nDSpotterMemSize);
                                nRecordFrameCount = 0;
#ifdef USE_LEDS
                                leds_off();
		                toggle_db_led(HW_LED_MSK_LED_2,true);
		                hw_clk_delay_usec(500000);
		                toggle_db_led(HW_LED_MSK_LED_2,false);
#endif
		                re_enable_vad();
		        } // Timeout

		}
		else
		{
		        __NOP();
		}
#endif //VAD_PERIPH

	} //while

	DSpotterEnabled = false;
	AudioRecordStop(path_idx);
	PortFree(lpsRecordSample);
#if defined(SUPPORT_UART_DUMP_RECORD) && (CONFIG_RETARGET_UART_BAUDRATE == HW_UART_BAUDRATE_921600)
	PortFree(lpbyTxBuffere);
#endif

	ReleaseRecognition(&hDSpotter);
	CybModelRelease(hCybModel);
	PortFree(lpbyDSpotterMem);
#ifdef dg_configLCD_GUI
	Lcd_text_api("Restart DSpotter");
#endif
	printf("\r\nTerminate DSpotter\r\n");
	goto Audio_start; // Since audio is terminated, it will simply block on start

}

HANDLE InitRecognition(HANDLE hCybModel, int nGroupIndex, BYTE *lpbyDSpotterMem, int nDSpotterMemSize)
{
	int nRet;
	BYTE *lppbyModel[1];
	HANDLE hDSpotter;

	lppbyModel[0] = (BYTE *)(BYTE *)CybModelGetGroup(hCybModel, nGroupIndex);
	hDSpotter = DSpotter_Init_Multi((BYTE *)CybModelGetBase(hCybModel), lppbyModel, 1, MAX_COMMAND_TIME,
									lpbyDSpotterMem, nDSpotterMemSize, NULL, 0, &nRet);
	if (hDSpotter == NULL)
	{
		printf("DSpotter_Init_Multi() fail, error = %d!\r\n", nRet);
		return NULL;
	}

	if (nGroupIndex == 0)
	{
	    int nCommandCount = CybModelGetCommandCount(hCybModel, nGroupIndex);

	    DSpotter_SetOptionBeginStateTime(hDSpotter, 2);
	    for (int i = 0; i < nCommandCount; i++)
	        DSpotter_SetWordOptionBeginState(hDSpotter, i, 6);
	}

//	printf("\r\n%s group active.\r\n", nGroupIndex == 0 ? "Wake-up" : "Command");
//	PrintGroupCommandList(hCybModel, nGroupIndex);

	// If the recording volume is too low, please use the following API to amplify it.
	if (AGC_VOLUME_SCALE != 100)
	{
		DSpotterAGC_Enable(hDSpotter);
		DSpotterAGC_SetMaxGain(hDSpotter, AGC_VOLUME_SCALE / 100.0);
	}

	return hDSpotter;
}

void ReleaseRecognition(HANDLE *phDSpotter)
{
	if (phDSpotter != NULL && *phDSpotter != NULL)
	{
		DSpotter_Release(*phDSpotter);
		*phDSpotter = NULL;
	}
}

void PrintGroupCommandList(HANDLE hCybModel, int nGroupIndex)
{
	char szCommand[64];
	int nMapID;

	for (int i = 0; i < CybModelGetCommandCount(hCybModel, nGroupIndex); i++)
	{
		CybModelGetCommandInfo(hCybModel, nGroupIndex, i, szCommand, sizeof(szCommand), &nMapID, NULL);
		if (strlen(szCommand) > 0)
		{
#ifdef NOT_SHOW_MULTI_PRONUNCIATION
			if (strchr(szCommand, '^') != NULL)
				continue;
#endif
			printf("    %s, Map ID = %d\r\n", szCommand, nMapID);
		}
	}
	printf("\r\n");
}
