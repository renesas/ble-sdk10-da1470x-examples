/**
 ****************************************************************************************
 *
 * @file platform_devices.h
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
#ifndef PLATFORM_DEVICES_H_
#define PLATFORM_DEVICES_H_




#if defined(dg_configUseI2CHandling)
/*
 * I2C DEVICES
 *****************************************************************************************
 */
/**
 * \brief I2C device handle
 */
typedef const void* i2c_device;
/**
 * \brief I2C peer devices
 */
extern i2c_device LSM6DSOX_DEVICE;

#elif defined(dg_configUseI3CHandling)
/*
 * I3C DEVICES
 *****************************************************************************************
 */
/**
 * \brief I3C device handle
 */
typedef const void* i3c_device;
/**
 * \brief I3C peer devices
 */
extern i3c_device LSM6DSOX_DEVICE;

#endif

#endif /* PLATFORM_DEVICES_H_ */
