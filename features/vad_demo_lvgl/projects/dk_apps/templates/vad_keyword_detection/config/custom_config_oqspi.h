/**
 ****************************************************************************************
 *
 * @file custom_config_ram.h
 *
 * @brief Custom configuration file for non-FreeRTOS applications executing from OQSPI.
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
#ifndef CUSTOM_CONFIG_OQSPI_H_
#define CUSTOM_CONFIG_OQSPI_H_

#include "bsp_definitions.h"

#define CONFIG_RETARGET

#define CONFIG_RETARGET_UART_BAUDRATE           HW_UART_BAUDRATE_921600

#define VAD_SYSTEM_CLK                          sysclk_PLL160

#define dg_configOQSPI_FLASH_AUTODETECT                 (1) // to detect all available XiP on DB. increases bin file
#define dg_configFLASH_AUTODETECT                       (1)
#define dg_configUNDISCLOSED_UNSUPPORTED_FLASH_DEVICES  (1)

#define dg_configUSE_LP_CLK                     ( LP_CLK_32768 )
#define dg_configEXEC_MODE                      MODE_IS_CACHED
#define dg_configCODE_LOCATION                  NON_VOLATILE_IS_OQSPI_FLASH

#define dg_configUSE_WDOG                       (0)

#define dg_configFLASH_CONNECTED_TO             (FLASH_CONNECTED_TO_1V8F)
#define dg_configOQSPI_FLASH_POWER_DOWN         (1)     // 2-3uA


#define dg_configUSE_SW_CURSOR                  (1)

#define dg_configQSPI_CODE_SIZE_AA              (480 * 1024)  /* Includes CMI and SNC, def value 384*/

/*************************************************************************************************\
 * FreeRTOS specific config
 */
#define OS_FREERTOS                              /* Define this to use FreeRTOS */
#ifdef dg_configLCD_GUI
#define configTOTAL_HEAP_SIZE                    (45000 + 17 * 1024)   /* This is the FreeRTOS Total Heap Size */
#else
#define configTOTAL_HEAP_SIZE                    (45000)   /* This is the FreeRTOS Total Heap Size */
#endif

/*************************************************************************************************\
 * Peripheral specific config
 */

#define dg_configUSE_HW_VAD                     (1)

#define dg_configRF_ENABLE_RECALIBRATION        (0)

#define dg_configFLASH_ADAPTER                  (0)
#define dg_configNVMS_ADAPTER                   (0)
#define dg_configNVMS_VES                       (0)

#define dg_configUSE_HW_USB                     (0)

#define dg_configUSE_SYS_TRNG                   (0)
#define dg_configUSE_SYS_DRBG                   (0)

#define PWR_MGR_USE_TIMING_DEBUG                (0)// Use to debug sleep- wake_up period

// Enable audio peripherals
#define dg_configUSE_HW_SDADC                   (1)
#define dg_configUSE_HW_PDM                     (1)
#define dg_configUSE_HW_SRC                     (1)
#define dg_configUSE_HW_PCM                     (1)
#define dg_configUSE_SYS_AUDIO_MGR              (1)
#define dg_configUSE_SYS_AUDIO_SINGLE_PATH      (0)

#define dg_configUSE_HW_DMA                     (1)

#ifdef dg_configLCD_GUI
#define AD_LCDC_DEFAULT_CLK                     VAD_SYSTEM_CLK  // Else the lcd changes master clock to 32MHz, default

#define dg_configLCDC_ADAPTER                   (1)
#define dg_configUSE_HW_LCDC                    (1)
#define dg_configUSE_E120A390QSR                (1)

#define DEMO_RESX                               (390)
#define DEMO_RESY                               (390)

#define LV_CONF_INCLUDE_SIMPLE
#define LV_LVGL_H_INCLUDE_SIMPLE
#endif /* dg_configLCD_GUI */

/* Include bsp default values */
#include "bsp_defaults.h"
/* Include middleware default values */
#include "middleware_defaults.h"

#endif /* CUSTOM_CONFIG_OQSPI_H_ */
