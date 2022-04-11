/**
 ****************************************************************************************
 *
 * @file lv_port_indev.c
 *
 * @brief Input device driver
 *
 * Copyright (C) 2021-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_indev.h"
#include "gdi.h"

/*********************
 *      DEFINES
 *********************/
#ifndef LV_PORT_INDEV_TOUCHPAD_EN
#define LV_PORT_INDEV_TOUCHPAD_EN               (GDI_TOUCH_ENABLE)
#endif

#ifndef LV_PORT_INDEV_BUTTON_EN
#define LV_PORT_INDEV_BUTTON_EN                 (0)
#endif

#ifndef LV_PORT_INDEV_TOUCH_QUEUE_EN
#define LV_PORT_INDEV_TOUCH_QUEUE_EN            (1)
#endif

#ifndef LV_PORT_INDEV_TOUCH_QUEUE_DPTH
#define LV_PORT_INDEV_TOUCH_QUEUE_DPTH          (5)
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if LV_PORT_INDEV_TOUCHPAD_EN
static void touchpad_init(void);
static void touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
#endif /* LV_PORT_INDEV_TOUCHPAD_EN */

#if LV_PORT_INDEV_BUTTON_EN
static void button_init(void);
static void button_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
static int8_t button_get_pressed_id(void);
static bool button_is_pressed(uint8_t id);
#endif /* LV_PORT_INDEV_BUTTON_EN */

/**********************
 *  STATIC VARIABLES
 **********************/
#if LV_PORT_INDEV_TOUCHPAD_EN
static lv_indev_drv_t indev_touchpad_drv;
static lv_indev_t *indev_touchpad;
#if LV_PORT_INDEV_TOUCH_QUEUE_EN
static OS_QUEUE touch_events;
#else  /* LV_PORT_INDEV_TOUCH_QUEUE_EN */
static gdi_touch_data_t touch_state = { 0 };
#endif /* LV_PORT_INDEV_TOUCH_QUEUE_EN */
#endif
#if LV_PORT_INDEV_BUTTON_EN
static lv_indev_t *indev_button;
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_indev_init(void)
{
#if LV_PORT_INDEV_TOUCHPAD_EN
        /*------------------
         * Touchpad
         * -----------------*/

        /*Initialize your touchpad if you have*/
        touchpad_init();

        /*Register a touchpad input device*/
        lv_indev_drv_init(&indev_touchpad_drv);
        indev_touchpad_drv.type = LV_INDEV_TYPE_POINTER;
        indev_touchpad_drv.read_cb = touchpad_read;
        indev_touchpad = lv_indev_drv_register(&indev_touchpad_drv);
#endif /* LV_PORT_INDEV_TOUCHPAD_EN */
#if LV_PORT_INDEV_BUTTON_EN
        /*------------------
         * Button
         * -----------------*/
        static lv_indev_drv_t indev_button_drv;

        /*Initialize your button if you have*/
        button_init();

        /*Register a button input device*/
        lv_indev_drv_init(&indev_button_drv);
        indev_button_drv.type = LV_INDEV_TYPE_BUTTON;
        indev_button_drv.read_cb = button_read;
        indev_button = lv_indev_drv_register(&indev_button_drv);

        /*Assign buttons to points on the screen*/
        static const lv_point_t btn_points[2] = {
                { 10, 10 }, /*Button 0 -> x:10; y:10*/
                { 40, 100 }, /*Button 1 -> x:40; y:100*/
        };
        lv_indev_set_button_points(indev_button, btn_points);
#endif /* LV_PORT_INDEV_BUTTON_EN */
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
#if LV_PORT_INDEV_TOUCHPAD_EN
/*------------------
 * Touchpad
 * -----------------*/
static void touch_store_events(gdi_touch_data_t *touch_data)
{
#if LV_PORT_INDEV_TOUCH_QUEUE_EN
        if (OS_QUEUE_FULL == OS_QUEUE_PUT(touch_events, touch_data, OS_QUEUE_NO_WAIT)) {
                /* Touch point is dropped, check if increased queue is required */
                OS_ASSERT(0);
        }
#else /* LV_PORT_INDEV_TOUCH_QUEUE_EN */
        OS_ENTER_CRITICAL_SECTION();
        touch_state = *touch_data;
        OS_LEAVE_CRITICAL_SECTION();
#endif /* LV_PORT_INDEV_TOUCH_QUEUE_EN */
}

/*Initialize your touchpad*/
static void touchpad_init(void)
{
        gdi_set_callback_store_touch(touch_store_events);
#if LV_PORT_INDEV_TOUCH_QUEUE_EN
        OS_QUEUE_CREATE(touch_events, sizeof(gdi_touch_data_t), LV_PORT_INDEV_TOUCH_QUEUE_DPTH);
#endif  /* LV_PORT_INDEV_TOUCH_QUEUE_EN */
}

/*Will be called by the library to read the touchpad*/
static void touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
#if LV_PORT_INDEV_TOUCH_QUEUE_EN
        gdi_touch_data_t event;
        if (OS_QUEUE_OK == OS_QUEUE_GET(touch_events, &event, OS_QUEUE_NO_WAIT)) {
                data->point.x = event.x;
                data->point.y = event.y;
                data->state = event.pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
                data->continue_reading = OS_QUEUE_MESSAGES_WAITING(touch_events) ? true : false;
        }
#else /* LV_PORT_INDEV_TOUCH_QUEUE_EN */
        OS_ENTER_CRITICAL_SECTION();
        /*Set the last pressed coordinates*/
        data->point.x = touch_state.x;
        data->point.y = touch_state.y;
        data->state = touch_state.pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
        OS_LEAVE_CRITICAL_SECTION();
#endif /* LV_PORT_INDEV_TOUCH_QUEUE_EN */
}

#endif /* LV_PORT_INDEV_TOUCHPAD_EN */
#if LV_PORT_INDEV_BUTTON_EN
/*------------------
 * Button
 * -----------------*/

/*Initialize your buttons*/
static void button_init(void)
{
        /*Your code comes here*/
}

/*Will be called by the library to read the button*/
static void button_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{

        static uint8_t last_btn = 0;

        /*Get the pressed button's ID*/
        int8_t btn_act = button_get_pressed_id();

        if (btn_act >= 0) {
                data->state = LV_INDEV_STATE_PR;
                last_btn = btn_act;
        } else {
                data->state = LV_INDEV_STATE_REL;
        }

        /*Save the last pressed button's ID*/
        data->btn_id = last_btn;
}

/*Get ID  (0, 1, 2 ..) of the pressed button*/
static int8_t button_get_pressed_id(void)
{
        uint8_t i;

        /*Check to buttons see which is being pressed (assume there are 2 buttons)*/
        for (i = 0; i < 2; i++) {
                /*Return the pressed button's ID*/
                if (button_is_pressed(i)) {
                        return i;
                }
        }

        /*No button pressed*/
        return -1;
}

/*Test if `id` button is pressed or not*/
static bool button_is_pressed(uint8_t id)
{

        /*Your code comes here*/

        return false;
}
#endif /* LV_PORT_INDEV_BUTTON_EN */

