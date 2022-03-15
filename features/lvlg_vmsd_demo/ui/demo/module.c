/**
 ****************************************************************************************
 *
 * @file modules.c
 *
 * @brief Modules source file
 *
 * Copyright (C) 2021-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include <stdio.h>
#include <string.h>
#include "module.h"

#define MAX_MODULES_NUM         (2)

ModulePropertyTypedef module_prop[MAX_MODULES_NUM];
uint16_t module_num = 0;

static uint8_t module_verify(ModuleItem_Typedef *module);

/**
 * @brief  Module Initialization.
 * @param  None.
 * @retval None.
 */
void module_init(void)
{
        module_num = 0;
        memset(module_prop, 0, sizeof(module_prop));
}

/**
 * @brief  Add module.
 * @param  module: pointer to data structure of type K_ModuleItem_Typedef
 * @retval module add status
 */
uint8_t module_add(ModuleItem_Typedef *module)
{
        if ((module_num < MAX_MODULES_NUM) && (module_verify(module) == 0)) {
                module_prop[module_num].module = module;
                module_num++;
                return 0;
        }
        else {
                return 1;
        }
}

/**
 * @brief  Remove module
 * @param  module: pointer to data structure of type K_ModuleItem_Typedef
 * @retval None
 */
void module_remove(ModuleItem_Typedef *module)
{
        uint8_t idx = 0;

        for (idx = 0; idx < MAX_MODULES_NUM; idx++) {
                if (module_prop[module_num].module->id == module->id) {
                        module_prop[module_num].module = NULL;
                        module_num--;
                }
        }
}

/**
 * @brief  Verify module.
 * @param  module: pointer to data structure of type K_ModuleItem_Typedef
 * @retval module ID status
 */
static uint8_t module_verify(ModuleItem_Typedef *module)
{
        uint8_t i = 0;

        for (i = 0; i < MAX_MODULES_NUM; i++) {
                if (module_prop[i].module != NULL) {
                        if (module_prop[i].module->id == module->id) {
                                return 1;
                        }
                }
        }
        return 0;
}
