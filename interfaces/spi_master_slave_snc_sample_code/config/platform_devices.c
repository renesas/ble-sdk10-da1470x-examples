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
#include "ad_spi.h"
#include "platform_devices.h"
#include "peripheral_setup.h"

#if (dg_configSPI_ADAPTER && dg_configUSE_HW_SPI)

static __CONST ad_io_conf_t cs_spi_master[] = {{
        .port = SPI_MASTER_CS_PORT,
        .pin  = SPI_MASTER_CS_PIN,
        .on   = { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_SPI_EN, true },
        .off  = { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_SPI_EN, true }},
};

/*
 * Structure that defines the I/O signal mapping upon employing an SPI block instance.
 * The 'on' state is applied when a device is opened whilst the 'off' state when the
 * device is closed.
 */
static __CONST ad_spi_io_conf_t io_spi_master = {
        .spi_do = {
               .port = SPI_MASTER_DO_PORT,
               .pin  = SPI_MASTER_DO_PIN,
               .on   = { HW_GPIO_MODE_OUTPUT_PUSH_PULL, HW_GPIO_FUNC_SPI_DO, false },
               .off  = { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true },
        },
        .spi_clk = {
                .port = SPI_MASTER_CLK_PORT,
                .pin  = SPI_MASTER_CLK_PIN,
                .on   = { HW_GPIO_MODE_OUTPUT_PUSH_PULL, HW_GPIO_FUNC_SPI_CLK, false },
                .off  = { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true },
        },
        .spi_di = {
                .port = SPI_MASTER_DI_PORT,
                .pin  = SPI_MASTER_DI_PIN,
                .on   = { HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_SPI_DI, false },
                .off  = { HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_GPIO, false },
        },

        .spi_cs = cs_spi_master,
        .cs_cnt = ARRAY_LENGTH(cs_spi_master),

        .voltage_level = HW_GPIO_POWER_VDD1V8P,
};

/* Structure that defines how an SPI block instance should be driven */
static __CONST ad_spi_driver_conf_t drv_spi_master = {
        .spi = {
                .cs_pad         = { SPI_MASTER_CS_PORT, SPI_MASTER_CS_PIN },

                .word_mode      = HW_SPI_WORD_8BIT,
                .smn_role       = HW_SPI_MODE_MASTER,
                .cpol_cpha_mode = HW_SPI_CP_MODE_0,
                /*
                 * If set the SPI block instance will be driven in interrupt-driven mode (default mode).
                 *
                 * \note If not set the device will still be driven in interrupt-driven mode.
                 * \note If the DMA-driven mode is enabled as well it will take precedence
                 *       over the interrupt-driven mode.
                 */
                .mint_mode      = HW_SPI_MINT_DISABLE,

                /*
                 * Clock divider to be applied to the clock source of the SPI block instance.
                 * User can use the following formula to compute the actual output frequency:
                 *
                 * output clock frequency = input clock source / 2 * (clock divider + 1).
                 *
                 * \note SPI1 and SPI2 can operate @32MHz max. frequency whilst the SPI3 block
                 *       can operate @24MHz max. frequency.
                 *
                 * \note A clock divider set to 0x7f is interpreted as "no clock divider
                 *       that is, the operating frequency will match the clock source.
                 *
                 */
                .xtal_freq      = 0,

                /* Do not care, should be handled internally */
                .fifo_mode      = HW_SPI_FIFO_NONE,

                /*
                 * The master mode supports three CS modes namely CS0, CS1 and GPIO.
                 * Make sure that the selected CS mode matches the one declared in
                 * the I/O structure definition.
                 *
                 * Following are demonstrated all three possible combinations:
                 *
                 * .spi_cs = HW_SPI_CS_0 <==> .on = { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_SPI_EN, true }
                 *
                 * .spi_cs = HW_SPI_CS_1 <==> .on = { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_SPI_EN2, true }
                 *
                 * .spi_cs = HW_SPI_CS_GPIO <==> .on = { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }
                 *
                 * The slave role supports only the CS0 mode.
                 */
                .spi_cs         = HW_SPI_CS_0,

                /* Do not care, should be handled internally */
                .disabled       = false,

                .rx_tl          = HW_SPI_FIFO_LEVEL0,
                .tx_tl          = HW_SPI_FIFO_LEVEL0,

                /*
                 *  Select a clock source. An SPI block can be clocked either by the XTAL32MHz crystal
                 *  directly (also referred to as DIVN path) or be clocked by the current system clock.
                 *  (also referred to as DIV1 path)
                 *
                 *  \note If the DIV1 path is selected then any attempt to switch the system clock with
                 *        the help of cm_cpu_clk_set() will fail for as long as the target SPI block is
                 *        enabled. This is done in order to avoid the SPI frequency being changed
                 *        mid-transfer.
                 */
                .select_divn    = true,

                /*
                 * If set, bytes in the Tx/Rx FIFOs are swapped. This feature is valid for
                 * 16- and 32-bit bus widths.
                 */
                .swap_bytes     = true,

#if (MAIN_PROCESSOR_BUILD)
                /*
                 * If set, the SPI will operate in DMA-driven mode bypassing the interrupt-driven mode.
                 * The DMA engine is powered by the PD_SYS power domain and so it's not allowed to be
                 * accessed by the SNC which is powered by PD_SNC.
                 */
                .use_dma        = true,

                /*
                 * By architecture, the RX channel should be assigned an even number.
                 * Valid Rx/Tx channel pairs are:
                 *
                 * .rx_dma_channel = HW_DMA_CHANNEL_0,
                 * .tx_dma_channel = HW_DMA_CHANNEL_1,
                 *
                 * .rx_dma_channel = HW_DMA_CHANNEL_2,
                 * .tx_dma_channel = HW_DMA_CHANNEL_3,
                 *
                 * .rx_dma_channel = HW_DMA_CHANNEL_4,
                 * .tx_dma_channel = HW_DMA_CHANNEL_5,
                 *
                 * .rx_dma_channel = HW_DMA_CHANNEL_6,
                 * .tx_dma_channel = HW_DMA_CHANNEL_7 ,
                 *
                 * \note If the secure boot is enabled the DMA channel 7 is reserved explicitly
                 *       for security operations and so, it cannot be used by the application.
                 */
                .rx_dma_channel = HW_DMA_CHANNEL_0,
                .tx_dma_channel = HW_DMA_CHANNEL_1,
#endif
        }
};

/*
 * Structure that binds an SPI block instance with a driver and I/O definitions;
 * eventually composing a device.
 *
 * \note The DA1470x integrates three SPI block instances with the SPI3 being powered by
 *       the PD_SYS power domain. Therefore, it's not allowed to be accessed by the SNC
 *       which is powered by PD_SNC.
 */
__CONST ad_spi_controller_conf_t dev_spi_master = {
        .id  = HW_SPI1,
        .io  = &io_spi_master,
        .drv = &drv_spi_master,
};

static __CONST ad_io_conf_t cs_spi_slave[] = {{
        .port = SPI_SLAVE_CS_PORT,
        .pin  = SPI_SLAVE_CS_PIN,
        .on   = { HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_SPI2_EN /* In slave mode only the CS0 mode is valid */, false },
        .off  = { HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_SPI2_EN, false }},
};

static __CONST ad_spi_io_conf_t io_spi_slave = {
        .spi_do = {
               .port = SPI_SLAVE_DO_PORT,
               .pin  = SPI_SLAVE_DO_PIN,
               .on   = { HW_GPIO_MODE_OUTPUT_PUSH_PULL, HW_GPIO_FUNC_SPI2_DO, false },
               .off  = { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true },
        },
        .spi_clk = {
                .port = SPI_SLAVE_CLK_PORT,
                .pin  = SPI_SLAVE_CLK_PIN,
                .on   = { HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_SPI2_CLK, false },
                .off  = { HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_GPIO, false },
        },
        .spi_di = {
                .port = SPI_SLAVE_DI_PORT,
                .pin  = SPI_SLAVE_DI_PIN,
                .on   = { HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_SPI2_DI, false },
                .off  = { HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_GPIO, false },
        },

        .spi_cs = cs_spi_slave,
        .cs_cnt = ARRAY_LENGTH(cs_spi_slave),

        .voltage_level = HW_GPIO_POWER_VDD1V8P,
};

static __CONST ad_spi_driver_conf_t drv_spi_slave = {
        .spi = {
                .cs_pad         = { SPI_SLAVE_CS_PORT, SPI_SLAVE_CS_PIN },

                .word_mode      = HW_SPI_WORD_8BIT,
                .smn_role       = HW_SPI_MODE_SLAVE,
                .cpol_cpha_mode = HW_SPI_CP_MODE_0,
                .mint_mode      = HW_SPI_MINT_DISABLE,

                .xtal_freq      = 0,
                .fifo_mode      = HW_SPI_FIFO_NONE,
                .spi_cs         = HW_SPI_CS_0,

                .disabled       = false,

                .rx_tl          = HW_SPI_FIFO_LEVEL0,
                .tx_tl          = HW_SPI_FIFO_LEVEL0,

                .select_divn    = true,
                .swap_bytes     = true,

#if (MAIN_PROCESSOR_BUILD)
                .use_dma        = true,
                .rx_dma_channel = HW_DMA_CHANNEL_2,
                .tx_dma_channel = HW_DMA_CHANNEL_3,
#endif
        }
};

__CONST ad_spi_controller_conf_t dev_spi_slave = {
        .id  = HW_SPI2,
        .io  = &io_spi_slave,
        .drv = &drv_spi_slave,
};

/* Abstractive device definitions */
PERIPHERAL_DEVICE SPI_DEVICE_MASTER = &dev_spi_master;
PERIPHERAL_DEVICE SPI_DEVICE_SLAVE = &dev_spi_slave;

#endif
