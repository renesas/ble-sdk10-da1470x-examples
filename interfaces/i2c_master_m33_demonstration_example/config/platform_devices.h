/**
 ****************************************************************************************
 *
 * @file platform_devices.h
 *
 * @brief Platform devices header file
 *
 * Copyright (C) 2020-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef CONFIG_PLATFORM_DEVICES_H_
#define CONFIG_PLATFORM_DEVICES_H_

#ifndef __CONST
#define __CONST const
#endif

typedef __CONST void* PERIPHERAL_DEVICE;

#if dg_configI2C_ADAPTER
extern PERIPHERAL_DEVICE I2C_DEVICE_MASTER;
#endif

#endif /* CONFIG_PLATFORM_DEVICES_H_ */
