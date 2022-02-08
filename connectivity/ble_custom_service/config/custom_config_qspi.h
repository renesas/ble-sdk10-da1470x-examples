/**
 ****************************************************************************************
 *
 * @file custom_config_qspi.h
 *
 * @brief Board Support Package. User Configuration file for cached QSPI mode.
 *
 * Copyright (C) 2015-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#ifndef CUSTOM_CONFIG_QSPI_H_
#define CUSTOM_CONFIG_QSPI_H_

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
#define dg_configCODE_LOCATION                  ( NON_VOLATILE_IS_FLASH )

#define dg_configUSE_WDOG                       ( 1 )

#define dg_configFLASH_CONNECTED_TO             ( FLASH_CONNECTED_TO_1V8 )
#define dg_configFLASH_POWER_DOWN               ( 0 )
#define dg_configPOWER_1V8_ACTIVE               ( 1 )
#define dg_configPOWER_1V8_SLEEP                ( 1 )

#if (DEVICE_FAMILY == DA1468X)
#define dg_configBATTERY_TYPE                   ( BATTERY_TYPE_LIMN2O4 )
#define dg_configBATTERY_CHARGE_CURRENT         ( 2 )    // 30mA
#define dg_configBATTERY_PRECHARGE_CURRENT      ( 20 )   // 2.1mA
#define dg_configBATTERY_CHARGE_NTC             ( 1 )    // disabled
#endif /* DEVICE_FAMILY == DA1468X */

#define dg_configUSE_SW_CURSOR                  ( 1 )

#define dg_configCACHEABLE_QSPI_AREA_LEN        ( NVMS_PARAM_PART_start - MEMORY_QSPIF_BASE )

#define dg_configTESTMODE_MEASURE_SLEEP_CURRENT ( 0 )

#if (DEVICE_FAMILY == DA1468X)
/*************************************************************************************************\
 * Memory layout configuration
 */
#define dg_configQSPI_CACHED_RAM_SIZE_AE           ( 60 * 1024 )
#define dg_configQSPI_CACHED_RETRAM_0_SIZE_AE      ( 68 * 1024 )
#define dg_configQSPI_CACHED_RAM_SIZE_BB           ( 62 * 1024 )
#define dg_configQSPI_CACHED_RETRAM_0_SIZE_BB      ( 66 * 1024 )
#endif

/*************************************************************************************************\
 * FreeRTOS configuration
 */
#define OS_FREERTOS                              /* Define this to use FreeRTOS */
#if (DEVICE_FAMILY == DA1468X)
#define configTOTAL_HEAP_SIZE                    ( 16384 )   /* FreeRTOS Total Heap Size */
#elif (DEVICE_FAMILY == DA1469X)
#define configTOTAL_HEAP_SIZE                    ( 22972 )   /* FreeRTOS Total Heap Size */
#endif

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
#if (DEVICE_FAMILY == DA1469X)
#define dg_configUSE_HW_SENSOR_NODE             ( 1 )
#define dg_configUSE_SNC_HW_GPADC               ( 1 )
#define dg_configSNC_ADAPTER                    ( 1 )
#endif
#endif

#if dg_configAUTOTEST_ENABLE
        #define CONFIG_RETARGET
        #define __HEAP_SIZE  0x0800
#endif

/*************************************************************************************************\
 * BLE configuration
 */
#define CONFIG_USE_BLE_SERVICES

#define dg_configBLE_CENTRAL                    ( 0 )
#define dg_configBLE_GATT_CLIENT                ( 0 )
#define dg_configBLE_OBSERVER                   ( 0 )
#define dg_configBLE_BROADCASTER                ( 0 )
#define dg_configBLE_L2CAP_COC                  ( 0 )

/* Include bsp default values */
#include "bsp_defaults.h"
/* Include middleware default values */
#include "middleware_defaults.h"

#endif /* CUSTOM_CONFIG_QSPI_H_ */
