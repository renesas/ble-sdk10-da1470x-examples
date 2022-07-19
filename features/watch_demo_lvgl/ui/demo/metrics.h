/**
 ****************************************************************************************
 *
 * @file metrics.h
 *
 * @brief Performance Metrics module header
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
#ifndef METRICS_H_
#define METRICS_H_

#include <stdio.h>
#include <stdbool.h>

#define METRICS_TAG_NO_LOGGING                  (0)
#define METRICS_TAG_TICK_ROTATION               (1)
#define METRICS_TAG_SLIDING_WATCH_FACE_TO_MENU  (2)
#define METRICS_TAG_BROWSE_MENU                 (3)
#define METRICS_TAG_TIMER_RUNNING               (4)
#define METRICS_TAG_COMPASS_ROTATION            (5)

#define GPU_METRICS_FILL                (1)
#define GPU_METRICS_BLITBITMAP          (2)
#define GPU_METRICS_ROTATEIMAGE         (3)
#define GPU_METRICS_MAX_TAG             (3)

typedef struct {
        uint8_t tag;
        int fps;
        int frame_rendering_time;
        int display_transfer_time;
        int pixel_count;
        int gpu_data[GPU_METRICS_MAX_TAG];
} METRICS;

void metrics_init(void);
void metrics_add(METRICS *metric);
void metrics_set_tag(uint8_t tag);
METRICS get_metrics_data();
void metrics_gpu_add(int gpu_rendering_time);
void metrics_set_gpu_tag(uint8_t tag);
void metrics_register_tag(uint8_t tag, const char *tag_name);
void metrics_print(void);

#endif /* METRICS_H_ */
