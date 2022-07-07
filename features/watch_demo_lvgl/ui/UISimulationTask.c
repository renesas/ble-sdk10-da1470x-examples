/**
 ****************************************************************************************
 *
 * @file UISimulationTask.c
 *
 * @brief UI Simulation module
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
#include "gdi.h"
#include "metrics.h"

extern volatile uint8_t current_tag;

/*
 *       Defines
 *****************************************************************************************
 */
#define DELAY_BETWEEN_EACH_STEP                   (2000)

/*
 *       Static
 *****************************************************************************************
 */
static INITIALISED_PRIVILEGED_DATA OS_TASK metrics_task_h = NULL;

/*
 *       Static code
 *****************************************************************************************
 */
/**
 * @brief  UI simulation task
 * @param  argument: pointer that is passed to the thread function as start argument.
 */
static OS_TASK_FUNCTION(ui_simulation_thread, argument)
{
        static bool metrics_print_flag = true;

        metrics_init();

        printf("UI simulation task\r\n");
        OS_DELAY(OS_MS_2_TICKS(8000)); //8s

        printf("--------------- START UI simulation --------------- \r\n");

        while (1) {
                metrics_set_tag(METRICS_TAG_TICK_ROTATION);
                OS_DELAY(OS_MS_2_TICKS(5000)); //5s
                metrics_set_tag(METRICS_TAG_NO_LOGGING);

                // swiping from clock face to menu
                touch_simulation(SLIDE_LEFT_TO_RIGHT);
                OS_DELAY(OS_MS_2_TICKS(DELAY_BETWEEN_EACH_STEP));
                while (current_tag != 0) {
                        OS_DELAY(1);
                }

                // swiping the menu down
                metrics_set_tag(METRICS_TAG_BROWSE_MENU);
                touch_simulation(SLIDE_MENU_UP_TO_DOWN);
                OS_DELAY(OS_MS_2_TICKS(DELAY_BETWEEN_EACH_STEP));

                // swiping the menu up
                touch_simulation(SLIDE_MENU_DOWN_TO_UP);
                OS_DELAY(OS_MS_2_TICKS(DELAY_BETWEEN_EACH_STEP));
                metrics_set_tag(METRICS_TAG_NO_LOGGING);

                // touch timer menu
                touch_simulation(TOUCH_TIMER_MENU);
                OS_DELAY(OS_MS_2_TICKS(DELAY_BETWEEN_EACH_STEP));

                // touch timer start
                touch_simulation(TOUCH_TIMER_START);
                metrics_set_tag(METRICS_TAG_TIMER_RUNNING);
                OS_DELAY(OS_MS_2_TICKS(DELAY_BETWEEN_EACH_STEP));
                metrics_set_tag(METRICS_TAG_NO_LOGGING);

                // stop timer
                touch_simulation(TOUCH_TIMER_START);
                OS_DELAY(OS_MS_2_TICKS(1000));

                // back to menu screen
                touch_simulation(SLIDE_BACK);
                OS_DELAY(OS_MS_2_TICKS(DELAY_BETWEEN_EACH_STEP));

                // touch activity menu
                touch_simulation(TOUCH_ACTIVITY_MENU);
                OS_DELAY(OS_MS_2_TICKS(DELAY_BETWEEN_EACH_STEP));
                touch_simulation(SLIDE_MENU_UP_TO_DOWN);
                OS_DELAY_MS(1);
                touch_simulation(SLIDE_MENU_UP_TO_DOWN);
                OS_DELAY(OS_MS_2_TICKS(DELAY_BETWEEN_EACH_STEP));

                // back to menu screen
                touch_simulation(SLIDE_BACK);
                OS_DELAY(OS_MS_2_TICKS(DELAY_BETWEEN_EACH_STEP));

                // touch compass
                touch_simulation(TOUCH_COMPASS_MENU);
                OS_DELAY(OS_MS_2_TICKS(DELAY_BETWEEN_EACH_STEP));
                while (current_tag != 0) {
                        OS_DELAY(1);
                }

                if (metrics_print_flag) {
                        printf("--------------- END UI simulation ---------------\r\n");
                        metrics_print();
                        metrics_print_flag = false;
                }

                // back to menu screen
                touch_simulation(SLIDE_BACK);
                OS_DELAY(OS_MS_2_TICKS(DELAY_BETWEEN_EACH_STEP));

                // back to watch screen
                touch_simulation(SLIDE_RIGHT_TO_LEFT);
                OS_DELAY(OS_MS_2_TICKS(DELAY_BETWEEN_EACH_STEP));
                while (current_tag != 0) {
                        OS_DELAY(1);
                }
        }
}

/*
 *       Public code
 *****************************************************************************************
 */
/*
 *       MainTask
 */
void UISimulationTask(void)
{
        OS_TASK_CREATE("Sim thread",            /* The text name assigned to the task, for
                                                   debug only; not used by the kernel. */
                       ui_simulation_thread,    /* The function that implements the task. */
                       NULL,                    /* The parameter passed to the task. */
                       1048,                    /* The number of bytes to allocate to the
                                                   stack of the task. */
                       OS_TASK_PRIORITY_NORMAL, /* The priority assigned to the task. */
                       metrics_task_h);         /* The task handle */
        OS_ASSERT(metrics_task_h);
}
