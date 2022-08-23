/**
 ****************************************************************************************
 *
 * @file peripheral_setup.h
 *
 * @brief File that includes all I/O pins definitions
 *
 * Copyright (C) 2020-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include "hw_gpio.h"

#ifndef CONFIG_PERIPHERAL_SETUP_H_
#define CONFIG_PERIPHERAL_SETUP_H_

#define I2C_MASTER_SCL_PORT         ( HW_GPIO_PORT_1 )
#define I2C_MASTER_SCL_PIN          ( HW_GPIO_PIN_12 )

#define I2C_MASTER_SDA_PORT         ( HW_GPIO_PORT_1 )
#define I2C_MASTER_SDA_PIN          ( HW_GPIO_PIN_11 )

#define I2C_SLAVE_ADDRESS           ( 0x48 )

#define THERMO3_ALERT_PORT          ( HW_GPIO_PORT_0 )
#define THERMO3_ALERT_PIN           ( HW_GPIO_PIN_24 )

#endif /* CONFIG_PERIPHERAL_SETUP_H_ */
