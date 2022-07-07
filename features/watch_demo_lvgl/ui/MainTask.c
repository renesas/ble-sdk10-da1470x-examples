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
#include "gdi.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "init_screens.h"
#include "screens/compass_screen.h"

/*
 *       Defines
 *****************************************************************************************
 */
#define MAGNETIC_DATA_EVT       (1 << 3)

/*
 *       Static
 *
 *****************************************************************************************
 */
static INITIALISED_PRIVILEGED_DATA OS_TASK gui_task_h = NULL;
static int16_t mag_data;

/*
 *       Static code
 *****************************************************************************************
 */

/**
 * @brief  Start task
 * @param  pvParameters: pointer that is passed to the thread function as start argument.
 */
static OS_TASK_FUNCTION(GUIThread, pvParameters)
{
        /* Initialize GUI */
        lv_init();

        /* Initialize display driver */
        lv_port_disp_init();

        /* Initialize input driver */
        lv_port_indev_init();

        create_basic_screens();

        while (1) {
                uint32_t notif, sleep_time;

                sleep_time = lv_task_handler();

                if (LV_NO_TIMER_READY == sleep_time) {
                        sleep_time = OS_TASK_NOTIFY_NO_WAIT;
                }

                OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_MS_2_TICKS(sleep_time));
                if ((notif & MAGNETIC_DATA_EVT)){
                        int16_t mag_data_loc;
                        OS_ENTER_CRITICAL_SECTION();
                        mag_data_loc = mag_data;
                        OS_LEAVE_CRITICAL_SECTION();

                        compass_rotate(mag_data_loc);
                }
        }

}

/*
 *       Public code
 *****************************************************************************************
 */
void compass_event(int16_t *data)
{
        OS_ENTER_CRITICAL_SECTION();
        mag_data = *data;
        OS_LEAVE_CRITICAL_SECTION();

        OS_TASK_NOTIFY(gui_task_h, MAGNETIC_DATA_EVT, OS_NOTIFY_SET_BITS);
}

/*
 *       MainTask
 *****************************************************************************************
 */
void MainTask(void)
{
        OS_TASK_CREATE("GUI thread",    /* The text name assigned to the task, for
                                           debug only; not used by the kernel. */
                       GUIThread,       /* The function that implements the task. */
                       NULL,            /* The parameter passed to the task. */
                       3048,            /* The number of bytes to allocate to the
                                           stack of the task. */
                       OS_TASK_PRIORITY_NORMAL,         /* The priority assigned to the task. */
                       gui_task_h);     /* The task handle */
        OS_ASSERT(gui_task_h);
}

