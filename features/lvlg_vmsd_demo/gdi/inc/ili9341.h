/**
 ****************************************************************************************
 *
 * @file
 *
 * @brief
 *
 * Copyright (C) 2020-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef ILI9341_H_
#define ILI9341_H_

#include "stdint.h"
#include "platform_devices.h"

#if dg_configUSE_ILI9341

#define GDI_DISP_RESX           (240)
#define GDI_DISP_RESY           (320)
#define GDI_DISP_COLOR          (HW_LCDC_OCM_8RGB565)
#define GDI_DISP_OFFSETX        (0)
#define GDI_DISP_OFFSETY        (0)
#define GDI_LCDC_CONFIG         (&ili9341_cfg)
#define GDI_USE_CONTINUOUS_MODE (0)

/*
 * static const ad_io_conf_t ili9341_gpio_cfg[] = {
 *         { ILI9341_SCK_PORT, ILI9341_SCK_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_CLK, true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *         { ILI9341_SDA_PORT, ILI9341_SDA_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DO,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *         { ILI9341_RST_PORT, ILI9341_RST_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *         { ILI9341_DC_PORT,  ILI9341_DC_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DC,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *         { ILI9341_CS_PORT,  ILI9341_CS_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_EN,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 * };
 *
 * const ad_lcdc_io_conf_t ili9341_io = {
 *         .voltage_level = HW_GPIO_POWER_VDD1V8P,
 *         .io_cnt = sizeof(ili9341_gpio_cfg) / sizeof(ili9341_gpio_cfg[0]),
 *         .io_list = ili9341_gpio_cfg,
 * };
 */

static GDI_DRV_CONF_ATTR ad_lcdc_driver_conf_t ili9341_drv = {
        .hw_init.phy_type = HW_LCDC_PHY_MIPI_SPI4,
        .hw_init.format = GDI_DISP_COLOR,
        .hw_init.resx = GDI_DISP_RESX,
        .hw_init.resy = GDI_DISP_RESY,
        .hw_init.cfg_extra_flags = 0,
        .hw_init.mode = HW_LCDC_MODE_DISABLE,
        .hw_init.write_freq = LCDC_FREQ_48MHz,
        .ext_clk = HW_LCDC_EXT_CLK_OFF,
        .te_enable = false,
        .te_mode = HW_LCDC_TE_POL_LOW,
};

static const ad_lcdc_controller_conf_t ili9341_cfg = {
        .io = &ili9341_io,
        .drv = &ili9341_drv,
};

typedef enum {
        ILI9341_PIXEL_FORMAT_RGB565  = 0x55,
        ILI9341_PIXEL_FORMAT_RG666   = 0x66
} ILI9341_PIXEL_FORMAT;

/* Required (and supported) output color modes */
static const HW_LCDC_OUTPUT_COLOR_MODE screen_color_modes[] = {
        HW_LCDC_OCM_8RGB565,
        HW_LCDC_OCM_8RGB666,
};

__STATIC_INLINE bool screen_set_color_mode(HW_LCDC_OUTPUT_COLOR_MODE color_mode)
{
        uint8_t value = 0;

        /* Exercise color mode */
        switch (color_mode) {
        case HW_LCDC_OCM_8RGB565:
                value = ILI9341_PIXEL_FORMAT_RGB565;
                break;
        case HW_LCDC_OCM_8RGB666:
                value = ILI9341_PIXEL_FORMAT_RG666;
                break;
        default:
                return false; // Unsupported color mode
        }

        hw_lcdc_mipi_cmd(HW_LCDC_MIPI_CMD, HW_LCDC_MIPI_DCS_SET_PIXEL_FORMAT);
        hw_lcdc_mipi_cmd(HW_LCDC_MIPI_DATA, value);

        return true;
}

static const uint8_t screen_init_cmds[] = {
        /* Power on */
        LCDC_GPIO_SET_INACTIVE(ILI9341_RST_PORT, ILI9341_RST_PIN),
        LCDC_DELAY_MS(20),
        LCDC_GPIO_SET_ACTIVE(ILI9341_RST_PORT, ILI9341_RST_PIN),
        LCDC_DELAY_MS(20),

        LCDC_MIPI_SW_RST(),
        LCDC_DELAY_MS(100),

        LCDC_MIPI_CMD_DATA(HW_LCDC_MIPI_DCS_EXIT_SLEEP_MODE),
        LCDC_DELAY_MS(120),

        LCDC_MIPI_CMD_DATA(0xCF, 0x00, 0xD9, 0x30),
        LCDC_MIPI_CMD_DATA(0xED, 0x64, 0x03, 0x12, 0x81),
        LCDC_MIPI_CMD_DATA(0xE8, 0x85, 0x10, 0x78),
        LCDC_MIPI_CMD_DATA(0xCB, 0x39, 0x2C, 0x00, 0x34, 0x02),

        LCDC_MIPI_CMD_DATA(0xF7, 0x20),
        LCDC_MIPI_CMD_DATA(0xEA, 0x00, 0x00),

        LCDC_MIPI_CMD_DATA(0xC0, 0x21),
        LCDC_MIPI_CMD_DATA(0xC1, 0x12),

        LCDC_MIPI_CMD_DATA(0xC5, 0x32, 0x3C),

        LCDC_MIPI_CMD_DATA(0xC7, 0xC1),

        LCDC_MIPI_CMD_DATA(0x36, 0x08),

        LCDC_MIPI_CMD_DATA(0x3A, 0x55),

        LCDC_MIPI_CMD_DATA(0xB1, 0x00, 0x10/*0x18*/),
        LCDC_MIPI_CMD_DATA(0xB2, 0x00, 0x10/*0x18*/),//lu added

        LCDC_MIPI_CMD_DATA(0xB6, 0x0A, 0xA2),

        LCDC_MIPI_CMD_DATA(0xF2, 0x00),

        LCDC_MIPI_CMD_DATA(0x26, 0x01),

        LCDC_MIPI_CMD_DATA(0xE0, 0x0F, 0x20, 0x1E, 0x09, 0x12, 0x0B, 0x50, 0xBA, 0x44, 0x09, 0x14, 0x05, 0x23, 0x21, 0x00),

        LCDC_MIPI_CMD_DATA(0xE1, 0x00, 0x19, 0x19, 0x00, 0x12, 0x07, 0x2D, 0x28, 0x3F, 0x02, 0x0A, 0x08, 0x25, 0x2D, 0x0F),


        //LCDC_MIPI_SET_POSITION(0x18, 0, GDI_DISP_RESX - 1 + 0x18, GDI_DISP_RESY - 1),
        LCDC_MIPI_SET_POSITION(GDI_DISP_OFFSETX, GDI_DISP_OFFSETY,
                GDI_DISP_RESX + GDI_DISP_OFFSETX - 1, GDI_DISP_RESY + GDI_DISP_OFFSETY - 1),
        //Enable test command
        LCDC_MIPI_CMD_DATA(0xf0, 0x01),
        //Disable ram power save mode
        LCDC_MIPI_CMD_DATA(0xf6, 0x00),

        LCDC_MIPI_SET_MODE(GDI_DISP_COLOR == HW_LCDC_OCM_8RGB565 ? ILI9341_PIXEL_FORMAT_RGB565 :
                                                                   ILI9341_PIXEL_FORMAT_RG666),

        //TEON, Tearing effect line on
        //TELOM=0x1, V-blanking +H-blanking
        LCDC_MIPI_CMD_DATA(0x35, 0x00),
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
        UNUSED_ARG(frame);
}

#endif /* dg_configLCDC_ADAPTER */

#endif /* ILI9341_H_ */
