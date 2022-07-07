/**
 ****************************************************************************************
 *
 * @file resources.h
 *
 * @file resources.c
 *
 * @brief Resources - bitmaps stored in flash source file
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
#include "resources.h"

#define RESOURCES_BASE_ADDRESS  (MEMORY_QSPIC_BASE)

#define HEADER_OFFSET           (sizeof(lv_img_header_t))

#define LIGHTS_ON_BITMAP_SIZE           (0xF574)
#define LIGHTS_OFF_BITMAP_SIZE          (0xF574)

#define LIGHTS_ON_BITMAP_OFFSET         (0x0)
#define LIGHTS_OFF_BITMAP_OFFSET        (LIGHTS_ON_BITMAP_SIZE)

const lv_img_dsc_t lights_on_img = {
        .header.always_zero = 0,
        .header.w = 102,
        .header.h = 154,
        .data_size = LIGHTS_ON_BITMAP_SIZE,
        .header.cf = LV_IMG_CF_ARGB8888,
        .data = (uint8_t*) RESOURCES_BASE_ADDRESS + LIGHTS_ON_BITMAP_OFFSET + HEADER_OFFSET, // Pointer to picture data
};

const lv_img_dsc_t lights_off_img = {
        .header.always_zero = 0,
        .header.w = 102,
        .header.h = 154,
        .data_size = LIGHTS_OFF_BITMAP_SIZE,
        .header.cf = LV_IMG_CF_ARGB8888,
        .data = (uint8_t*) RESOURCES_BASE_ADDRESS + LIGHTS_OFF_BITMAP_OFFSET + HEADER_OFFSET, // Pointer to picture data
};
