/**
 ****************************************************************************************
 *
 * @file mct024l6w240320pml.h
 *
 * @brief LCD configuration for MCT024L6W240320PML with IC ILI9341
 *
 * Copyright (C) 2020-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef MCT024L6W240320PML_H_
#define MCT024L6W240320PML_H_

#include <stdint.h>
#include "platform_devices.h"

#if dg_configUSE_MCT024L6W240320PML

#if dg_configLCDC_ADAPTER

/*********************************************************************
 *
 *       Defines
 *
 *********************************************************************
 */
#define GDI_DISP_COLOR           (HW_LCDC_OCM_8RGB666)
#define GDI_DISP_RESX            (240)
#define GDI_DISP_RESY            (320)
#define GDI_DISP_OFFSETX         (0)
#define GDI_DISP_OFFSETY         (0)
#define GDI_LCDC_CONFIG          (&mct024l6w240320pml_cfg)
#define GDI_USE_CONTINUOUS_MODE  (0)

/*
 * static const ad_io_conf_t mct024l6w240320pml_gpio_cfg[] = {
 *       {MCT024L6W240320PML_SDA_PORT, MCT024L6W240320PML_SDA_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DO,  true}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       {MCT024L6W240320PML_SCK_PORT, MCT024L6W240320PML_SCK_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_CLK, true}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       {MCT024L6W240320PML_CS_PORT,  MCT024L6W240320PML_CS_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_EN,  true}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       {MCT024L6W240320PML_DC_PORT,  MCT024L6W240320PML_DC_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DC,  true}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       {MCT024L6W240320PML_RST_PORT, MCT024L6W240320PML_RST_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        true}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 * };

 * const ad_lcdc_io_conf_t mct024l6w240320pml_io = {
 *       .voltage_level = HW_GPIO_POWER_V33,
 *       .io_cnt = sizeof(mct024l6w240320pml_gpio_cfg) / sizeof(mct024l6w240320pml_gpio_cfg[0]),
 *       .io_list = mct024l6w240320pml_gpio_cfg,
 * };
 */

static GDI_DRV_CONF_ATTR ad_lcdc_driver_conf_t mct024l6w240320pml_drv = {
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

static const ad_lcdc_controller_conf_t mct024l6w240320pml_cfg = {
        .io = &mct024l6w240320pml_io,
        .drv = &mct024l6w240320pml_drv,
};

typedef enum {
        MCT024L6W240320PML_PIXEL_FORMAT_RGB565 = 0x5,
        MCT024L6W240320PML_PIXEL_FORMAT_RGB666 = 0x6,
} MCT024L6W240320PML_PIXEL_FORMAT;

/* Required (and supported) output color modes */
static const HW_LCDC_OUTPUT_COLOR_MODE screen_color_modes[] = {
        HW_LCDC_OCM_8RGB565,
        HW_LCDC_OCM_8RGB666,
};

/* Callback function to configure display's color mode */
__STATIC_INLINE bool screen_set_color_mode(HW_LCDC_OUTPUT_COLOR_MODE color_mode)
{
        uint8_t value = 0;

        /* Exercise color mode */
        switch (color_mode) {
        case HW_LCDC_OCM_8RGB565:
                value = MCT024L6W240320PML_PIXEL_FORMAT_RGB565;
                break;
        case HW_LCDC_OCM_8RGB666:
                value = MCT024L6W240320PML_PIXEL_FORMAT_RGB666;
                break;
        default:
                return false; // Unsupported color mode
        }
        hw_lcdc_dcs_cmd_params(HW_LCDC_MIPI_DCS_SET_PIXEL_FORMAT, &value, sizeof(value));

        return true;
}

static const uint8_t screen_init_cmds[] = {
        /* Power on */
        LCDC_GPIO_SET_INACTIVE(MCT024L6W240320PML_RST_PORT, MCT024L6W240320PML_RST_PIN),
        LCDC_DELAY_MS(10),
        LCDC_GPIO_SET_ACTIVE(MCT024L6W240320PML_RST_PORT, MCT024L6W240320PML_RST_PIN),
        LCDC_DELAY_MS(120),

        LCDC_MIPI_SW_RST(),
        LCDC_DELAY_MS(100),

        LCDC_MIPI_CMD_DATA(0xC0, 0x23),              // POWERCONTROL1 - GVDD=3.8V
        LCDC_MIPI_CMD_DATA(0xC1, 0x10),              // POWERCONTROL2
        LCDC_MIPI_CMD_DATA(0xC5, 0x2B, 0x2B),        // VCOMCONTROL1 - VMH=3.775V - VML=-1.425V
        LCDC_MIPI_CMD_DATA(0xC7, 0xC0),              // VCOMCONTROL2 -
        LCDC_MIPI_SET_ADDR_MODE(0x88),

        LCDC_MIPI_SET_MODE(GDI_DISP_COLOR == HW_LCDC_OCM_8RGB111_1 ? MCT024L6W240320PML_PIXEL_FORMAT_RGB565 :
                                                                     MCT024L6W240320PML_PIXEL_FORMAT_RGB666),

        LCDC_MIPI_CMD_DATA(0xB1, 0x00, 0x1B),        // FRAMECONTROL - 70Hz
};

static const uint8_t screen_power_on_cmds[] = {
};

static const uint8_t screen_enable_cmds[] = {
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

#endif /* dg_configUSE_MCT024L6W240320PML */

#endif /* MCT024L6W240320PML_H_ */
