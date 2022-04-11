/**
 ****************************************************************************************
 *
 * @file lv_port_disp.h
 *
 * @brief Display driver
 *
 * Copyright (C) 2021-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#ifndef LV_PORT_DISP_H
#define LV_PORT_DISP_H

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

/*********************
 *      DEFINES
 *********************/
#ifndef LV_PORT_DISP_HOR_RES
#define LV_PORT_DISP_HOR_RES                    (DEMO_RESX)
#endif

#ifndef LV_PORT_DISP_VER_RES
#define LV_PORT_DISP_VER_RES                    (DEMO_RESY)
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_port_disp_init(void);
#if TWO_LAYERS_HORIZONTAL_SLIDING
void lv_port_disp_slide_init(lv_obj_t *obj, lv_dir_t dir);
void lv_port_disp_slide(int posXLayer0, int posXLayer1);
void lv_port_disp_slide_end();
#endif

/**********************
 *      MACROS
 **********************/

#endif /* LV_PORT_DISP_H */
