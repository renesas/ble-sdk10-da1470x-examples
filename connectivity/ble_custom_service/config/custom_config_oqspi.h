/**
 ****************************************************************************************
 *
 * @file custom_config_oqspi.h
 *
 * @brief Board Support Package. User Configuration file for cached OQSPI mode.
 *
 * Copyright (C) 2020-2021 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#ifndef CUSTOM_CONFIG_OQSPI_H_
#define CUSTOM_CONFIG_OQSPI_H_

#include "bsp_definitions.h"

#define CONFIG_USE_BLE
#undef CONFIG_USE_FTDF

/*************************************************************************************************\
 * System configuration
 */
#if DEVICE_FPGA
#define dg_configUSE_LP_CLK                     ( LP_CLK_32000 )
#else
#define dg_configUSE_LP_CLK                     ( LP_CLK_32768 )
#endif
#define dg_configEXEC_MODE                      ( MODE_IS_CACHED )
#define dg_configCODE_LOCATION                  ( NON_VOLATILE_IS_OQSPI_FLASH )

#define dg_configUSE_WDOG                       ( 1 )

#define dg_configFLASH_CONNECTED_TO             ( FLASH_CONNECTED_TO_1V8F )
#define dg_configFLASH_POWER_DOWN               ( 0 )

#define dg_configUSE_SW_CURSOR                  ( 1 )

/*************************************************************************************************\
 * FreeRTOS configuration
 */
#define OS_FREERTOS                             /* Define this to use FreeRTOS */
#define configTOTAL_HEAP_SIZE                   ( 35260 )   /* FreeRTOS Total Heap Size */

/*************************************************************************************************\
 * Peripherals configuration
 */
#define dg_configFLASH_ADAPTER                  ( 1 )
#define dg_configNVMS_ADAPTER                   ( 1 )
#define dg_configNVMS_VES                       ( 1 )
#define dg_configNVPARAM_ADAPTER                ( 1 )
#if DEVICE_FPGA
#define dg_configGPADC_ADAPTER                  ( 0 )
#else
#define dg_configGPADC_ADAPTER                  ( 1 )
#endif

#if dg_configAUTOTEST_ENABLE
        #define CONFIG_RETARGET
        #define __HEAP_SIZE  0x0800
#endif

/*************************************************************************************************\
 * BLE configuration
 */
#define CONFIG_USE_BLE_SERVICES                 ( 1 )

#define dg_configBLE_CENTRAL                    ( 0 )
#define dg_configBLE_GATT_CLIENT                ( 0 )
#define dg_configBLE_OBSERVER                   ( 0 )
#define dg_configBLE_BROADCASTER                ( 0 )
#define dg_configBLE_L2CAP_COC                  ( 0 )

/* Include bsp default values */
#include "bsp_defaults.h"
/* Include middleware default values */
#include "middleware_defaults.h"

#endif /* CUSTOM_CONFIG_OQSPI_H_ */
