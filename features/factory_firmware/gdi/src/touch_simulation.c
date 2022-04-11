/**
 ****************************************************************************************
 *
 * @file touch_simulation.s
 *
 * @brief Touch Simulation for GDI
 *
 * Copyright (C) 2021-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#include <string.h>
#include "osal.h"
#include "gdi.h"
#include "touch_simulation.h"
#include "lvgl.h"

#if dg_configUSE_TOUCH_SIMULATION

extern void compass_event();

/*********************************************************************
 *
 *       Defines
 *
 **********************************************************************
 */
#ifdef LV_INDEV_DEF_READ_PERIOD
#define TOUCH_DATA_DELAY_MS             (LV_INDEV_DEF_READ_PERIOD)
#else
#define TOUCH_DATA_DELAY_MS             (10)
#endif

/*********************************************************************
 *
 *       Types
 *
 **********************************************************************
 */
typedef struct {
        GESTURE_TYPE type;
        uint16_t index;
        uint16_t count;
        const void* data;
} GESTURE;

static const gdi_touch_data_t slide_left_to_right[] = {
        { 385, 200, 1 },
        { 381, 200, 1 },
        { 359, 200, 1 },
        { 318, 200, 1 },
        { 278, 200, 1 },
        { 233, 200, 1 },
        { 191, 200, 1 },
        { 151, 200, 1 },
        { 110, 200, 1 },
        { 70 ,200, 1 },
        { 28 ,200, 1 },
        { 5 ,200, 1 },
        { 5 ,200, 0 },
};

static const gdi_touch_data_t slide_right_to_left[] = {
        { 12, 200, 1 },
        { 65, 200, 1 },
        { 120, 200, 1 },
        { 170, 200, 1 },
        { 227, 200, 1 },
        { 283, 200, 1 },
        { 324, 200, 1 },
        { 346, 200, 1 },
        { 350, 200, 1 },
        { 350, 200, 0 },
};

static const gdi_touch_data_t slide_menu_down[] = {
        { 200, 380, 1 },
        { 200, 340, 1 },
        { 200, 300, 1 },
        { 200, 250, 1 },
        { 200, 200, 1 },
        { 200, 180, 1 },
        { 200, 150, 1 },
        { 200, 100, 1 },
        { 200, 50, 1 },
        { 200, 50, 0 },
};

static const gdi_touch_data_t slide_menu_up[] = {
        { 200, 30, 1 },
        { 200, 60, 1 },
        { 200, 90, 1 },
        { 200, 120, 1 },
        { 200, 150, 1 },
        { 200, 180, 1 },
        { 200, 190, 1 },
        { 200, 200, 1 },
        { 200, 250, 1 },
        { 200, 300, 1 },
        { 200, 320, 1 },
        { 200, 340, 1 },
        { 200, 380, 0 },
        { 200, 30, 1 },
        { 200, 60, 1 },
        { 200, 90, 1 },
        { 200, 120, 1 },
        { 200, 150, 1 },
        { 200, 180, 1 },
        { 200, 180, 0 },
};

static const gdi_touch_data_t touch_timer_menu[] = {
        { 120, 88, 1 },
        { 120, 88, 0 }
};

static const gdi_touch_data_t touch_timer_start[] = {
        { 200, 141, 1 },
        { 200, 141, 0 }
};

static const gdi_touch_data_t touch_activity_menu[] = {
        { 107, 209, 1 },
        { 107, 209, 0 },
};

static const gdi_touch_data_t touch_compass_menu[] = {
        { 124, 333, 1 },
        { 124, 333, 0 },
};

static const gdi_touch_data_t slide_back[] = {
        { 113, 216, 1 },
        { 281, 216, 1 },
        { 281, 216, 0 },
};

static GESTURE gesture;

void touch_simulation_set_data(GESTURE_TYPE type)
{
        gesture.type = type;
        gesture.index = 0;
        if (type == SLIDE_LEFT_TO_RIGHT) {
                gesture.count = ARRAY_LENGTH(slide_left_to_right);
                gesture.data = slide_left_to_right;
        } else if (type == SLIDE_RIGHT_TO_LEFT) {
                gesture.count = ARRAY_LENGTH(slide_right_to_left);
                gesture.data = slide_right_to_left;
        } else if (type == SLIDE_MENU_UP_TO_DOWN) {
                gesture.count = ARRAY_LENGTH(slide_menu_down);
                gesture.data = slide_menu_down;
        } else if (type == SLIDE_MENU_DOWN_TO_UP) {
                gesture.count = ARRAY_LENGTH(slide_menu_up);
                gesture.data = slide_menu_up;
        } else if (type == TOUCH_TIMER_MENU) {
                gesture.count = ARRAY_LENGTH(touch_timer_menu);
                gesture.data = touch_timer_menu;
        } else if (type == TOUCH_TIMER_START) {
                gesture.count = ARRAY_LENGTH(touch_timer_start);
                gesture.data = touch_timer_start;
        } else if (type == TOUCH_ACTIVITY_MENU) {
                gesture.count = ARRAY_LENGTH(touch_activity_menu);
                gesture.data = touch_activity_menu;
        } else if (type == TOUCH_COMPASS_MENU) {
                gesture.count = ARRAY_LENGTH(touch_compass_menu);
                gesture.data = touch_compass_menu;
        } else if (type == SLIDE_BACK) {
                gesture.count = ARRAY_LENGTH(slide_back);
                gesture.data = slide_back;
        }
}

/* User-defined routine used by GDI to read touch events */
void touch_simulation_read_event(void *dev, gdi_touch_data_t *data)
{
        if (gesture.index < gesture.count) {
                memcpy(data, &((gdi_touch_data_t *)gesture.data)[gesture.index++], sizeof(gdi_touch_data_t));
        }
}

void touch_simulation(GESTURE_TYPE type)
{
        touch_simulation_set_data(type);

        for (uint16_t i = 0; i < gesture.count; i++) {
                gdi_touch_event();
                OS_DELAY(OS_MS_2_TICKS(TOUCH_DATA_DELAY_MS));
        }
}

#endif /* dg_configUSE_TOUCH_SIMULATION */
