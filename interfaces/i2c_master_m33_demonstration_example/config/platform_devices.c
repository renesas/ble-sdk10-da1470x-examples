/**
 ****************************************************************************************
 *
 * @file platform_devices.h
 *
 * @brief Source file that includes all the peripheral devices structures
 *
 * Copyright (C) 2020-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include "hw_gpio.h"
#include "ad_i2c.h"
#include "platform_devices.h"
#include "peripheral_setup.h"

#if (dg_configI2C_ADAPTER && dg_configUSE_HW_I2C)
/*
 * Structure that defines the I/O signal mapping upon employing an I2C block instance.
 * The 'on' state is applied when a device is opened whilst the 'off' state when the
 * device is closed.
 */
static __CONST ad_i2c_io_conf_t io_i2c_master = {
        .scl = {
               .port = I2C_MASTER_SCL_PORT,
               .pin  = I2C_MASTER_SCL_PIN,
               .on   = { HW_GPIO_MODE_OUTPUT_OPEN_DRAIN, HW_GPIO_FUNC_I2C_SCL, false },
               .off  = { HW_GPIO_MODE_INPUT,             HW_GPIO_FUNC_GPIO,    false },
        },
        .sda = {
                .port = I2C_MASTER_SDA_PORT,
                .pin  = I2C_MASTER_SDA_PIN,
                .on   = { HW_GPIO_MODE_OUTPUT_OPEN_DRAIN, HW_GPIO_FUNC_I2C_SDA, false },
                .off  = { HW_GPIO_MODE_INPUT,             HW_GPIO_FUNC_GPIO,    false },
        },

        .voltage_level = HW_GPIO_POWER_VDD1V8P,
};

/* Structure that defines how an I2C block instance should be driven */
static __CONST ad_i2c_driver_conf_t drv_i2c_master = {
        .i2c = {
                I2C_DEFAULT_CLK_CFG,
                .speed       = HW_I2C_SPEED_STANDARD,
                .mode        = HW_I2C_MODE_MASTER,
                .addr_mode   = HW_I2C_ADDRESSING_7B,
                .address     = I2C_SLAVE_ADDRESS,
        },

#if (MAIN_PROCESSOR_BUILD)
                /*
                 * The DMA engine is powered by the PD_SYS power domain and so it's not allowed to be
                 * accessed by the SNC which is powered by PD_SNC.
                 *
                 * \note If the secure boot is enabled the DMA channel 7 is reserved explicitly for
                 *       security operations and so, it cannot be used by the application.
                 */
                .dma_channel = HW_DMA_CHANNEL_INVALID,
#endif
};

/*
 * Structure that binds an SPI block instance with a driver and I/O definitions;
 * eventually composing a device.
 */
__CONST ad_i2c_controller_conf_t dev_i2c_master = {
        .id  = HW_I2C1,
        .io  = &io_i2c_master,
        .drv = &drv_i2c_master,
};

/* Abstractive device definitions */
PERIPHERAL_DEVICE I2C_DEVICE_MASTER = &dev_i2c_master;

#endif /* dg_configI2C_ADAPTER && dg_configUSE_HW_I2C */
