/**
 ****************************************************************************************
 *
 * @file dt280qv10ct.h
 *
 * @brief LCD configuration for DT280QV10CT LCD with IC ILI9341
 *
 * Copyright (C) 2020-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#ifndef DT280QV10CT_H_
#define DT280QV10CT_H_

#include <stdio.h>
#include <stdint.h>
#include "platform_devices.h"
#include "ad_i2c.h"

#if dg_configUSE_DT280QV10CT

#if dg_configLCDC_ADAPTER

#define DT280QV10CT_LANDSCAPE           (0)

/*********************************************************************
 *
 *       Defines
 *
 *********************************************************************
 */
#define GDI_DISP_COLOR                  (HW_LCDC_OCM_8RGB666)
#if DT280QV10CT_LANDSCAPE
#define GDI_DISP_RESX                   (320)
#define GDI_DISP_RESY                   (240)
#else
#define GDI_DISP_RESX                   (240)
#define GDI_DISP_RESY                   (320)
#endif
#define GDI_DISP_OFFSETX                (0)
#define GDI_DISP_OFFSETY                (0)
#define GDI_LCDC_CONFIG                 (&dt280qv10ct_cfg)
#define GDI_USE_CONTINUOUS_MODE         (0)

/*
 * static const ad_io_conf_t dt280qv10ct_gpio_cfg[] = {
 *        { DT280QV10CT_SCK_PORT, DT280QV10CT_SCK_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_CLK, true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *        { DT280QV10CT_SDA_PORT, DT280QV10CT_SDA_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DO,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *        { DT280QV10CT_SI_PORT,  DT280QV10CT_SI_PIN,  { HW_GPIO_MODE_INPUT,  HW_GPIO_FUNC_LCD,         true }, { HW_GPIO_MODE_INPUT,  HW_GPIO_FUNC_GPIO, true }},
 *        { DT280QV10CT_RST_PORT, DT280QV10CT_RST_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *        { DT280QV10CT_DC_PORT,  DT280QV10CT_DC_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DC,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *        { DT280QV10CT_CS_PORT,  DT280QV10CT_CS_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_EN,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *        { DT280QV10CT_TE_PORT,  DT280QV10CT_TE_PIN,  { HW_GPIO_MODE_INPUT,  HW_GPIO_FUNC_LCD,         true }, { HW_GPIO_MODE_INPUT,  HW_GPIO_FUNC_GPIO, true }},
 *        { DT280QV10CT_BL_PORT,  DT280QV10CT_BL_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        false}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false}},
 * };

 * const ad_lcdc_io_conf_t dt280qv10ct_io = {
 *       .voltage_level = HW_GPIO_POWER_V33,
 *       .io_cnt = sizeof(dt280qv10ct_gpio_cfg) / sizeof(dt280qv10ct_gpio_cfg[0]),
 *       .io_list = dt280qv10ct_gpio_cfg,
 * };
 */

static GDI_DRV_CONF_ATTR ad_lcdc_driver_conf_t dt280qv10ct_drv = {
        .hw_init.phy_type = HW_LCDC_PHY_MIPI_SPI4,
        .hw_init.format = GDI_DISP_COLOR,
        .hw_init.resx = GDI_DISP_RESX,
        .hw_init.resy = GDI_DISP_RESY,
        .hw_init.cfg_extra_flags = 0,
        .hw_init.mode = HW_LCDC_MODE_DISABLE,
        .hw_init.write_freq = LCDC_FREQ_16MHz,   //! Max. @10MHz
        .hw_init.read_freq = LCDC_FREQ_4MHz,    //! Max. @6.7MHz
        .ext_clk = HW_LCDC_EXT_CLK_OFF,
        .te_enable = false,
        .te_mode = HW_LCDC_TE_POL_LOW,
};

static const ad_lcdc_controller_conf_t dt280qv10ct_cfg = {
        .io = &dt280qv10ct_io,
        .drv = &dt280qv10ct_drv,
};

typedef enum {
        DT280QV10CT_PIXEL_FORMAT_RGB565  = 0x55,
        DT280QV10CT_PIXEL_FORMAT_RGB666  = 0x66
} DT280QV10CT_PIXEL_FORMAT;

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
                value = DT280QV10CT_PIXEL_FORMAT_RGB565;
                break;
        case HW_LCDC_OCM_8RGB666:
                value = DT280QV10CT_PIXEL_FORMAT_RGB666;
                break;
        default:
                return false; //! Unsupported color mode
        }
        hw_lcdc_dcs_cmd_params(HW_LCDC_MIPI_DCS_SET_PIXEL_FORMAT, &value, sizeof(value));

        return true;
}

static const uint8_t screen_init_cmds[] = {
        /* Power on */
        LCDC_GPIO_SET_INACTIVE(DT280QV10CT_RST_PORT, DT280QV10CT_RST_PIN),
        LCDC_DELAY_MS(10),
        LCDC_GPIO_SET_ACTIVE(DT280QV10CT_RST_PORT, DT280QV10CT_RST_PIN),
        LCDC_DELAY_MS(120),

        LCDC_MIPI_SW_RST(),
        LCDC_DELAY_MS(100),

        LCDC_MIPI_CMD_DATA(0xC0, 0x23),              // POWERCONTROL1 - GVDD=3.8V
        LCDC_MIPI_CMD_DATA(0xC1, 0x10),              // POWERCONTROL2
        LCDC_MIPI_CMD_DATA(0xC5, 0x2B, 0x2B),        // VCOMCONTROL1 - VMH=3.775V - VML=-1.425V
        LCDC_MIPI_CMD_DATA(0xC7, 0xC0),              // VCOMCONTROL2 -
#if DT280QV10CT_LANDSCAPE
        LCDC_MIPI_SET_ADDR_MODE(0x28),
#else
        LCDC_MIPI_SET_ADDR_MODE(0x48),
#endif

        LCDC_MIPI_CMD_DATA(0xB1, 0x00, 0x1F),        // FRAMECONTROL
        LCDC_MIPI_CMD_DATA(0xB5, 0x5F, 0x60, 0x1F, 0x1F),

        LCDC_MIPI_SET_POSITION(GDI_DISP_OFFSETX, GDI_DISP_OFFSETY,
                GDI_DISP_RESX + GDI_DISP_OFFSETX - 1, GDI_DISP_RESY + GDI_DISP_OFFSETY - 1),
        LCDC_MIPI_SET_MODE(GDI_DISP_COLOR == HW_LCDC_OCM_8RGB565 ? DT280QV10CT_PIXEL_FORMAT_RGB565 :
                                                                   DT280QV10CT_PIXEL_FORMAT_RGB666),
        LCDC_MIPI_SET_TEAR_ON(0x00),
};

static const uint8_t screen_power_on_cmds[] = {
};

static const uint8_t screen_enable_cmds[] = {
        LCDC_GPIO_SET_ACTIVE(DT280QV10CT_BL_PORT, DT280QV10CT_BL_PIN), // Enable the backlight
        LCDC_MIPI_CMD_DATA(HW_LCDC_MIPI_DCS_EXIT_SLEEP_MODE),
        LCDC_DELAY_MS(120),
        LCDC_MIPI_CMD_DATA(HW_LCDC_MIPI_DCS_SET_DISPLAY_ON),
        LCDC_DELAY_MS(80),
};

static const uint8_t screen_disable_cmds[] = {
        LCDC_MIPI_CMD_DATA(HW_LCDC_MIPI_DCS_SET_DISPLAY_OFF),
        LCDC_DELAY_MS(50),
        LCDC_MIPI_CMD_DATA(HW_LCDC_MIPI_DCS_ENTER_SLEEP_MODE),
        LCDC_DELAY_MS(120),
        LCDC_GPIO_SET_INACTIVE(DT280QV10CT_BL_PORT, DT280QV10CT_BL_PIN), // Disable the backlight
};

static const uint8_t screen_power_off_cmds[] = {
};

static const uint8_t screen_clear_cmds[] = {
};

__STATIC_INLINE void screen_set_partial_update_area(hw_lcdc_frame_t *frame)
{
        UNUSED_ARG(frame);  //! Just to suppress compiler warnings
}

#endif /* dg_configLCDC_ADAPTER */

#endif /* dg_configUSE_DT280QV10CT */

#endif /* DT280QV10CT_H_ */
