/**
 ****************************************************************************************
 *
 * @file main_demo_screen.c
 *
 * @brief Main demo screen source file
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
/*
 *      INCLUDES
 *****************************************************************************************
 */
#include "resources.h"

/*
 *      DEFINES
 *****************************************************************************************
 */

/*
 *      TYPEDEFS
 *****************************************************************************************
 */

/*
 *  STATIC PROTOTYPES
 *****************************************************************************************
 */

/*
 *  STATIC VARIABLES
 *****************************************************************************************
 */
static lv_obj_t * img_obj, *txt_label;

/*
 *      MACROS
 *****************************************************************************************
 */
LV_IMG_DECLARE(lights_on_img);
LV_IMG_DECLARE(lights_off_img);

/*
 *   GLOBAL FUNCTIONS
 *****************************************************************************************
 */
void ui_set_img_src(bool light_on)
{
        const void * src;

        if (light_on)
                src = &lights_on_img;
        else
                src = &lights_off_img;

        lv_img_set_src(img_obj, src);
        lv_obj_invalidate(img_obj);
}

void ui_set_txt(char *txt)
{
        lv_label_set_text_fmt(txt_label, "#FFFFFF %s", txt);
        lv_obj_invalidate(txt_label);
}

void main_demo_screen()
{
        lv_obj_t * main_screen_obj;
        lv_coord_t y_ofs = 50;

        main_screen_obj = lv_obj_create(NULL);
        lv_obj_set_style_bg_color(main_screen_obj, lv_color_black(), LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(main_screen_obj, LV_SCROLLBAR_MODE_OFF);

        img_obj = lv_img_create(main_screen_obj);
        ui_set_img_src(false);
        lv_obj_set_pos(img_obj, (DEMO_RESX - lights_on_img.header.w)/2, y_ofs);

        txt_label = lv_label_create(main_screen_obj);
        lv_label_set_recolor(txt_label, true);
        lv_obj_set_style_text_font(txt_label, &lv_font_montserrat_32, 0);
        ui_set_txt("Init...");
        y_ofs = (lights_on_img.header.h + (DEMO_RESY - lights_on_img.header.h) / 2) - DEMO_RESY/2;
        lv_obj_align(txt_label, LV_ALIGN_CENTER, 0, y_ofs);

        lv_scr_load(main_screen_obj);
}

/*
 *   STATIC FUNCTIONS
 *****************************************************************************************
 */

