/**
 ****************************************************************************************
 *
 * @file menu_list_screen.h
 *
 * @brief menu list screen header file
 *
 * Copyright (C) 2021-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#ifndef UI_DEMO_SCREENS_MENU_LIST_SCREEN_H_
#define UI_DEMO_SCREENS_MENU_LIST_SCREEN_H_

/*
 *      INCLUDES
 *****************************************************************************************
 */
#include "module.h"

/*
 * GLOBAL PROTOTYPES
 *****************************************************************************************
 */
ModuleItem_Typedef menu_list;
void menu_list_screen(lv_obj_t *parent_obj, lv_style_t *style_screen, lv_coord_t x, lv_coord_t y);

#endif /* UI_DEMO_SCREENS_MENU_LIST_SCREEN_H_ */
