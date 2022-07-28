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
 ***************************************************************************************
 */

#include <ad_spi.h>
#include "peripheral_setup.h"
#include "platform_devices.h"

/*
 * PLATFORM PERIPHERALS GPIO CONFIGURATION
 *****************************************************************************************
 */

#if (dg_configSPI_ADAPTER == 1) || (dg_configUSE_HW_SPI == 1)
#if (MCP4921_MODULE_USED == 1)

/* SPI chip-select pins */
static const ad_io_conf_t cs_MCP4921[] = {{

        .port = MCP4921_CS_PORT,
        .pin  = MCP4921_CS_PIN,
        .on = {
                .mode     = HW_GPIO_MODE_OUTPUT,
                .function = HW_GPIO_FUNC_GPIO,
                .high     = true
        },
        .off = {
                .mode     = HW_GPIO_MODE_OUTPUT,
                .function = HW_GPIO_FUNC_GPIO,
                .high     = true
        }},
};

/* SPI1 IO */
static const ad_spi_io_conf_t bus_MCP4921 = {

        .spi_do = {
                .port = MCP4921_DO_PORT,
                .pin  = MCP4921_DO_PIN,
                .on   = {HW_GPIO_MODE_OUTPUT_PUSH_PULL, HW_GPIO_FUNC_SPI_DO, false},
                .off  = {HW_GPIO_MODE_INPUT,            HW_GPIO_FUNC_GPIO,   true},
        },
        .spi_di = { // Not required
                .port = MCP4921_DI_PORT,
                .pin  = MCP4921_DI_PIN,
                .on   = {HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_SPI_DI, false},
                .off  = {HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_GPIO,   true},
        },
        .spi_clk = {
                .port = MCP4921_CLK_PORT,
                .pin  = MCP4921_CLK_PIN,
                .on   = {HW_GPIO_MODE_OUTPUT_PUSH_PULL, HW_GPIO_FUNC_SPI_CLK, false},
                .off  = {HW_GPIO_MODE_INPUT,            HW_GPIO_FUNC_GPIO,    true},
        },

        /*
         * The number of pins in spi_master_cs array.
         *
         * \warning When the SPI bus is used by SNC \p cs_cnt must be always 1
         */
        .cs_cnt = 1,
        .spi_cs = cs_MCP4921,

        .voltage_level = HW_GPIO_POWER_V33
};

/* External sensor/module SPI driver */
static const ad_spi_driver_conf_t drv_MCP4921 = {
        .spi = {
                .cs_pad         = { MCP4921_CS_PORT, MCP4921_CS_PIN },
                .word_mode      = HW_SPI_WORD_16BIT,    /* Required by the module used */
                .smn_role       = HW_SPI_MODE_MASTER,
                .cpol_cpha_mode  = HW_SPI_CP_MODE_0,
                .mint_mode      = HW_SPI_MINT_ENABLE,
                .xtal_freq      = 16,    /* Operating freq. @ DIVN / xtal_freq = 4MHz */
                .fifo_mode      = HW_SPI_FIFO_RX_TX,
                .disabled       = 0,                    /* Should be disabled during initialization phase */
                .spi_cs         = HW_SPI_CS_GPIO,
                .rx_tl = HW_SPI_FIFO_LEVEL0,
                .tx_tl = HW_SPI_FIFO_LEVEL0,
                .swap_bytes = false,
                .select_divn = true,
                .use_dma = HW_DMA_CHANNEL_0 >= 0,
                .rx_dma_channel = HW_DMA_CHANNEL_0,
                .tx_dma_channel = HW_DMA_CHANNEL_0 + 1,
        }
};
/* Sensor/module device configuration */
const ad_spi_controller_conf_t dev_MCP4921 = {
        .id  = HW_SPI1,
        .io  = &bus_MCP4921,
        .drv = &drv_MCP4921,
};
spi_device MCP4921_DEVICE = &dev_MCP4921;

#endif /* MCP482_MODULE_USED */
#endif /* dg_configSPI_ADAPTER || dg_configUSE_HW_SPI */
