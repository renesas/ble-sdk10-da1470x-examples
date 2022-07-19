/**
 ****************************************************************************************
 *
 * @file platform_devices.c
 *
 * @brief Configuration of devices connected to board data structures
 *
 * Copyright (c) 2022 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor. By using this Software
 * you agree that Dialog Semiconductor retains all intellectual property and proprietary
 * rights in and to this Software and any use, reproduction, disclosure or distribution
 * of the Software without express written permission or a license agreement from Dialog
 * Semiconductor is strictly prohibited. This Software is solely for use on or in
 * conjunction with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR AS
 * REQUIRED BY LAW, THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE PROVIDED
 * IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR BY LAW, IN NO EVENT SHALL DIALOG
 * SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE.
 *
 ****************************************************************************************
 */
#include <ad_i2c.h>
#include <ad_i3c.h>
#include "peripheral_setup.h"
#include "platform_devices.h"

/*
 * PLATFORM PERIPHERALS GPIO CONFIGURATION
 *****************************************************************************************
 */

#if defined(dg_configUseI2CHandling)

/* I2C I/O configuration */
const ad_i2c_io_conf_t lsm6dsox_io = {
        .scl = {
                .port = I2C_PORT, .pin = I2C_MASTER_SCL_PIN,
                .on =  { HW_GPIO_MODE_OUTPUT_OPEN_DRAIN, HW_GPIO_FUNC_I2C_SCL, false },
                .off = { HW_GPIO_MODE_INPUT,             HW_GPIO_FUNC_GPIO,    true  }
        },
        .sda = {
                .port = I2C_PORT, .pin = I2C_MASTER_SDA_PIN,
                .on =  { HW_GPIO_MODE_OUTPUT_OPEN_DRAIN, HW_GPIO_FUNC_I2C_SDA, false },
                .off = { HW_GPIO_MODE_INPUT,             HW_GPIO_FUNC_GPIO,    true  }
        },
        .voltage_level = I2C_GPIO_LEVEL
};
/*
 * PLATFORM PERIPHERALS CONTROLLER CONFIGURATION
 *****************************************************************************************
 */

const ad_i2c_driver_conf_t lsm6dsox_driver_config = {
                I2C_DEFAULT_CLK_CFG,
                .i2c.speed = HW_I2C_SPEED_STANDARD,
                .i2c.mode = HW_I2C_MODE_MASTER,
                .i2c.addr_mode = HW_I2C_ADDRESSING_7B,
                .i2c.address = LSM6DSOX_ADDRESS,
                //.i2c.event_cb = NULL,
                .dma_channel = HW_DMA_CHANNEL_INVALID
                /**
                 * I2C master is not configured to use DMA because in case we are connecting the
                 * same board in loopback, the DMA controller will block in case there are blocking
                 * transactions being handled from both the I2C master and slave controller at the same time.
                 *
                 * In case we are connecting two boards we can use DMA for the master as well.
                 */
};


/* I2C controller configuration */
const ad_i2c_controller_conf_t lsm6dsox_config = {
        .id = I2C_AD_CONFIG_MASTER_I2C_CTRL,
        .io = &lsm6dsox_io,
        .drv = &lsm6dsox_driver_config
};
i2c_device LSM6DSOX_DEVICE = &lsm6dsox_config;
#elif defined(dg_configUseI3CHandling)
#define I3C_PURE_CLK_CFG .i3c.i3c_scl_cfg = {0x28, 0x28, 0x10, 0x10, 0x7, 0x6, 0x5, 0x1B, 0x7, 0x8, 0xB, 0x12}
#define I3C_MIXED_CLK_CFG .i3c.i3c_scl_cfg = {0xC8,0xC8,0x50,0x50,0x07,0x06,0x05,0x20,0x0D,0x14,0x21,0x49}
/* I2C I/O configuration */
const ad_i3c_io_conf_t lsm6dsox_io = {
        .scl = {
                .port = I3C_PORT, .pin = I3C_MASTER_SCL_PIN,
                .on =  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_I3C_SCL, false },
                .off = { HW_GPIO_MODE_INPUT_PULLUP,HW_GPIO_FUNC_GPIO,false}
        },
        .sda = {
                .port = I3C_PORT, .pin = I3C_MASTER_SDA_PIN,
                .on =  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_I3C_SDA, false },
                .off = { HW_GPIO_MODE_INPUT_PULLUP, HW_GPIO_FUNC_GPIO,false }
        },
        .voltage_level = I3C_GPIO_LEVEL
};
/*
 * PLATFORM PERIPHERALS CONTROLLER CONFIGURATION
 *****************************************************************************************
 */

const ad_i3c_driver_conf_t lsm6dsox_driver_config = {
                .i3c.select_divn = 0,
                 I3C_PURE_CLK_CFG,
                .i3c.i3c_dat_cfg[0]={HW_I3C_SLAVE_DEVICE_LEGACY_I2C,LSM6DSOX_ADDRESS,LSM6DSOX_ADDRESS_DYNAMIC},
                .i3c.hot_join_accept = 0,
                .i3c.iba = 0,

};


/* I2C controller configuration */
const ad_i3c_controller_conf_t lsm6dsox_config = {
        .id = I3C_AD_CONFIG_MASTER_I3C_CTRL,
        .io = &lsm6dsox_io,
        .drv = &lsm6dsox_driver_config
};
i3c_device LSM6DSOX_DEVICE = &lsm6dsox_config;

#endif
