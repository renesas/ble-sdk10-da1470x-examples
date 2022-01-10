/*
 * platform_devices.h
 *
 *  Created on: Aug 25, 2021
 *      Author: akleinen
 */

#ifndef CONFIG_PLATFORM_DEVICES_H_
#define CONFIG_PLATFORM_DEVICES_H_

#include "ad_i2c.h"
#include "peripheral_setup.h"

/**
 * \brief I2C device handle
 */
typedef const void* i2c_device;


/*
 * I2C DEVICES
 *****************************************************************************************
 */
#if dg_configI2C_ADAPTER || dg_configUSE_HW_I2C

/**
 * \brief EEPROM 24C08 device
 */
extern i2c_device EEPROM_24C08;

#endif /* dg_configI2C_ADAPTER || dg_configUSE_HW_I2C */

#endif /* CONFIG_PLATFORM_DEVICES_H_ */
