/**
 ****************************************************************************************
 *
 * @file nhd43480272efasxn.h
 *
 * @brief LCD configuration for NHD-4.3-480272EF-ASXN# LCD with IC ST7282T2
 *
 * Copyright (C) 2020-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#ifndef NHD43480272EFASXN_H_
#define NHD43480272EFASXN_H_

#include <stdio.h>
#include <stdint.h>
#include "platform_devices.h"
#include "ad_i2c.h"

#if dg_configUSE_NHD43480272EFASXN

#if dg_configLCDC_ADAPTER

/*********************************************************************
 *
 *       Defines
 *
 **********************************************************************
 */
#define GDI_DISP_COLOR                  (HW_LCDC_OCM_RGB222)
#define GDI_DISP_RESX                   (480)
#define GDI_DISP_RESY                   (272)
#define GDI_DISP_OFFSETX                (0)
#define GDI_DISP_OFFSETY                (0)
#define GDI_USE_CONTINUOUS_MODE         (1)
#define GDI_LCDC_CONFIG                 (&nhd43480272efasxn_cfg)

/*
 * static const ad_io_conf_t nhd43480272efasxn_gpio_cfg[] = {
 *       { NHD43480272EFASXN_GREEN_0_PORT, NHD43480272EFASXN_GREEN_0_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 *       { NHD43480272EFASXN_GREEN_1_PORT, NHD43480272EFASXN_GREEN_1_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 *       { NHD43480272EFASXN_RED_0_PORT,   NHD43480272EFASXN_RED_0_PIN,   { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 *       { NHD43480272EFASXN_RED_1_PORT,   NHD43480272EFASXN_RED_1_PIN,   { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 *       { NHD43480272EFASXN_BLUE_0_PORT,  NHD43480272EFASXN_BLUE_0_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 *       { NHD43480272EFASXN_BLUE_1_PORT,  NHD43480272EFASXN_BLUE_1_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 *       { NHD43480272EFASXN_VSYNC_PORT,   NHD43480272EFASXN_VSYNC_PIN,   { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  } },
 *       { NHD43480272EFASXN_HSYNC_PORT,   NHD43480272EFASXN_HSYNC_PIN,   { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  } },
 *       { NHD43480272EFASXN_DEN_PORT,     NHD43480272EFASXN_DEN_PIN,     { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 *       { NHD43480272EFASXN_CLK_PORT,     NHD43480272EFASXN_CLK_PIN,     { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 *       { NHD43480272EFASXN_DISP_PORT,    NHD43480272EFASXN_DISP_PIN,    { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 * };

 * const ad_lcdc_io_conf_t nhd43480272efasxn_io = {
 *       .voltage_level = HW_GPIO_POWER_V33,
 *       .io_cnt = sizeof(nhd43480272efasxn_gpio_cfg) / sizeof(nhd43480272efasxn_gpio_cfg[0]),
 *       .io_list = nhd43480272efasxn_gpio_cfg,
 * };
 */

static GDI_DRV_CONF_ATTR ad_lcdc_driver_conf_t nhd43480272efasxn_drv = {
        .hw_init.phy_type = HW_LCDC_PHY_DPI,
        .hw_init.format = HW_LCDC_OCM_8RGB888,  //! LCD controller requires that RGB888 is selected
        .hw_init.resx = GDI_DISP_RESX,
        .hw_init.resy = GDI_DISP_RESY,
        .hw_init.cfg_extra_flags = 0,
        .hw_init.mode = HW_LCDC_MODE_DISABLE,
        .hw_init.write_freq = LCDC_FREQ_8MHz,
        .ext_clk = HW_LCDC_EXT_CLK_OFF,
        .te_enable = false,
        .te_mode = HW_LCDC_TE_POL_LOW,

        .hw_init.iface_conf.dpi.enable_dpi_ready = false,

        .hw_init.iface_conf.dpi.fpx = 2,
        .hw_init.iface_conf.dpi.blx = 2,
        .hw_init.iface_conf.dpi.bpx = 3,

        .hw_init.iface_conf.dpi.fpy = 2,
        .hw_init.iface_conf.dpi.bly = 2,
        .hw_init.iface_conf.dpi.bpy = 2,
};

static const ad_lcdc_controller_conf_t nhd43480272efasxn_cfg = {
        .io = &nhd43480272efasxn_io,
        .drv = &nhd43480272efasxn_drv,
};

/* Required (and supported) output color modes */
static const HW_LCDC_OUTPUT_COLOR_MODE screen_color_modes[] = {
        HW_LCDC_OCM_8RGB888,
};

/* Callback function to configure display's color mode */
__STATIC_INLINE bool screen_set_color_mode(HW_LCDC_OUTPUT_COLOR_MODE color_mode)
{
        switch (color_mode) {
        case HW_LCDC_OCM_8RGB888:
                break;
        default:
                return false; //! Unsupported color mode
        }

        return true;
}

static const uint8_t screen_init_cmds[] = {
        /* RGB interface does not support commands. */
};

static const uint8_t screen_power_on_cmds[] = {
};

static const uint8_t screen_enable_cmds[] = {
        LCDC_GPIO_SET_ACTIVE(NHD43480272EFASXN_DISP_PORT, NHD43480272EFASXN_DISP_PIN), // Enable the display
};

static const uint8_t screen_disable_cmds[] = {
        LCDC_GPIO_SET_INACTIVE(NHD43480272EFASXN_DISP_PORT, NHD43480272EFASXN_DISP_PIN), // Disable the display
};

static const uint8_t screen_power_off_cmds[] = {
};

static const uint8_t screen_clear_cmds[] = {
        /* RGB interface does not support commands. */
};

__STATIC_INLINE void screen_set_partial_update_area(hw_lcdc_frame_t *frame)
{
        /* RGB interface does not support partial update  */
        frame->startx = 0;
        frame->starty = 0;
        frame->endx = GDI_DISP_RESX - 1;
        frame->endy = GDI_DISP_RESY - 1;
}

#endif /* dg_configLCDC_ADAPTER */

#endif /* dg_configUSE_NHD43480272EFASXN */

#endif /* NHD43480272EFASXN_H_ */
