/**
 ****************************************************************************************
 *
 * @file custom_config_ram.h
 *
 * @brief Board Support Package. User Configuration file for execution from RAM.
 *
 * Copyright (C) 2015-2021 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef CUSTOM_CONFIG_RAM_H_
#define CUSTOM_CONFIG_RAM_H_

#include "bsp_definitions.h"

#define CONFIG_RETARGET

#define dg_configUSE_LP_CLK                     ( LP_CLK_32768 )
#define dg_configCODE_LOCATION                  NON_VOLATILE_IS_NONE

#define dg_configUSE_WDOG                       (1)

#define dg_configFLASH_CONNECTED_TO             (FLASH_IS_NOT_CONNECTED)


#define dg_configUSE_SW_CURSOR                  (1)

#define dg_configUSE_HW_QSPI                    (0)

/*************************************************************************************************\
 * FreeRTOS specific config
 */
#define OS_FREERTOS                              /* Define this to use FreeRTOS */
#define configTOTAL_HEAP_SIZE                    14000   /* This is the FreeRTOS Total Heap Size */

/*************************************************************************************************\
 * Peripheral specific config
 */

#define dg_configRF_ENABLE_RECALIBRATION        (0)

#define dg_configFLASH_ADAPTER                  (0)
#define dg_configNVMS_ADAPTER                   (0)
#define dg_configNVMS_VES                       (0)

#define dg_configUSE_HW_USB                     (0)


#define dg_configUSE_SYS_TRNG                   (0)
#define dg_configUSE_SYS_DRBG                   (0)

/*
 * Enable/disable the GPIO sub block of the WKUP controller. Valid values are:
 *
 * 1 --> Enable the GPIO block as well as non-debounced IO IRQs
 * 0 --> Disable the GPIO block.
 */
#define WKUP_GPIO_P1_BLOCK_ENABLE               (0)

/*
 * Enable/disable the KEY sub block of the WKUP controller. Valid values are:
 *
 * 1 --> Enable the KEY block as well as debounced IO IRQs
 * 0 --> Disable the KEY block.
 */
#define WKUP_KEY_BLOCK_ENABLE                   (1)

/* Include bsp default values */
#include "bsp_defaults.h"
/* Include middleware default values */
#include "middleware_defaults.h"

#endif /* CUSTOM_CONFIG_RAM_H_ */
