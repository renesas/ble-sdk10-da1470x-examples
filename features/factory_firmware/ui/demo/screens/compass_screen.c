/**
 ****************************************************************************************
 *
 * @file compass_screen.c
 *
 * @brief Compass screen source file
 *
 * Copyright (C) 2021-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

/*
 *      INCLUDES
 *****************************************************************************************
 */
#include <stdio.h>
#include "Resources.h"

extern void start_compass_data();
extern void stop_compass_data();

/*
 *  STATIC PROTOTYPES
 *****************************************************************************************
 */
static void show_compass_screen();
#if COMPASS_ROTATION_USES_CANVAS
static void compass_delete_event_cb(lv_event_t *e);
#endif
static void gesture_event_cb(lv_event_t *e);

/*
 *  STATIC VARIABLES
 *****************************************************************************************
 */
static lv_obj_t *compass_obj, *compass_txt, *prev_active;

/*
 *      MACROS
 *****************************************************************************************
 */
LV_IMG_DECLARE(compass);
LV_IMG_DECLARE(compass_earth);
LV_IMG_DECLARE(compass_index);

/*
 *   GLOBAL FUNCTIONS
 *****************************************************************************************
 */
void show_compass_cb(lv_event_t *e)
{
        lv_event_code_t code = lv_event_get_code(e);

        if (code == LV_EVENT_CLICKED) {
                show_compass_screen();
        }
}

void compass_rotate(int16_t angle)
{
        uint8_t text[17];

        if (!compass_obj || !compass_txt) {
                return;
        }

        /* Set the rotation center of the image. */
        lv_img_set_pivot(compass_obj, compass.header.w / 2, compass.header.h / 2);

        /* Angle has 0.1 degree precision, so for 45.8 set 458. */
        lv_img_set_angle(compass_obj, (360 - angle) * 10);
        if ((angle < 45) || (angle == 360)) {
                sprintf((char*)text, "%s %d", "#FF4500 N ", (angle == 360) ? 0 : angle);
        } else if ((angle >= 45) && (angle < 90)) {
                sprintf((char*)text, "%s %d", "#FF4500 NE ", angle);
        } else if ((angle >= 90) && (angle < 135)) {
                sprintf((char*)text, "%s %d", "#FF4500 E ", angle - 90);
        } else if ((angle >= 135) && (angle < 180)) {
                sprintf((char*)text, "%s %d", "#FF4500 SE ", angle - 90);
        } else if ((angle >= 180) && (angle < 225)) {
                sprintf((char*)text, "%s %d", "#FF4500 S ", angle - 180);
        } else if ((angle >= 225) && (angle < 270)) {
                sprintf((char*)text, "%s %d", "#FF4500 SW ", angle - 180);
        } else if ((angle >= 270) && (angle < 315)) {
                sprintf((char*)text, "%s %d", "#FF4500 W ", angle - 270);
        } else if ((angle >= 315) && (angle < 360)) {
                sprintf((char*)text, "%s %d", "#FF4500 NW ", angle - 270);
        }
        lv_label_set_text(compass_txt, (char*)text);
}

/*
 *   STATIC FUNCTIONS
 *****************************************************************************************
 */
static void show_compass_screen()
{
        lv_obj_t *compass_screen_obj, *compass_earth_obj, *compass_index_obj;

        /* Save previously active screen */
        prev_active = lv_scr_act();

        compass_screen_obj = lv_obj_create(NULL);
        lv_obj_set_style_bg_color(compass_screen_obj, lv_color_black(), LV_PART_MAIN);
        lv_obj_add_event_cb(compass_screen_obj, gesture_event_cb, LV_EVENT_GESTURE,
                compass_screen_obj);
        lv_obj_clear_flag(compass_screen_obj, LV_OBJ_FLAG_GESTURE_BUBBLE);

        /* Create the required objects for the compass */
#if COMPASS_ROTATION_USES_CANVAS
        lv_color_t *cbuf = lv_mem_alloc(lv_img_buf_get_img_size(compass.header.w, compass.header.h, compass.header.cf));
        compass_obj = lv_canvas_create(compass_screen_obj);

        lv_canvas_set_buffer(compass_obj, cbuf, compass.header.w, compass.header.h, compass.header.cf);
        lv_obj_add_event_cb(compass_obj, compass_delete_event_cb, LV_EVENT_DELETE, compass_obj);

        lv_draw_img_dsc_t img_dsc;
        lv_draw_img_dsc_init(&img_dsc);
        lv_canvas_draw_img(compass_obj, 0, 0, &compass, &img_dsc);
#else
        compass_obj = lv_img_create(compass_screen_obj);
        lv_img_set_src(compass_obj, &compass);
#endif
        lv_obj_align(compass_obj, LV_ALIGN_CENTER, 0, 0);

        compass_earth_obj = lv_img_create(compass_screen_obj);
        lv_img_set_src(compass_earth_obj, &compass_earth);
        lv_obj_align(compass_earth_obj, LV_ALIGN_CENTER, 0, 0);
        compass_txt = lv_label_create(compass_screen_obj);
        lv_label_set_recolor(compass_txt, true);
        lv_obj_set_style_text_font(compass_txt, &lv_font_montserrat_32, 0);
        lv_label_set_text(compass_txt, "#FF4500 N 0");
        lv_obj_align(compass_txt, LV_ALIGN_CENTER, 0, 0);

        compass_index_obj = lv_img_create(compass_screen_obj);
        lv_img_set_src(compass_index_obj, &compass_index);
        lv_obj_set_pos(compass_index_obj, (DEMO_RESX - compass_index.header.w) / 2, 0);

        lv_scr_load(compass_screen_obj);

        start_compass_data();
}

#if COMPASS_ROTATION_USES_CANVAS
static void compass_delete_event_cb(lv_event_t *e)
{
        lv_obj_t *compass_obj = lv_event_get_user_data(e);
        lv_img_dsc_t *dsc = lv_canvas_get_img(compass_obj);
        lv_mem_free((void *)dsc->data);
}

#endif

static void gesture_event_cb(lv_event_t *e)
{
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        lv_obj_t *screen_obj = lv_event_get_user_data(e);

        if (dir == LV_DIR_RIGHT) {
                stop_compass_data();
                lv_scr_load(prev_active);
                lv_obj_del(screen_obj);
        }
}
