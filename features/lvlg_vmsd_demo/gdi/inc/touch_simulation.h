/**
 ****************************************************************************************
 *
 * @file touch_simulation.h
 *
 * @brief Touch configuration for GDI
 *
 * Copyright (C) 2021-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#ifndef TOUCH_SIMULATION_H_
#define TOUCH_SIMULATION_H_

#include <stdio.h>
#include <stdint.h>

#if dg_configUSE_TOUCH_SIMULATION

#define GDI_TOUCH_ENABLE                (1) /* i.e. no device to init by GDI */
#define GDI_TOUCH_READ_EVENT            (touch_simulation_read_event)

typedef enum {
        SLIDE_LEFT_TO_RIGHT,
        SLIDE_RIGHT_TO_LEFT,
        SLIDE_MENU_UP_TO_DOWN,
        SLIDE_MENU_DOWN_TO_UP,
        TOUCH_TIMER_MENU,
        TOUCH_TIMER_START,
        TOUCH_ACTIVITY_MENU,
        TOUCH_COMPASS_MENU,
        SLIDE_BACK,
} GESTURE_TYPE;

void touch_simulation_set_data(GESTURE_TYPE type);

/* User-defined routine used by GDI to read touch events */
void touch_simulation_read_event(void *dev, gdi_touch_data_t *data);
void touch_simulation(GESTURE_TYPE type);

#endif /* dg_configUSE_TOUCH_SIMULATION */

#endif /* TOUCH_SIMULATION_H_ */
