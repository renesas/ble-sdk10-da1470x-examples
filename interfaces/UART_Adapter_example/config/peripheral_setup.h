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

#define UART1_RX_PORT           ( HW_GPIO_PORT_1 )
#define UART1_RX_PIN            ( HW_GPIO_PIN_8 )
#define UART1_TX_PORT           ( HW_GPIO_PORT_1 )
#define UART1_TX_PIN            ( HW_GPIO_PIN_9 )

#define UART2_RX_PORT           ( HW_GPIO_PORT_2 )
#define UART2_RX_PIN            ( HW_GPIO_PIN_1 )
#define UART2_TX_PORT           ( HW_GPIO_PORT_0 )
#define UART2_TX_PIN            ( HW_GPIO_PIN_8 )
#define UART2_RTS_PORT          ( HW_GPIO_PORT_0 )
#define UART2_RTS_PIN           ( HW_GPIO_PIN_29 )
#define UART2_CTS_PORT          ( HW_GPIO_PORT_0 )
#define UART2_CTS_PIN           ( HW_GPIO_PIN_11 )

#define UART3_RX_PORT           ( HW_GPIO_PORT_0 )
#define UART3_RX_PIN            ( HW_GPIO_PIN_26 )
#define UART3_TX_PORT           ( HW_GPIO_PORT_0 )
#define UART3_TX_PIN            ( HW_GPIO_PIN_27 )
#define UART3_RTS_PORT          ( HW_GPIO_PORT_0 )
#define UART3_RTS_PIN           ( HW_GPIO_PIN_28 )
#define UART3_CTS_PORT          ( HW_GPIO_PORT_1 )
#define UART3_CTS_PIN           ( HW_GPIO_PIN_29 )

#endif /* CONFIG_PERIPHERAL_SETUP_H_ */
