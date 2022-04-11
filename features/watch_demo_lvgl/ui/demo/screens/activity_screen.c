/**
 ****************************************************************************************
 *
 * @file activity_screen.c
 *
 * @brief Activity screen source file
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

/*
 *      DEFINES
 *****************************************************************************************
 */
#define ITEMS_PER_SCREEN        3

/*
 *  STATIC PROTOTYPES
 *****************************************************************************************
 */
static void show_activity_screen();
static void gesture_event_cb(lv_event_t *e);

/*
 *  STATIC VARIABLES
 *****************************************************************************************
 */
static lv_obj_t *prev_active;

/*
 *      MACROS
 *****************************************************************************************
 */
LV_IMG_DECLARE(track);

/*
 *   GLOBAL FUNCTIONS
 *****************************************************************************************
 */
void show_activity_cb(lv_event_t *e)
{
        lv_event_code_t code = lv_event_get_code(e);

        if (code == LV_EVENT_CLICKED) {
                show_activity_screen();
        }
}

/*
 *   STATIC FUNCTIONS
 *****************************************************************************************
 */
static void show_activity_screen()
{
        lv_obj_t *activity_screen_obj, *track_obj;
        lv_obj_t *label_title, *activity_label, *duration_label, *distance_label, *pace_label,
                *hrate_label, *cadence_label, *stride_label, *consumption_label;

        /* save previously active screen */
        prev_active = lv_scr_act();

        activity_screen_obj = lv_obj_create(NULL);
        lv_obj_set_style_bg_color(activity_screen_obj, lv_color_black(), LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(activity_screen_obj, LV_SCROLLBAR_MODE_OFF);
        lv_obj_add_event_cb(activity_screen_obj, gesture_event_cb, LV_EVENT_GESTURE,
                activity_screen_obj);
        lv_obj_clear_flag(activity_screen_obj, LV_OBJ_FLAG_GESTURE_BUBBLE);

        label_title = lv_label_create(activity_screen_obj);
        lv_label_set_recolor(label_title, true);
        lv_obj_set_style_text_font(label_title, &lv_font_montserrat_32, 0);
        lv_label_set_text(label_title, "#ffffff Outdoor\n#ffffff Activity");
        lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 10);

        activity_label = lv_label_create(activity_screen_obj);
        lv_label_set_recolor(activity_label, true);
        lv_obj_set_style_text_font(activity_label, &lv_font_montserrat_16, 0);
        lv_label_set_text(activity_label,
                "#ffffff 08/26                                         15:35");
        lv_obj_set_pos(activity_label, 71, 119);

        track_obj = lv_img_create(activity_screen_obj);
        lv_img_set_src(track_obj, &track);
        lv_obj_set_pos(track_obj, 72, 143);

        duration_label = lv_label_create(activity_screen_obj);
        lv_label_set_recolor(duration_label, true);
        lv_obj_set_style_text_font(duration_label, &lv_font_montserrat_20, 0);
        lv_label_set_text(duration_label, "#ffffff Duration\n#ffffff  00:20:19");
        lv_obj_set_pos(duration_label, 150, 530);

        distance_label = lv_label_create(activity_screen_obj);
        lv_label_set_recolor(distance_label, true);
        lv_obj_set_style_text_font(distance_label, &lv_font_montserrat_20, 0);
        lv_label_set_text(distance_label, "#ffffff Distance\n#ffffff   2.83 km");
        lv_obj_set_pos(distance_label, 150, 630);

        pace_label = lv_label_create(activity_screen_obj);
        lv_label_set_recolor(pace_label, true);
        lv_obj_set_style_text_font(pace_label, &lv_font_montserrat_20, 0);
        lv_label_set_text(pace_label, "#ffffff Pace\n#ffffff 7'10''");
        lv_obj_set_pos(pace_label, 170, 730);

        hrate_label = lv_label_create(activity_screen_obj);
        lv_label_set_recolor(hrate_label, true);
        lv_obj_set_style_text_font(hrate_label, &lv_font_montserrat_20, 0);
        lv_label_set_text(hrate_label, "#ffffff Heart Rate\n#ffffff   163 bpm");
        lv_obj_set_pos(hrate_label, 140, 830);

        cadence_label = lv_label_create(activity_screen_obj);
        lv_label_set_recolor(cadence_label, true);
        lv_obj_set_style_text_font(cadence_label, &lv_font_montserrat_20, 0);
        lv_label_set_text(cadence_label, "#ffffff Cadence\n#ffffff        155");
        lv_obj_set_pos(cadence_label, 155, 930);

        stride_label = lv_label_create(activity_screen_obj);
        lv_label_set_recolor(stride_label, true);
        lv_obj_set_style_text_font(stride_label, &lv_font_montserrat_20, 0);
        lv_label_set_text(stride_label, "#ffffff Stride Length\n#ffffff         70 cm");
        lv_obj_set_pos(stride_label, 125, 1030);

        consumption_label = lv_label_create(activity_screen_obj);
        lv_label_set_recolor(consumption_label, true);
        lv_obj_set_style_text_font(consumption_label, &lv_font_montserrat_20, 0);
        lv_label_set_text(consumption_label, "#ffffff Consumption\n#ffffff       260 kcal");
        lv_obj_set_pos(consumption_label, 135, 1130);

        lv_scr_load(activity_screen_obj);
}

static void gesture_event_cb(lv_event_t *e)
{
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        lv_obj_t *screen_obj = lv_event_get_user_data(e);

        if (dir == LV_DIR_RIGHT) {
                lv_scr_load(prev_active);
                lv_obj_del(screen_obj);
        }
}
