/**
 ****************************************************************************************
 *
 * @file custom_config_ram.h
 *
 * @brief Board Support Package. User Configuration file for execution from RAM.
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
#ifndef CUSTOM_CONFIG_RAM_H_
#define CUSTOM_CONFIG_RAM_H_

#include "bsp_definitions.h"

#define CONFIG_RETARGET
#define dg_configUseI3CHandling
//#define dg_configUseI2CHandling
#define dg_configUSE_LP_CLK                     ( LP_CLK_32768 )
#define dg_configCODE_LOCATION                  NON_VOLATILE_IS_NONE

#define dg_configUSE_WDOG                       (0)
#if defined (dg_configUseI3CHandling)
#define dg_configUSE_HW_I3C                     (1)
#define dg_configI3C_ADAPTER                    (1)
#define dg_configI3C_DMA_SUPPORT                (0)
#elif defined(dg_configUseI2CHandling)
#define dg_configUSE_HW_I2C                     (1)
#define dg_configI2C_ADAPTER                    (1)
#endif
#define dg_configFLASH_CONNECTED_TO             (FLASH_IS_NOT_CONNECTED)


#define dg_configUSE_SW_CURSOR                  (1)

/*************************************************************************************************\
 * FreeRTOS specific config
 */
#define OS_FREERTOS                              /* Define this to use FreeRTOS */
#define configTOTAL_HEAP_SIZE                    14000   /* This is the FreeRTOS Total Heap Size */

/*************************************************************************************************\
 * Peripheral specific config
 */
#define CONFIG_I2C_USE_SYNC_TRANSACTIONS        (1)
#define dg_configRF_ENABLE_RECALIBRATION        (0)

#define dg_configFLASH_ADAPTER                  (0)
#define dg_configNVMS_ADAPTER                   (0)
#define dg_configNVMS_VES                       (0)
#define HW_I2C_SLAVE_SUPPORT                    (0)
#define dg_configUSE_HW_USB                     (0)


#define dg_configUSE_SYS_TRNG                   (0)
#define dg_configUSE_SYS_DRBG                   (0)
//#define LSM6DSOX_ACTIVITY_DETECT
#define  LSM6DSOX_FIFO
/* Include bsp default values */
#include "bsp_defaults.h"
/* Include middleware default values */
#include "middleware_defaults.h"

#endif /* CUSTOM_CONFIG_RAM_H_ */
