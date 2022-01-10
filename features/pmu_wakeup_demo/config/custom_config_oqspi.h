/**
 ****************************************************************************************
 *
 * @file custom_config_oqspi.h
 *
 * @brief Board Support Package. User Configuration file for execution from OQSPI.
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


#define dg_configENABLE_PD_SNC_WHILE_ACTIVE     0

#define CONFIG_RETARGET

#define dg_configUSE_LP_CLK                     ( LP_CLK_32768 )
#define dg_configEXEC_MODE                      MODE_IS_CACHED
#define dg_configCODE_LOCATION                  NON_VOLATILE_IS_OQSPI_FLASH

#define dg_configUSE_WDOG                       (0)

#define dg_configFLASH_CONNECTED_TO             (FLASH_CONNECTED_TO_1V8F)


#define dg_configUSE_USB                        0
#define dg_configUSE_USB_CHARGER                0
#define dg_configALLOW_CHARGING_NOT_ENUM        1

#define dg_configUSE_SW_CURSOR                  (1)

/*************************************************************************************************\
 * FreeRTOS specific config
 */
#define OS_FREERTOS                              /* Define this to use FreeRTOS */
#define configTOTAL_HEAP_SIZE                    (24 * 1024)   /* This is the FreeRTOS Total Heap Size */

/*************************************************************************************************\
 * Peripheral specific config
 */

#define dg_configRF_ENABLE_RECALIBRATION        (0)

#define dg_configFLASH_ADAPTER                  (0)
#define dg_configNVMS_ADAPTER                   (0)
#define dg_configNVMS_VES                       (0)
#define dg_configNVPARAM_ADAPTER                (0)


#define dg_configUSE_HW_TIMER                   (1)

#define dg_configUSE_HW_QSPI                    (1)
#define dg_configUSE_HW_QSPI2                   (1)

#define dg_configFLASH_AUTODETECT               (1)
#define dg_configQSPIC2_DEV_AUTODETECT          (1)
#define dg_configFLASH_POWER_DOWN               (1)
#define dg_configOQSPI_FLASH_AUTODETECT         (1)
#define dg_configOQSPI_FLASH_POWER_DOWN         (1)
/*If daughterboard has Winbond W25Q64JWIM flash and segger_flash_loader will be used please uncomment the lines below */
//#define dg_configUSE_SEGGER_FLASH_LOADER        (1)
//#define dg_configOQSPI_FLASH_HEADER_FILE                 "oqspi_w25q64jwim.h"
//#define dg_configOQSPI_FLASH_CONFIG                      oqspi_w25q64jwim_cfg

#define dg_configSYSTEMVIEW                     (0)
#define dg_configUSE_HW_USB                     (0)

#define PWR_MGR_USE_TIMING_DEBUG                (1)
#define PWR_MGR_FUNCTIONAL_DEBUG                (0)

#define dg_configPMU_ADAPTER                    (1)
#define dg_configGPADC_ADAPTER                  (1)

#define dg_configXTAL32K_SETTLE_TIME            (2000)
#define dg_configUSE_SYS_TRNG                   (0)



/* Include bsp default values */
#include "bsp_defaults.h"
/* Include middleware default values */
#include "middleware_defaults.h"

#endif /* CUSTOM_CONFIG_OQSPI_H_ */
