/**
 ****************************************************************************************
 *
 * @file AudioRecord.c
 *
 * @brief Audio related functions
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

#include <stdbool.h>
#include "include/base_types.h"
#include "include/RingBuffer.h"

#include "periph_setup.h"
#include "osal.h"
#include "sys_audio_mgr.h"
#include "Helper_func.h"

#define DSPOTTER_FRAME_SAMPLE    480                     // DSpotter compute every 30ms, it is 480 samples for 16KHz sampling rate.
#define DSPOTTER_FRAME_SIZE      DSPOTTER_FRAME_SAMPLE*2 // 16 bits(two bytes) per sample.

// Only 16KHz sampling rate is supported for the analog microphone. If AUDIO_SAMPLING_RATE is set to a different
// value, then an SRC unit will be used to convert the audio rate.
#define AUDIO_SAMPLING_RATE             16000

// The output of the ADC is 16bit wide.
#define AUDIO_SAMPLE_BIT_DEPTH          16

// Define how may bytes must be stored in the memory buffer before the callback is called.
#define MEMORY_BUFFER_BUF_CB_SAMPLE_LEN DSPOTTER_FRAME_SAMPLE//(AUDIO_SAMPLING_RATE / 100) // every 10msec, 100 times per second

// The size of the buffer in samples
#define MEMORY_BUFFER_SAMPLE_SIZE       (MEMORY_BUFFER_BUF_CB_SAMPLE_LEN * 2)


#define RBUF_SIZE                (DSPOTTER_FRAME_SIZE*6)  // Ring buffer size.

static HANDLE g_hRingBuffer = NULL;
static BYTE   g_byaRingBuffer[RING_BUFFER_GET_MEM_USAGE(RBUF_SIZE)];// The working memory for ring buffer.

#if (AUDIO_SAMPLE_BIT_DEPTH == 32)
#define AUDIO_DATA_TYPE int32_t
#elif (AUDIO_SAMPLE_BIT_DEPTH == 16)
#define AUDIO_DATA_TYPE int16_t
#elif (AUDIO_SAMPLE_BIT_DEPTH == 8)
#define AUDIO_DATA_TYPE int8_t
#endif

// MEMS MIC samples provided by the ADC.
static AUDIO_DATA_TYPE analog_mic_buffer[MEMORY_BUFFER_SAMPLE_SIZE];

#if (MEMORY_BUFFER_SAMPLE_SIZE > 0xFFFF)
#error "Audio buffer size cannot exceed 0xFFFF due to the DMA length limitation"
#endif

// This callback is called whenever MEMORY_BUFFER_BUF_CB_SAMPLE_LEN samples have been transfered in the MEMS MIC memory buffer
static void analog_mic_mem_cb(sys_audio_mgr_buffer_data_block_t *buff_data_block, void *app_ud);
/*
 * Initialize devices related with Audio
 */
static sys_audio_device_t analog_mic_in = {
        // Select SDADC interface as input of path 3
        .device_type = AUDIO_SDADC,
        .sdadc_param = {
                .pga_gain = HW_SDADC_PGA_GAIN_30dB,     // PGA gain must be adjusted according the the requirements of the
                                                        // application and the specifications of the microphone.
                .pga_mode = HW_SDADC_PGA_MODE_DIFF,     // This is the recommended mode for best performance.
        }
};

static sys_audio_device_t analog_mic_mem_out = {
        // Select analog_mic_buffer as output of path 3
        .device_type = AUDIO_MEMORY,
        .memory_param = {
                .app_ud                 = 0,
                .bit_depth              = AUDIO_SAMPLE_BIT_DEPTH,
                .cb_buffer_len          = MEMORY_BUFFER_BUF_CB_SAMPLE_LEN * sizeof(AUDIO_DATA_TYPE),    // cb_buffer_len is in bytes
                .cb                     = analog_mic_mem_cb,
#if (AUDIO_SAMPLING_RATE == 16000)
                // Requested sampling rate is supported by the SDADC. SRC is not used. An odd numbered DMA channel must be used
                // to get the data from the SDADC. Even numbered channels are triggered by the GPADC.
                .dma_channel[0]         = HW_DMA_CHANNEL_1,
#else
                // Requested sampling rate is not supported by the SDADC. SRC must be used. An even numbered DMA channel must be used
                // to get the data from SRC Rx registers. Odd numbered channels are triggered by SRC Tx path.
                .dma_channel[0]         = HW_DMA_CHANNEL_0,
#endif
                .dma_channel[1]         = HW_DMA_CHANNEL_INVALID,                                       // This channel is not used
                .buff_addr[0]           = (uint32_t)analog_mic_buffer,
                //.buff_addr[1]           = 0,                                                          // buff_addr[1] is not needed because
                .sample_rate            = AUDIO_SAMPLING_RATE,
                .stereo                 = false,
                .total_buffer_len       = MEMORY_BUFFER_SAMPLE_SIZE * sizeof(AUDIO_DATA_TYPE),          // total_buffer_len is in bytes
                .circular               = true,
        }
};

//// Define the audio paths
//static const sys_audio_path_t paths_cfg = {
//        // MEMS MIC -> Memory path
//        .audio_path[0] = {
//                .dev_in  = (sys_audio_device_t*)&analog_mic_in,
//                .dev_out = (sys_audio_device_t*)&analog_mic_mem_out,
//        },
//};

static OS_TASK audio_task_handle;
static int g_nAudioDataNotif, g_nAudioDataLostNotif;

static int AudioRecordPutData(void *lpData, int nSize);


static void analog_mic_mem_cb(sys_audio_mgr_buffer_data_block_t *buff_data_block, void *app_ud)
{
    volatile uint32_t nDataStartPos;

    if (buff_data_block->buff_len_pos > 0)
        nDataStartPos = buff_data_block->buff_len_pos - buff_data_block->buff_len_cb;
    else
        nDataStartPos = buff_data_block->buff_len_total - buff_data_block->buff_len_cb;

    if (AudioRecordPutData((void *)(buff_data_block->address + nDataStartPos), buff_data_block->buff_len_cb) == 0)
        OS_TASK_NOTIFY_FROM_ISR(audio_task_handle, g_nAudioDataNotif, OS_NOTIFY_SET_BITS);
    else
        OS_TASK_NOTIFY_FROM_ISR(audio_task_handle, g_nAudioDataLostNotif, OS_NOTIFY_SET_BITS);

#ifdef USE_MARKER_PIN
        hw_gpio_pad_latch_enable(MARKER_PIN2);
        hw_gpio_set_active(MARKER_PIN2);
        __NOP();
        hw_gpio_set_inactive(MARKER_PIN2);
        hw_gpio_pad_latch_disable(MARKER_PIN2);
#endif
}

extern bool DSpotterEnabled;

bool AudioRecordStart(int path_idx, OS_TASK task_handle, int nAudioDataNotif, int nAudioDataLostNotif)
{
//        printf("\r\nStart Audio");
        bool retval = false;

        RingBufferInit(g_byaRingBuffer, RING_BUFFER_GET_MEM_USAGE(RBUF_SIZE), RBUF_SIZE, &g_hRingBuffer);
        if (g_hRingBuffer == NULL)
        {
                printf("RingBufferInit() fail!\r\n");
                return false;
        }

        audio_task_handle = task_handle;
        g_nAudioDataNotif = nAudioDataNotif;
        g_nAudioDataLostNotif = nAudioDataLostNotif;

#if (!DEVICE_FPGA)
    #if dg_configUSE_SYS_AUDIO_SINGLE_PATH
        path_idx = sys_audio_mgr_open_single(&analog_mic_in, &analog_mic_mem_out, SRC_AUTO);
    #else
        // Open audio interfaces of audio for the required paths
        path_idx = sys_audio_mgr_open_path(&analog_mic_in, &analog_mic_mem_out, SRC_AUTO); //sys_audio_mgr_open((sys_audio_path_t*)&paths_cfg);
    #endif
        // Start Analog MIC path
//      printf("\n\r>>> Selected start PATH_%d <<<", path_idx+1);

        // Enable devices of the required path
        retval = sys_audio_mgr_start(path_idx);

        ASSERT_ERROR(retval);

        // Sleep is blocked when DMA is active. No need to block it again.
        // Sleep mode will be restored when audio path is stopped and DMA channels are disabled.
#endif
#ifdef USE_MARKER_PIN
        hw_gpio_pad_latch_enable(MARKER_PIN0);
        hw_gpio_set_active(MARKER_PIN0);
        hw_gpio_pad_latch_disable(MARKER_PIN0);
#endif
#ifdef dg_configLCD_GUI
//        Lcd_text_api("Listening...");
#endif
        return retval;
}

void AudioRecordStop(int path_idx)
{
//        printf("\r\nStop Audio");
#if (!DEVICE_FPGA)
//        printf("\n\r>>> Selected stop PATH_%d <<<", path_idx+1);
        sys_audio_mgr_stop(path_idx);
    #if dg_configUSE_SYS_AUDIO_SINGLE_PATH
        sys_audio_mgr_close_single(path_idx);
    #else
        sys_audio_mgr_close_path(path_idx); //sys_audio_mgr_close();
    #endif
#endif
        RingBufferRelease(g_hRingBuffer);
        g_hRingBuffer = NULL;
#ifdef USE_MARKER_PIN
        hw_gpio_pad_latch_enable(MARKER_PIN0);
        hw_gpio_set_inactive(MARKER_PIN0);
        hw_gpio_pad_latch_disable(MARKER_PIN0);
#endif
#ifdef dg_configLCD_GUI
        extern OS_TASK template_task_h;
        if (DSpotterEnabled){
                OS_TASK_NOTIFY(template_task_h, LCD_TIMEOUT_NOTIF, OS_NOTIFY_SET_BITS);
        }
#endif
}

int AudioRecordGetDataSize()
{
    return (RingBufferGetDataSize(g_hRingBuffer));
}

int AudioRecordGetData(void *lpData, int nSize)
{
    return RingBufferGetData(g_hRingBuffer, lpData, nSize);
}

static int AudioRecordPutData(void *lpData, int nSize)
{
        if (RingBufferGetFreeSize(g_hRingBuffer) < nSize)
                return RING_BUFFER_ERR_OUT_OF_MEMORY;

        // Put record data to ring buffer, it must be 16KHz, 16 bits, mono channel PCM format.
        return RingBufferPutData(g_hRingBuffer, lpData, nSize);
}
