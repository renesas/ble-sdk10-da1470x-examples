/*
 * platform_devices.c
 *
 *  Created on: Aug 25, 2021
 *      Author: akleinen
 */

#include "ad_spi.h"
#include "ad_i2c.h"
#include "peripheral_setup.h"

#include "platform_devices.h"

/*
 * PLATFORM PERIPHERALS GPIO CONFIGURATION
 *****************************************************************************************
 */

#if dg_configI2C_ADAPTER || dg_configUSE_HW_I2C

/* I2C1 I/O configuration */
const ad_i2c_io_conf_t io_EEPROM_24C08 = {
        .scl = {
                .port = EEPROM_24C08_SCL_PORT, .pin = EEPROM_24C08_SCL_PIN,
                .on =  { HW_GPIO_MODE_OUTPUT_OPEN_DRAIN, HW_GPIO_FUNC_I2C_SCL, false },
                .off = { HW_GPIO_MODE_INPUT,             HW_GPIO_FUNC_GPIO,    true  }
        },
        .sda = {
                .port = EEPROM_24C08_SDA_PORT, .pin =EEPROM_24C08_SDA_PIN,
                .on =  { HW_GPIO_MODE_OUTPUT_OPEN_DRAIN, HW_GPIO_FUNC_I2C_SDA, false },
                .off = { HW_GPIO_MODE_INPUT,             HW_GPIO_FUNC_GPIO,    true  }
        },
        .voltage_level = HW_GPIO_POWER_V33
};

#endif /* dg_configI2C_ADAPTER || dg_configUSE_HW_I2C */


/*
 * PLATFORM PERIPHERALS CONTROLLER CONFIGURATION
 *****************************************************************************************
 */

#if dg_configI2C_ADAPTER || dg_configUSE_HW_I2C


/* EEPROM 24C08 I2C driver configuration */
const ad_i2c_driver_conf_t drv_EEPROM_24C08 = {
        I2C_DEFAULT_CLK_CFG,
        .i2c.speed              = HW_I2C_SPEED_STANDARD,
        .i2c.mode               = HW_I2C_MODE_MASTER,
        .i2c.addr_mode          = HW_I2C_ADDRESSING_7B,
        .i2c.address            = EEPROM_24C08_I2C_ADDRESS,
        .dma_channel            = HW_DMA_CHANNEL_2
};

/* EEPROM 24C08 I2C controller configuration */
const ad_i2c_controller_conf_t dev_24C08 = {
        .id     = HW_I2C1,
        .io     = &io_EEPROM_24C08,
        .drv    = &drv_EEPROM_24C08
};

i2c_device EEPROM_24C08 = &dev_24C08;

#endif /* dg_configI2C_ADAPTER || dg_configUSE_HW_I2C */
