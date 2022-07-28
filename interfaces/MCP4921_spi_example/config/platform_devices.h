/**
 ****************************************************************************************
 *
 * @file platform_devices.c
 *
 * @brief Configuration of devices connected to board
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
 ***************************************************************************************
 */

#ifndef PLATFORM_DEVICES_H_
#define PLATFORM_DEVICES_H_

#include <ad_spi.h>
#include "peripheral_setup.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (dg_configSPI_ADAPTER == 1) || (dg_configUSE_HW_SPI == 1)

#if (MCP4921_MODULE_USED == 1)
/**
 * \brief SPI device handle
 */
typedef const void* spi_device;

/* List of devices */
extern spi_device MCP4921_DEVICE;

#endif /* MCP482_MODULE_USED */
#endif /* dg_configSPI_ADAPTER || dg_configUSE_HW_SPI */

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_DEVICES_H_ */