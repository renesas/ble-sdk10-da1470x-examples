/**
 ****************************************************************************************
 *
 * @file watch_face_screen.h
 *
 * @brief Watch face screen header file
 *
 * Copyright (C) 2021-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#ifndef UI_DEMO_SCREENS_WATCH_FACE_SCREEN_H_
#define UI_DEMO_SCREENS_WATCH_FACE_SCREEN_H_

/*
 *      INCLUDES
 *****************************************************************************************
 */
#include "module.h"

/*
 * GLOBAL PROTOTYPES
 *****************************************************************************************
 */
ModuleItem_Typedef watch_face;
void watch_face_screen(lv_obj_t *parent_obj, lv_style_t *style_screen, lv_coord_t x, lv_coord_t y);

#endif /* UI_DEMO_SCREENS_WATCH_FACE_SCREEN_H_ */
