/**
 ****************************************************************************************
 *
 * @file modules.h
 *
 * @brief Modules header file
 *
 * Copyright (C) 2021-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#ifndef UI_DEMO_MODULE_H_
#define UI_DEMO_MODULE_H_

#include "lvgl.h"

typedef struct
{
        uint8_t id;
        const char *name;
        const lv_img_dsc_t *icon;
        void (*startup)(lv_obj_t*, lv_style_t*, lv_coord_t, lv_coord_t);
        void (*DirectOpen)(char*);
}
ModuleItem_Typedef;

typedef struct
{
        uint8_t idx;
        uint8_t in_use;
        const ModuleItem_Typedef *module;
}
ModulePropertyTypedef;

extern ModulePropertyTypedef module_prop[];
void module_init(void);
uint8_t module_add(ModuleItem_Typedef *module);
void module_remove(ModuleItem_Typedef *module);

#endif /* UI_DEMO_MODULE_H_ */
