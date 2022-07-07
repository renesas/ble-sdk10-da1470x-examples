/**
 ****************************************************************************************
 *
 * @file Helper_func.h
 *
 * @brief Helper functions header file
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
#ifndef HELPER_FUNC_H_
#define HELPER_FUNC_H_

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "osal.h"
#include "hw_pdc.h"
#include "hw_vad.h"
#include "periph_setup.h"
#include "cmsis_gcc.h"

#include "hw_led.h"

typedef struct {
        uint16_t WakeUp;
        uint16_t VDV;
        uint16_t NDV;
        uint32_t Period;
} vad_stats_t;

void re_enable_vad(void);
void vad_init(void);
void vad_power_down(void);

void init_led(void);
void leds_off(void);
void toggle_db_led(HW_LED_ID led, bool state);
void led_operation (int nRecordFrameCount, int nActiveGroupIndex);
void test_leds(void);
void print_stats(void);

#ifdef dg_configLCD_GUI
void Lcd_text_api(char *text);
void Lcd_light_api(bool state);
#endif

#endif /* HELPER_FUNC_H_ */
