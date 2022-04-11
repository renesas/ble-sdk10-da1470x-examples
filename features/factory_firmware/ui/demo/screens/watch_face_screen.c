/**
 ****************************************************************************************
 *
 * @file watch_face_screen.c
 *
 * @brief Watch face main screen source file
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
#include "Resources.h"
#include "module.h"

/*
 *      DEFINES
 *****************************************************************************************
 */
#define DEFAULT_TIME_HOUR       9
#define DEFAULT_TIME_MINUTES    15
#define DEFAULT_TIME_SECONDS    0

#define ANGLE_PRECISION_FACTOR  10
#define TIMER_DIVISOR           10

/*
 *      TYPEDEFS
 *****************************************************************************************
 */
/*
 * @brief  RTC Time structure definition
 */
typedef struct
{
        uint8_t Hours; /*!< Specifies the RTC Time Hour.
         This parameter must be a number between Min_Data = 0 and Max_Data = 12 if the RTC_HourFormat_12 is selected.
         This parameter must be a number between Min_Data = 0 and Max_Data = 23 if the RTC_HourFormat_24 is selected */

        uint8_t Minutes; /*!< Specifies the RTC Time Minutes.
         This parameter must be a number between Min_Data = 0 and Max_Data = 59 */

        uint8_t Seconds; /*!< Specifies the RTC Time Seconds.
         This parameter must be a number between Min_Data = 0 and Max_Data = 59 */

} RTC_TimeTypeDef;

void watch_face_screen(lv_obj_t *parent_obj, lv_style_t *style_screen, lv_coord_t x, lv_coord_t y);
/* Startup module: watch face */
ModuleItem_Typedef watch_face =
{
        0,
        "watch face",
        0,
        watch_face_screen,
        NULL,
};

/*
 *  STATIC PROTOTYPES
 *****************************************************************************************
 */
static void lv_UpdateTime();
static void lv_SetTime(RTC_TimeTypeDef *time);
static void lv_GetTime(RTC_TimeTypeDef *time);
static void lv_UpdateClock();
static void lv_DrawClock();
static void rotate_image(void *img_obj, int32_t angle, const lv_img_dsc_t *img_dsc);

/*
 *  STATIC VARIABLES
 *****************************************************************************************
 */
static lv_obj_t *tick_hour_obj, *tick_minute_obj, *tick_second_obj;
static int slice_count;
static lv_timer_t *update_time_timer;
/*
 * RTC defines
 */
static RTC_TimeTypeDef current_time = { DEFAULT_TIME_HOUR, DEFAULT_TIME_MINUTES, DEFAULT_TIME_SECONDS };

/*
 *      MACROS
 *****************************************************************************************
 */
LV_IMG_DECLARE(clock_bg);
LV_IMG_DECLARE(stamens);
LV_IMG_DECLARE(tick_hour);
LV_IMG_DECLARE(tick_minute);
LV_IMG_DECLARE(tick_second);

/*
 *   GLOBAL FUNCTIONS
 *****************************************************************************************
 */
static void event_cb(lv_event_t *e)
{
        lv_event_code_t code = lv_event_get_code(e);
        lv_obj_t *obj = lv_event_get_target(e);
        lv_area_t coords;
        lv_obj_get_coords(obj, &coords);

        if ((code == LV_EVENT_SCROLL_BEGIN) && (coords.x2 == DEMO_RESX - 1)) {
                lv_timer_pause(update_time_timer);
        }

        if ((code == LV_EVENT_SCROLL_END) && (coords.x2 == DEMO_RESX - 1)) {
                lv_timer_resume(update_time_timer);
        }
}

void watch_face_screen(lv_obj_t *parent_obj, lv_style_t *style_screen, lv_coord_t x, lv_coord_t y)
{
        lv_obj_t *watch_face_screen_obj, *clock_bg_obj, *stamens_obj;

        watch_face_screen_obj = lv_obj_create(parent_obj);
        lv_obj_remove_style_all(watch_face_screen_obj);
        lv_obj_set_size(watch_face_screen_obj, DEMO_RESX, DEMO_RESY);
        lv_obj_set_pos(watch_face_screen_obj, x, y);
        lv_obj_set_scrollbar_mode(watch_face_screen_obj, LV_SCROLLBAR_MODE_OFF);
        lv_obj_add_style(watch_face_screen_obj, style_screen, LV_PART_MAIN);
        lv_obj_add_event_cb(watch_face_screen_obj, event_cb, LV_EVENT_ALL, NULL);

        /*Create the required objects for the watch*/
        clock_bg_obj = lv_img_create(watch_face_screen_obj);
        lv_img_set_src(clock_bg_obj, &clock_bg);
        lv_obj_align(clock_bg_obj, LV_ALIGN_CENTER, 0, 0);

        tick_hour_obj = lv_img_create(watch_face_screen_obj);
        lv_img_set_src(tick_hour_obj, &tick_hour);

        tick_minute_obj = lv_img_create(watch_face_screen_obj);
        lv_img_set_src(tick_minute_obj, &tick_minute);

        tick_second_obj = lv_img_create(watch_face_screen_obj);
        lv_img_set_src(tick_second_obj, &tick_second);

        stamens_obj = lv_img_create(watch_face_screen_obj);
        lv_img_set_src(stamens_obj, &stamens);
        lv_obj_align(stamens_obj, LV_ALIGN_CENTER, 0, 0);

        lv_DrawClock();

        /*Create a timer to update the displayed time*/
        update_time_timer = lv_timer_create(lv_UpdateTime, (uint32_t)(1000 / TIMER_DIVISOR), NULL);
        lv_timer_set_repeat_count(update_time_timer, -1);
}

/*
 *   STATIC FUNCTIONS
 *****************************************************************************************
 */

/**
 * @brief  Update current_time.
 *
 * Should be called every second.
 *
 * @retval None
 */
static void lv_UpdateTime()
{
        slice_count++;
        if (slice_count == TIMER_DIVISOR) {
                current_time.Seconds++;
                slice_count = 0;
        }

        if (current_time.Seconds == 60) {
                current_time.Minutes++;
                current_time.Seconds = 0;
        }
        if (current_time.Minutes == 60) {
                current_time.Hours++;
                current_time.Minutes = 0;
        }
        if (current_time.Hours == 24) {
                current_time.Hours = 0;
        }

        lv_UpdateClock();
}

/**
 * @brief  Set time.
 * @param  Time: Pointer to Time structure
 * @retval None
 */
static void lv_SetTime(RTC_TimeTypeDef *time)
{
        current_time.Hours = time->Hours;
        current_time.Minutes = time->Minutes;
        current_time.Seconds = time->Seconds;
}

/**
 * @brief  Get time.
 * @param  Time: Pointer to Time structure
 * @retval None
 */
static void lv_GetTime(RTC_TimeTypeDef *time)
{
        time->Hours = current_time.Hours;
        time->Minutes = current_time.Minutes;
        time->Seconds = current_time.Seconds;
}

/**
 * @brief  Update clock
 * @retval None
 */
static void lv_UpdateClock()
{
        RTC_TimeTypeDef Time;

        lv_GetTime(&Time);
        if (Time.Hours > 12) {
                Time.Hours -= 12;
                lv_SetTime(&Time);
        }

        if (Time.Hours == 0) {
                Time.Hours = 12;
                lv_SetTime(&Time);
        }

        lv_DrawClock();
}

/**
 * @brief  Draw clock
 * @param  None
 * @retval None
 */
static void lv_DrawClock()
{
        static int angle_hour, angle_min, angle_sec;
        static int angle_hour_prev = -1, angle_min_prev = -1, angle_sec_prev = -1;

        angle_hour = (current_time.Hours + (float)current_time.Minutes / 60.0) * 30.0 * ANGLE_PRECISION_FACTOR;
        angle_min = (current_time.Minutes + (float)current_time.Seconds / 60.0) * 6.0 * ANGLE_PRECISION_FACTOR;
        angle_sec = (current_time.Seconds * 6.0 * ANGLE_PRECISION_FACTOR) + (slice_count * 6.0 * ANGLE_PRECISION_FACTOR / TIMER_DIVISOR);

        /* Draw the hour needle */
        if (angle_hour != angle_hour_prev) {
                rotate_image(tick_hour_obj, angle_hour, &tick_hour);
        }

        /* Draw the minutes needle */
        if (angle_min != angle_min_prev) {
                rotate_image(tick_minute_obj, angle_min, &tick_minute);
        }

        /* Draw the seconds needle */
        if (angle_sec != angle_sec_prev) {
                rotate_image(tick_second_obj, angle_sec, &tick_second);
        }

        angle_hour_prev = angle_hour;
        angle_min_prev = angle_min;
        angle_sec_prev = angle_sec;
}

static void rotate_image(void *img_obj, int32_t angle, const lv_img_dsc_t *img_dsc)
{
        lv_obj_set_pos(img_obj, (clock_bg.header.w - img_dsc->header.w) / 2, (clock_bg.header.h / 2) - img_dsc->header.h);
        /* Set the rotation center of the image. */
        lv_img_set_pivot(img_obj, img_dsc->header.w / 2, img_dsc->header.h);
        /* Angle has 0.1 degree precision, so for 45.8 set 458. */
        lv_img_set_angle(img_obj, angle);
}
