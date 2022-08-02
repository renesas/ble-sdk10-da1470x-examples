/**
 ****************************************************************************************
 *
 * @file timer_screen.c
 *
 * @brief Timer screen source file
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
 *  STATIC PROTOTYPES
 *****************************************************************************************
 */
static void show_timer_screen();
static void gesture_event_cb(lv_event_t *e);
static void update_timer_cb();
static void update_timer(uint32_t elapsedTime);
static void click_timer_cb(lv_event_t *e);
static void reset_timer_cb(lv_event_t *e);

/*
 *  STATIC VARIABLES
 *****************************************************************************************
 */
static lv_obj_t *stopwatch_label, *prev_active;
static lv_timer_t *counter_timer;
static uint32_t start_time, total_elapsed_time;

/*
 *      MACROS
 *****************************************************************************************
 */
LV_IMG_DECLARE(timer);
LV_IMG_DECLARE(reset_timer);

/*
 *   GLOBAL FUNCTIONS
 *****************************************************************************************
 */
void show_timer_cb(lv_event_t *e)
{
        lv_event_code_t code = lv_event_get_code(e);

        if (code == LV_EVENT_CLICKED) {
                show_timer_screen();
        }
}

/*
 *   STATIC FUNCTIONS
 *****************************************************************************************
 */
static void show_timer_screen()
{
        lv_obj_t *timer_area_obj, *timer_screen_obj, *show_timer_obj, *timer_label, *reset_area_obj, *reset_obj;
        lv_coord_t clickable_w = 100;
        lv_coord_t clickable_h = 100;

        /* save previously active screen */
        prev_active = lv_scr_act();

        timer_screen_obj = lv_obj_create(NULL);
        lv_obj_set_style_bg_color(timer_screen_obj, lv_color_black(), LV_PART_MAIN);
        lv_obj_add_event_cb(timer_screen_obj, gesture_event_cb, LV_EVENT_GESTURE, timer_screen_obj);
        lv_obj_clear_flag(timer_screen_obj, LV_OBJ_FLAG_GESTURE_BUBBLE);

        timer_label = lv_label_create(timer_screen_obj);
        lv_label_set_recolor(timer_label, true);
        lv_obj_set_style_text_font(timer_label, &lv_font_montserrat_32, 0);
        lv_label_set_text(timer_label, "#ffffff Timer");
        lv_obj_set_pos(timer_label, 148, 38);

        counter_timer = lv_timer_create(update_timer_cb, LV_DISP_DEF_REFR_PERIOD, NULL);
        lv_timer_pause(counter_timer);

        timer_area_obj = lv_obj_create(timer_screen_obj);
        lv_obj_remove_style_all(timer_area_obj);
        lv_obj_set_size(timer_area_obj, clickable_w, clickable_h);
        lv_obj_set_pos(timer_area_obj, (DEMO_RESX - clickable_w) / 2, 100);
        lv_obj_add_event_cb(timer_area_obj, click_timer_cb, LV_EVENT_CLICKED, NULL);
        lv_obj_add_flag(timer_area_obj, LV_OBJ_FLAG_CLICKABLE);

        show_timer_obj = lv_img_create(timer_area_obj);
        lv_img_set_src(show_timer_obj, &timer);
        lv_obj_align(show_timer_obj, LV_ALIGN_CENTER, 0, 0);

        stopwatch_label = lv_label_create(timer_screen_obj);
        lv_label_set_recolor(stopwatch_label, true);
        lv_obj_set_style_text_font(stopwatch_label, &lv_font_montserrat_32, 0);
        update_timer(total_elapsed_time);

        reset_area_obj = lv_obj_create(timer_screen_obj);
        lv_obj_remove_style_all(reset_area_obj);
        lv_obj_set_size(reset_area_obj, clickable_w, clickable_w);
        lv_obj_set_pos(reset_area_obj, (DEMO_RESX - clickable_w) / 2, 276);
        lv_obj_add_event_cb(reset_area_obj, reset_timer_cb, LV_EVENT_CLICKED, timer_area_obj);
        lv_obj_add_flag(reset_area_obj, LV_OBJ_FLAG_CLICKABLE);

        reset_obj = lv_img_create(reset_area_obj);
        lv_img_set_src(reset_obj, &reset_timer);
        lv_obj_align(reset_obj, LV_ALIGN_CENTER, 0, 0);

        lv_scr_load(timer_screen_obj);
}

static void gesture_event_cb(lv_event_t *e)
{
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        lv_obj_t *screen_obj = lv_event_get_user_data(e);

        if (dir == LV_DIR_RIGHT) {
                if (counter_timer && !counter_timer->paused) {
                        lv_timer_pause(counter_timer);
                }
                lv_scr_load(prev_active);
                lv_obj_del(screen_obj);
                lv_timer_del(counter_timer);
        }
}

static void update_timer_cb()
{
        update_timer(lv_tick_elaps(start_time) + total_elapsed_time);
}

static void update_timer(uint32_t elapsedTime)
{
        int msec = elapsedTime % 1000;
        int intSec = elapsedTime / 1000;
        int sec = intSec % 60;
        int min = intSec / 60;

        if (min == 100) {
                elapsedTime = min = sec = 0;
        }

        lv_label_set_text_fmt(stopwatch_label, "#ffffff %02d:%02d.%01d", min, sec, msec / 100);
        lv_obj_set_pos(stopwatch_label, 135, 214);
}

static void click_timer_cb(lv_event_t *e)
{
        lv_obj_t *obj = lv_event_get_target(e);
        lv_event_code_t code = lv_event_get_code(e);

        if (code == LV_EVENT_CLICKED) {
                if (lv_obj_has_state(obj, LV_STATE_CHECKED)) {
                        lv_obj_clear_state(obj, LV_STATE_CHECKED);
                        total_elapsed_time = total_elapsed_time + lv_tick_get() - start_time;
                        lv_timer_pause(counter_timer);
                }
                else {
                        lv_obj_add_state(obj, LV_STATE_CHECKED);
                        start_time = lv_tick_get();
                        lv_timer_resume(counter_timer);
                }
        }
}

static void reset_timer_cb(lv_event_t *e)
{
        lv_event_code_t code = lv_event_get_code(e);
        lv_obj_t *timer_area_obj = (lv_obj_t *)lv_event_get_user_data(e);

        if (code == LV_EVENT_CLICKED) {
                if (!counter_timer->paused) {
                        lv_obj_clear_state(timer_area_obj, LV_STATE_CHECKED);
                        lv_timer_pause(counter_timer);
                }
                total_elapsed_time = 0;
                update_timer(total_elapsed_time);
        }
}
