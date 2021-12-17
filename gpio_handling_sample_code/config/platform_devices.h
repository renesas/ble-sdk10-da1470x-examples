/**
 ****************************************************************************************
 *
 * @file platform_devices.h
 *
 * @brief Configuration of devices connected to board
 *
 * Copyright (C) 2017-2021 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef PLATFORM_DEVICES_H_
#define PLATFORM_DEVICES_H_

#include "ad.h"

/* LED configuration section */
#define LED1_PORT       (HW_GPIO_PORT_0)
#define LED1_PIN        (HW_GPIO_PIN_22)
#define LED1_MODE       (HW_GPIO_MODE_OUTPUT)
#define LED1_FUNC       (HW_GPIO_FUNC_GPIO)

/*
 * Pins can be grouped according to their functionalities (e.g. input, output). This is
 * convenient in cases where latch/unlatch functionality is not performed on all pins.
 */
/* Input GPIO pins configuration array */
__UNUSED static const ad_io_conf_t input_gpio_cfg[] = {
        {
                 .port =  KEY1_PORT,
                 .pin = KEY1_PIN,
                 .on = {
                     .mode = HW_GPIO_MODE_INPUT_PULLUP,
                     .function = HW_GPIO_FUNC_GPIO
                  },
                  .off = {
                      .mode = HW_GPIO_MODE_INPUT_PULLUP,
                      .function = HW_GPIO_FUNC_GPIO
                  },
            }
/*TODO: demonstrate KEY usage*/
};

/*
 * Output GPIO pins configuration array.
 */
static const ad_io_conf_t output_gpio_cfg[] = {
    {
         .port =  LED1_PORT,
         .pin = LED1_PIN,
         .on = {
             .mode = HW_GPIO_MODE_OUTPUT,
             .function = HW_GPIO_FUNC_GPIO,
             .high = true
          },
          .off = {
              .mode = HW_GPIO_MODE_OUTPUT,
              .function = HW_GPIO_FUNC_GPIO
          },
    }
};

/* Miscellaneous pins configuration array (e.g. PWM pins) */
__UNUSED static const ad_io_conf_t miscellaneous_gpio_cfg[] = {
};

#endif /* PLATFORM_DEVICES_H_ */

