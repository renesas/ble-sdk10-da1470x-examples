/**
 ****************************************************************************************
 *
 * @file hm80160a090.h
 *
 * @brief LCD configuration for HM80160A090 LCD with IC ST7735S
 *
 * Copyright (C) 2020-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef HM80160A090_H_
#define HM80160A090_H_

#include <stdint.h>
#include "platform_devices.h"

#if dg_configUSE_HM80160A090

#if dg_configLCDC_ADAPTER

/*********************************************************************
 *
 *       Defines
 *
 *********************************************************************
 */
#define GDI_DISP_COLOR          (HW_LCDC_OCM_8RGB565)
#define GDI_DISP_RESX           (80)
#define GDI_DISP_RESY           (160)
#define GDI_DISP_OFFSETX        (24)
#define GDI_DISP_OFFSETY        (0)
#define GDI_LCDC_CONFIG         (&hm80160a090_cfg)
#define GDI_USE_CONTINUOUS_MODE (0)

/*
 * static const ad_io_conf_t hm80160a090_gpio_cfg[] = {
 *       { HM80160A090_SCK_PORT, HM80160A090_SCK_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_CLK, true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { HM80160A090_SDA_PORT, HM80160A090_SDA_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DO,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { HM80160A090_RST_PORT, HM80160A090_RST_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { HM80160A090_DC_PORT,  HM80160A090_DC_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DC,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { HM80160A090_CS_PORT,  HM80160A090_CS_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_EN,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 * };

 * const ad_lcdc_io_conf_t hm80160a090_io = {
 *       .voltage_level = HW_GPIO_POWER_VDD1V8P,
 *       .io_cnt = sizeof(hm80160a090_gpio_cfg) / sizeof(hm80160a090_gpio_cfg[0]),
 *       .io_list = hm80160a090_gpio_cfg,
 * };
 */

static GDI_DRV_CONF_ATTR ad_lcdc_driver_conf_t hm80160a090_drv = {
        .hw_init.phy_type = HW_LCDC_PHY_MIPI_SPI4,
        .hw_init.format = GDI_DISP_COLOR,
        .hw_init.resx = GDI_DISP_RESX,
        .hw_init.resy = GDI_DISP_RESY,
        .hw_init.cfg_extra_flags = 0,
        .hw_init.mode = HW_LCDC_MODE_DISABLE,
        .hw_init.write_freq = LCDC_FREQ_16MHz,
        .ext_clk = HW_LCDC_EXT_CLK_OFF,
        .te_enable = false,
        .te_mode = HW_LCDC_TE_POL_LOW,
};

static const ad_lcdc_controller_conf_t hm80160a090_cfg = {
        .io = &hm80160a090_io,
        .drv = &hm80160a090_drv,
};

typedef enum {
        HM80160A090_PIXEL_FORMAT_RGB444 = 0x3,
        HM80160A090_PIXEL_FORMAT_RGB565 = 0x5,
        HM80160A090_PIXEL_FORMAT_RGB666 = 0x6,
} HM80160A090_PIXEL_FORMAT;

/* Required (and supported) output color modes */
static const HW_LCDC_OUTPUT_COLOR_MODE screen_color_modes[] = {
        HW_LCDC_OCM_8RGB444,
        HW_LCDC_OCM_8RGB565,
        HW_LCDC_OCM_8RGB666,
};

/* Callback function to configure display's color mode */
__STATIC_INLINE bool screen_set_color_mode(HW_LCDC_OUTPUT_COLOR_MODE color_mode)
{
        uint8_t value = 0;

        /* Exercise color mode */
        switch (color_mode) {
        case HW_LCDC_OCM_8RGB444:
                value = HM80160A090_PIXEL_FORMAT_RGB444;
                break;
        case HW_LCDC_OCM_8RGB565:
                value = HM80160A090_PIXEL_FORMAT_RGB565;
                break;
        case HW_LCDC_OCM_8RGB666:
                value = HM80160A090_PIXEL_FORMAT_RGB666;
                break;
        default:
                return false; // Unsupported color mode
        }
        hw_lcdc_dcs_cmd_params(HW_LCDC_MIPI_DCS_SET_PIXEL_FORMAT, &value, sizeof(value));

        return true;
}

static const uint8_t screen_init_cmds[] = {
        /* Power on */
        LCDC_GPIO_SET_INACTIVE(HM80160A090_RST_PORT, HM80160A090_RST_PIN),
        LCDC_DELAY_MS(100),
        LCDC_GPIO_SET_ACTIVE(HM80160A090_RST_PORT, HM80160A090_RST_PIN),
        LCDC_DELAY_MS(100),

        LCDC_MIPI_SW_RST(),
        LCDC_DELAY_MS(100),

        //ST7735R Frame Rate
        LCDC_MIPI_CMD_DATA(0xB1, 0x01, 0x2C, 0x2D),
        LCDC_MIPI_CMD_DATA(0xB2, 0x01, 0x2C, 0x2D),
        LCDC_MIPI_CMD_DATA(0xB3, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D),
        //Column inversion
        LCDC_MIPI_CMD_DATA(0xB4, 0x07),

        //ST7735R Power Sequence
        LCDC_MIPI_CMD_DATA(0xC0, 0xA2, 0x02, 0x84),
        LCDC_MIPI_CMD_DATA(0xC1, 0xC5),
        LCDC_MIPI_CMD_DATA(0xC2, 0x0A, 0x00),
        LCDC_MIPI_CMD_DATA(0xC3, 0x8A, 0x2A),
        LCDC_MIPI_CMD_DATA(0xC4, 0x8A, 0xEE),
        LCDC_MIPI_CMD_DATA(0xC5, 0x0E),

        LCDC_MIPI_SET_ADDR_MODE(0x04),

        //ST7735R Gamma Sequence
        LCDC_MIPI_CMD_DATA(0xe0, 0x0f, 0x1A, 0x0f, 0x18, 0x2f, 0x28, 0x20, 0x22, 0x1f, 0x1b, 0x23,
                0x37, 0x00, 0x07, 0x02, 0x10),

        LCDC_MIPI_CMD_DATA(0xe1, 0x0f, 0x1b, 0x0f, 0x17, 0x33, 0x2C, 0x29, 0x2e, 0x30, 0x30, 0x39,
                0x3f, 0x00, 0x07, 0x03, 0x10),

        LCDC_MIPI_SET_POSITION(GDI_DISP_OFFSETX, GDI_DISP_OFFSETY,
                GDI_DISP_RESX + GDI_DISP_OFFSETX - 1, GDI_DISP_RESY + GDI_DISP_OFFSETY - 1),

        //Enable test command
        LCDC_MIPI_CMD_DATA(0xf0, 0x01),

        //Disable ram power save mode
        LCDC_MIPI_CMD_DATA(0xf6, 0x00),

        LCDC_MIPI_SET_MODE(GDI_DISP_COLOR == HW_LCDC_OCM_8RGB444 ? HM80160A090_PIXEL_FORMAT_RGB444 :
                           GDI_DISP_COLOR == HW_LCDC_OCM_8RGB565 ? HM80160A090_PIXEL_FORMAT_RGB565 :
                                                                   HM80160A090_PIXEL_FORMAT_RGB666),
};

static const uint8_t screen_power_on_cmds[] = {
};

static const uint8_t screen_enable_cmds[] = {
        LCDC_MIPI_CMD_DATA(HW_LCDC_MIPI_DCS_EXIT_SLEEP_MODE),
        LCDC_DELAY_MS(120),
        LCDC_MIPI_CMD_DATA(HW_LCDC_MIPI_DCS_SET_DISPLAY_ON),
        LCDC_DELAY_MS(120),
};

static const uint8_t screen_disable_cmds[] = {
        LCDC_MIPI_CMD_DATA(HW_LCDC_MIPI_DCS_SET_DISPLAY_OFF),
        LCDC_DELAY_MS(120),
        LCDC_MIPI_CMD_DATA(HW_LCDC_MIPI_DCS_ENTER_SLEEP_MODE),
        LCDC_DELAY_MS(120),
};

static const uint8_t screen_power_off_cmds[] = {
};


static const uint8_t screen_clear_cmds[] = {
};

__STATIC_INLINE void screen_set_partial_update_area(hw_lcdc_frame_t *frame)
{
        UNUSED_ARG(frame); //! Just to suppress compiler warnings
}

#endif /* dg_configLCDC_ADAPTER */

#endif /* dg_configUSE_HM80160A090 */

#endif /* HM80160A090_H_ */
