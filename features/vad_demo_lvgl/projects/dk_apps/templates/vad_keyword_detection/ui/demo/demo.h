/**
 ****************************************************************************************
 *
 * @file demo.h
 *
 * @brief Demo header file
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
#ifndef __DEMO_H
#define __DEMO_H

#define FB_COLOR_FORMAT                 (CF_NATIVE_RGB565)
#define GDI_FB_USE_QSPI_RAM             (0)

#define ORIGINAL                        (1)
#define OPTIMAL                         (2)
#define NO_GPU                          (3)
#define SCENARIO                        (OPTIMAL)

#define TWO_LAYERS_HORIZONTAL_SLIDING   (0)
#define MAX_TEMP_BUFFER_SIZE            (8 * 1024)
#define DEMO_GUI_HEAP_SIZE              (6 * 1024 + MAX_TEMP_BUFFER_SIZE * 2)

#ifndef PERFORMANCE_METRICS
#define LV_PORT_INDEV_TOUCH_QUEUE_EN    (0)
#endif

#endif /*__DEMO_H */
