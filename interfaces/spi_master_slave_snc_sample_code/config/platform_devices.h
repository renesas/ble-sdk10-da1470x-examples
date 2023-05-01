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

#if dg_configSPI_ADAPTER
extern PERIPHERAL_DEVICE SPI_DEVICE_MASTER;
extern PERIPHERAL_DEVICE SPI_DEVICE_SLAVE;
#endif

#endif /* CONFIG_PLATFORM_DEVICES_H_ */
