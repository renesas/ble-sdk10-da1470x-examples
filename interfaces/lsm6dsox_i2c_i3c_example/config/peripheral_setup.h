/**
 ****************************************************************************************
 *
 * @file periph_setup.h
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
 ****************************************************************************************
 */
#ifndef _PERIPH_SETUP_H_
#define _PERIPH_SETUP_H_

#include "hw_gpio.h"
#include "lsm6dsox_reg.h"
/* I2X Interface */
#define I2C_GPIO_LEVEL HW_GPIO_POWER_V33
#define I3C_GPIO_LEVEL HW_GPIO_POWER_V33



/* I2C */
#define I2C_PORT    HW_GPIO_PORT_1
#define I3C_PORT    HW_GPIO_PORT_1
#define I2C_MASTER_SCL_PIN  HW_GPIO_PIN_12
#define I2C_MASTER_SDA_PIN  HW_GPIO_PIN_11
#define I3C_MASTER_SCL_PIN  HW_GPIO_PIN_12
#define I3C_MASTER_SDA_PIN  HW_GPIO_PIN_11

#define LSM6DSOX_ADDRESS            ( 0x6A )
#define LSM6DSOX_ADDRESS_DYNAMIC    ( 0x25 )

#define I2C_AD_CONFIG_MASTER_I2C_CTRL       (HW_I2C1)
#define I3C_AD_CONFIG_MASTER_I3C_CTRL       (HW_I3C)
#endif /* _PERIPH_SETUP_H_ */
