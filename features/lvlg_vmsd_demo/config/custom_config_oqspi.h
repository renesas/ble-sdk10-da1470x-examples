/**
 ****************************************************************************************
 *
 * @file custom_config_oqspi.h
 *
 * @brief Custom configuration file for non-FreeRTOS applications executing from OQSPI.
 *
 * Copyright (C) 2021-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef CUSTOM_CONFIG_OQSPI_H_
#define CUSTOM_CONFIG_OQSPI_H_

#include "bsp_definitions.h"

/* Should be enabled for untrimmed/T0/T0- samples in custom configuration file */
#define TEST_WITH_UNTRIMMED_SILICON             ( 0 )

#define CONFIG_USE_BLE

#define CONFIG_RETARGET

#if DEVICE_FPGA
#define dg_configUSE_LP_CLK                     ( LP_CLK_32000 )
#else
#define dg_configUSE_LP_CLK                     ( LP_CLK_32768 )
#endif
#define dg_configIMAGE_SETUP                    ( DEVELOPMENT_MODE )
#define dg_configEXEC_MODE                      MODE_IS_CACHED
#define dg_configCODE_LOCATION                  NON_VOLATILE_IS_OQSPI_FLASH

#define dg_configMEM_RETENTION_MODE             (0x1F)
#define dg_configSHUFFLING_MODE                 (0x3)

#define dg_configUSE_WDOG                       (1)

#define dg_configFLASH_CONNECTED_TO             (FLASH_CONNECTED_TO_1V8)

#define dg_configFLASH_AUTODETECT               (1)
#define dg_configFLASH_POWER_DOWN               (1)

#define dg_configPOWER_1V8_ACTIVE               (1)
#define dg_configPOWER_1V8_SLEEP                (1)

#define dg_configOQSPI_FLASH_AUTODETECT         (1)
//#define dg_configOQSPI_FLASH_HEADER_FILE        "oqspi_mx66um1g45g.h"
//#define dg_configOQSPI_FLASH_CONFIG             oqspi_mx66um1g45g_cfg


#define dg_configUSE_USB_CHARGER                (0)
#define dg_configALLOW_CHARGING_NOT_ENUM        (1)
#define dg_configUSE_USB_ENUMERATION            (1)

#define dg_configUSE_SW_CURSOR                  (1)

#define dg_configUSE_SYS_TRNG                   (1)

/*************************************************************************************************\
 * FreeRTOS configuration
 */
#define OS_FREERTOS                             /* Define this to use FreeRTOS */
#define configTOTAL_HEAP_SIZE                   (36 * 1024)  //( 32 * 1024 )  /* FreeRTOS Total Heap Size */

#define dg_configQSPI_CODE_SIZE_AA              (768 * 1024) //(480 * 1024)  /* Includes CMI and SNC, def value 384*/

/*************************************************************************************************\
 * Peripheral specific config
 */

#define dg_configGPADC_ADAPTER                  (1)

#define dg_configFLASH_ADAPTER                  (1)
#define dg_configCRYPTO_ADAPTER                 (0)
#define dg_configNVMS_ADAPTER                   (1)
#define dg_configNVMS_VES                       (1)
#define dg_configRF_ENABLE_RECALIBRATION        (1)

#ifdef CONFIG_RETARGET
#define dg_configUSE_HW_DMA                     (1)
#define dg_configUSE_HW_UART                    (1)
#else
#define dg_configUSE_HW_DMA                     (0)
#define dg_configUSE_HW_UART                    (0)
#endif

#define dg_configLCDC_ADAPTER                   ( 1 )
#define dg_configUSE_HW_LCDC                    ( 1 )
#define dg_configI2C_ADAPTER                    ( 1 )
#define dg_configUSE_HW_I2C                     ( 1 )
#define dg_configUSE_HW_WKUP                    ( 1 )

#define dg_configUSE_HW_QSPI2                    ( 1 )
#define dg_configQSPIC2_DEV_AUTODETECT           ( 1 )

/*************************************************************************************************\
 * BLE configuration
 */
#define CONFIG_USE_BLE_SERVICES

#define dg_configBLE_CENTRAL                    ( 0 )
#define dg_configBLE_GATT_CLIENT                ( 0 )
#define dg_configBLE_OBSERVER                   ( 0 )
#define dg_configBLE_BROADCASTER                ( 0 )
#define dg_configBLE_L2CAP_COC                  ( 0 )

#define defaultBLE_ATT_DB_CONFIGURATION         ( 0x10 )  // Peripheral Pref. Conn. Param. attribute
#define defaultBLE_PPCP_INTERVAL_MIN            ( BLE_CONN_INTERVAL_FROM_MS( 500 ) )    // 500 ms
#define defaultBLE_PPCP_INTERVAL_MAX            ( BLE_CONN_INTERVAL_FROM_MS( 750 ) )    // 750 ms
#define defaultBLE_PPCP_SLAVE_LATENCY           ( 0 )                                   // 0 events
#define defaultBLE_PPCP_SUP_TIMEOUT             ( BLE_SUPERVISION_TMO_FROM_MS( 6000 ) ) // 6000 ms

/*************************************************************************************************\
 * Display model selection. Note that one display model can be selected at a time.
 */
#define dg_configUSE_DT280QV10CT                ( 0 )
#define dg_configUSE_HM80160A090                ( 0 )
#define dg_configUSE_LPM012M134B                ( 0 )
#define dg_configUSE_LPM013M091A                ( 0 )
#define dg_configUSE_NHD43480272EFASXN          ( 0 )
#define dg_configUSE_MCT024L6W240320PML         ( 0 )
#define dg_configUSE_PSP27801                   ( 0 )
#define dg_configUSE_E1394AA65A                 ( 0 )
#define dg_configUSE_T1D3BP006                  ( 0 )
#define dg_configUSE_T1D54BP002                 ( 0 )
#define dg_configUSE_LS013B7DH06                ( 0 )
#define dg_configUSE_LS013B7DH03                ( 0 )

#define dg_configUSE_MRB3973_DBIB               ( 0 )
#define dg_configUSE_XSJ120TY2401_QSPI          ( 0 )
#define dg_configUSE_XSJ120TY2401_DSPI          ( 0 )
#define dg_configUSE_BOE139F454SM               ( 0 )
#define dg_configUSE_E120A390QSR                ( 1 )

#define LV_CONF_INCLUDE_SIMPLE
#define LV_LVGL_H_INCLUDE_SIMPLE
/*************************************************************************************************\
 * Touch controller selection. Note that one touch driver can be selected at a time.
 */
#ifdef PERFORMANCE_METRICS
#define dg_configUSE_TOUCH_SIMULATION           ( 1 )
#else
#define dg_configUSE_FT6206                     ( 0 )
#define dg_configUSE_FT5306                     ( 0 )
#define dg_configUSE_ZT2628                     ( 1 )
#endif

#if DEVICE_FPGA
#define dg_configUSE_SEGGER_FLASH_LOADER        ( 1 )
#endif

#if dg_configUSE_BOE139F454SM
#define AD_LCDC_DEFAULT_CLK                     sysclk_RCHS_96
#else
#define AD_LCDC_DEFAULT_CLK                     sysclk_RCHS_96//sysclk_PLL160
#endif

/* Include bsp default values */
#include "bsp_defaults.h"
/* Include middleware default values */
#include "middleware_defaults.h"

#endif /* CUSTOM_CONFIG_OQSPI_H_ */
