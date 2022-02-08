/**
 ****************************************************************************************
 *
 * @file ble_peripheral_config.h
 *
 * @brief Application configuration
 *
 * Copyright (C) 2015-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef BLE_PERIPHERAL_CONFIG_H_
#define BLE_PERIPHERAL_CONFIG_H_

// enable debug service (see readme.txt for details)
#define CFG_DEBUG_SERVICE    (0)

#define CFG_BAS              (0)     // register BAS service
#define CFG_BAS_MULTIPLE     (0)     // add 2 instances of BAS service
#define CFG_CTS              (0)     // register CTS
#define CFG_DIS              (0)     // register DIS
#define CFG_DIS_FULL         (0)     // add all possible characteristics to DIS
#define CFG_SCPS             (0)     // register ScPS
#define CFG_USER_SERVICE     (0)     // register custom service (using 128-bit UUIDs)

#define CFG_MY_CUSTOM_SERVICE   (1)     // my custom service

#endif /* BLE_PERIPHERAL_CONFIG_H_ */
