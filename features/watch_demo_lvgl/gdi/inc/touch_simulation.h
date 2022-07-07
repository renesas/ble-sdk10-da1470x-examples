/**
 ****************************************************************************************
 *
 * @file touch_simulation.h
 *
 * @brief Touch configuration for GDI
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
#ifndef TOUCH_SIMULATION_H_
#define TOUCH_SIMULATION_H_

#include <stdio.h>
#include <stdint.h>

#if dg_configUSE_TOUCH_SIMULATION

#define GDI_TOUCH_ENABLE                (1) /* i.e. no device to init by GDI */
#define GDI_TOUCH_READ_EVENT            (touch_simulation_read_event)

typedef enum {
        SLIDE_LEFT_TO_RIGHT,
        SLIDE_RIGHT_TO_LEFT,
        SLIDE_MENU_UP_TO_DOWN,
        SLIDE_MENU_DOWN_TO_UP,
        TOUCH_TIMER_MENU,
        TOUCH_TIMER_START,
        TOUCH_ACTIVITY_MENU,
        TOUCH_COMPASS_MENU,
        SLIDE_BACK,
} GESTURE_TYPE;

void touch_simulation_set_data(GESTURE_TYPE type);

/* User-defined routine used by GDI to read touch events */
void touch_simulation_read_event(void *dev, gdi_touch_data_t *data);
void touch_simulation(GESTURE_TYPE type);

#endif /* dg_configUSE_TOUCH_SIMULATION */

#endif /* TOUCH_SIMULATION_H_ */
