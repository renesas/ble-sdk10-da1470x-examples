/**
 ****************************************************************************************
 *
 * @file custom_config_snc.h
 *
 * @brief Custom configuration file for applications executing from RAM on SNC.
 *
 * Copyright (C) 2021 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#ifndef CUSTOM_CONFIG_SNC_H_
#define CUSTOM_CONFIG_SNC_H_

#include "bsp_definitions.h"

#define CONFIG_USE_SNC
#define CONFIG_RETARGET

/*************************************************************************************************\
 * System configuration
 */
#define dg_configUSE_LP_CLK                     ( LP_CLK_32768 )
#define dg_configCODE_LOCATION                  ( NON_VOLATILE_IS_NONE )

#define dg_configUSE_WDOG                       ( 1 )

#define dg_configFLASH_CONNECTED_TO             ( FLASH_IS_NOT_CONNECTED )

#define dg_configUSE_SW_CURSOR                  ( 1 )

/*************************************************************************************************\
 * Dialog CoRoutines configuration
 */
#define OS_DGCOROUTINES                         /* Define this to use Dialog CoRoutines */
#define configTOTAL_HEAP_SIZE                   ( 10000 )  /* Dialog CoRoutines Total Heap Size */

#define __STACK_SIZE                            ( 0x300 )

/*************************************************************************************************\
 * Peripherals configuration
 */
#define dg_configFLASH_ADAPTER                  ( 0 )
#define dg_configNVMS_ADAPTER                   ( 0 )
#define dg_configNVMS_VES                       ( 0 )
#define dg_configCRYPTO_ADAPTER                 ( 0 )

#define SNC_HOLD                                ( 0 )

#ifdef CONFIG_RETARGET
#undef dg_configUSE_HW_UART
#define dg_configUSE_HW_UART                    ( 1 )
#else
#define dg_configUSE_HW_UART                    ( 0 )
#endif

/* Include bsp default values */
#include "bsp_defaults.h"
/* Include middleware default values */
#include "middleware_defaults.h"

#endif /* CUSTOM_CONFIG_SNC_H_ */
