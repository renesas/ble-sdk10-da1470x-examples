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

#define SPI_MASTER_DO_PORT         ( HW_GPIO_PORT_1 )
#define SPI_MASTER_DO_PIN          ( HW_GPIO_PIN_10 )

#define SPI_MASTER_CLK_PORT        ( HW_GPIO_PORT_1 )
#define SPI_MASTER_CLK_PIN         ( HW_GPIO_PIN_11 )

#define SPI_MASTER_DI_PORT         ( HW_GPIO_PORT_1 )
#define SPI_MASTER_DI_PIN          ( HW_GPIO_PIN_12 )

#define SPI_MASTER_CS_PORT         ( HW_GPIO_PORT_1 )
#define SPI_MASTER_CS_PIN          ( HW_GPIO_PIN_13 )

#define SPI_SLAVE_DO_PORT          ( HW_GPIO_PORT_1 )
#define SPI_SLAVE_DO_PIN           ( HW_GPIO_PIN_14 )

#define SPI_SLAVE_CLK_PORT         ( HW_GPIO_PORT_1 )
#define SPI_SLAVE_CLK_PIN          ( HW_GPIO_PIN_15 )

#define SPI_SLAVE_DI_PORT          ( HW_GPIO_PORT_1 )
#define SPI_SLAVE_DI_PIN           ( HW_GPIO_PIN_16 )

#define SPI_SLAVE_CS_PORT          ( HW_GPIO_PORT_1 )
#define SPI_SLAVE_CS_PIN           ( HW_GPIO_PIN_17 )

#endif /* CONFIG_PERIPHERAL_SETUP_H_ */
