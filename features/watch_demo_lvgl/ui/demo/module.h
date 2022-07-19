/**
 ****************************************************************************************
 *
 * @file modules.h
 *
 * @brief Modules header file
 *
 * Copyright (c) 2022 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor. By using this Software
 * you agree that Dialog Semiconductor retains all intellectual property and proprietary
 * rights in and to this Software and any use, reproduction, disclosure or distribution
 * of the Software without express written permission or a license agreement from Dialog
 * Semiconductor is strictly prohibited. This Software is solely for use on or in
 * conjunction with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR AS
 * REQUIRED BY LAW, THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE PROVIDED
 * IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR BY LAW, IN NO EVENT SHALL DIALOG
 * SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE.
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
