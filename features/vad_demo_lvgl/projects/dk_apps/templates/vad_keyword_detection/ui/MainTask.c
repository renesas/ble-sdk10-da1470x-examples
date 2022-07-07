/**
 ****************************************************************************************
 *
 * @file MainTask.c
 *
 * @brief Watch Demo application
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
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "osal.h"
#include "lvgl.h"
#include "main_demo_screen.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"

/*
 *       Defines
 *****************************************************************************************
 */

/*
 *       Static
 *
 *****************************************************************************************
 */

/*
 *       Static code
 *****************************************************************************************
 */

/*
 *       Public
 *****************************************************************************************
 */
bool lights_on = false;
char *ui_txt = "";

/*
 *       Public code
 *****************************************************************************************
 */
/**
 * @brief  Start task
 * @param  pvParameters: pointer that is passed to the thread function as start argument.
 */
OS_TASK_FUNCTION(GUITask, pvParameters)
{
        /* Initialize GUI */
        lv_init();

        /* Initialize display driver */
        lv_port_disp_init();

        /* Initialize input driver */
        lv_port_indev_init();

        main_demo_screen();

        while (1) {
                uint32_t sleep_time, notif;

                sleep_time = lv_task_handler();

                if (LV_NO_TIMER_READY == sleep_time) {
                        sleep_time = OS_TASK_NOTIFY_NO_WAIT;
                }

                OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_MS_2_TICKS(sleep_time));
                if ((notif & UI_CHANGE_IMG_EVT)){
                        int16_t lights_on_loc;

                        OS_ENTER_CRITICAL_SECTION();
                        lights_on_loc = lights_on;
                        OS_LEAVE_CRITICAL_SECTION();

                        ui_set_img_src(lights_on_loc);
                }

                if ((notif & UI_CHANGE_TEXT_EVT)){
                        char *ui_txt_loc;

                        OS_ENTER_CRITICAL_SECTION();
                        ui_txt_loc = ui_txt;
                        OS_LEAVE_CRITICAL_SECTION();

                        ui_set_txt(ui_txt_loc);
                }
        }
}

