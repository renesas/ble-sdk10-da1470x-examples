/**
 ****************************************************************************************
 *
 * @file boe139f454sm.h
 *
 * @brief LCD configuration for BOE139F454SM AMOLED with IC RM67162FI
 *
 * Copyright (C) 2020-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef BOE139F454SM_H_
#define BOE139F454SM_H_

#include <stdint.h>
#include "platform_devices.h"

#if dg_configUSE_BOE139F454SM

#if dg_configLCDC_ADAPTER

/*********************************************************************
 *
 *       Defines
 *
 *********************************************************************
 */
#define GDI_DISP_COLOR           (HW_LCDC_OCM_8RGB888)
#define GDI_DISP_RESX            (454)
#define GDI_DISP_RESY            (454)
#define GDI_DISP_OFFSETX         (12)
#define GDI_DISP_OFFSETY         (0)
#define GDI_LCDC_CONFIG          (&boe139f454sm_cfg)
#define GDI_USE_CONTINUOUS_MODE  (0)
#define BOE139F454SM_TE_ENABLE   (0)

static GDI_DRV_CONF_ATTR ad_lcdc_driver_conf_t boe139f454sm_drv = {
        .hw_init.phy_type = HW_LCDC_PHY_MIPI_DSI,
        .hw_init.format = GDI_DISP_COLOR,
        .hw_init.resx = GDI_DISP_RESX,
        .hw_init.resy = GDI_DISP_RESY,
        .hw_init.cfg_extra_flags = 0,
        .hw_init.mode = HW_LCDC_MODE_DISABLE,
        .hw_init.dsi_hs_freq = HW_LCDC_DSI_FREQ_320MHz,
        .hw_init.dither = HW_LCDC_DITHER_OFF,
        .ext_clk = HW_LCDC_EXT_CLK_OFF,
        .te_enable = BOE139F454SM_TE_ENABLE ? true : false,
        .te_mode = HW_LCDC_TE_TRIG,

        /* DSI specific configuration */
        .hw_init.iface_conf.dsi.vc = 0,
        .hw_init.iface_conf.dsi.lane_en = HW_LCDC_DSI_LANE_D0,
        .hw_init.iface_conf.dsi.video_mode = HW_LCDC_DSI_VID_MODE_DISABLE,
};

static const ad_lcdc_controller_conf_t boe139f454sm_cfg = {
        .io = &boe139f454sm_io,
        .drv = &boe139f454sm_drv,
};

typedef enum {
        BOE139F454SM_PIXEL_FORMAT_L8 = 0x01,              //SPI3/4 only
        BOE139F454SM_PIXEL_FORMAT_RGB332 = 0x02,          //SPI3/4 only
        BOE139F454SM_PIXEL_FORMAT_RGB111 = 0x03,          //SPI3/4 only
        BOE139F454SM_PIXEL_FORMAT_RGB565 = 0x55,
        BOE139F454SM_PIXEL_FORMAT_RGB666 = 0x66,
        BOE139F454SM_PIXEL_FORMAT_RGB888 = 0x77,
} BOE139F454SM_PIXEL_FORMAT;

/* Required (and supported) output color modes */
static const HW_LCDC_OUTPUT_COLOR_MODE screen_color_modes[] = {
        HW_LCDC_OCM_8RGB565,
        HW_LCDC_OCM_8RGB666,
        HW_LCDC_OCM_8RGB888,
};

/* Callback function to configure display's color mode */
__STATIC_INLINE bool screen_set_color_mode(HW_LCDC_OUTPUT_COLOR_MODE color_mode)
{
        uint8_t value = 0;

        /* Exercise color mode */
        switch (color_mode) {
        case HW_LCDC_OCM_8RGB565:
                value = BOE139F454SM_PIXEL_FORMAT_RGB565;
                break;
        case HW_LCDC_OCM_8RGB666:
                value = BOE139F454SM_PIXEL_FORMAT_RGB666;
                break;
        case HW_LCDC_OCM_8RGB888:
                value = BOE139F454SM_PIXEL_FORMAT_RGB888;
                break;
        default:
                return false; // Unsupported color mode
        }
        hw_lcdc_dcs_cmd_params(HW_LCDC_MIPI_DCS_SET_PIXEL_FORMAT, &value, sizeof(value));

        return true;
}

static const uint8_t screen_init_cmds[] = {
        LCDC_GPIO_SET_INACTIVE(BOE139F454SM_RESX_PORT, BOE139F454SM_RESX_PIN),
        LCDC_DELAY_MS(20),
        LCDC_GPIO_SET_ACTIVE(BOE139F454SM_RESX_PORT, BOE139F454SM_RESX_PIN),
        LCDC_DELAY_MS(20),
        LCDC_MIPI_DSI_TRIG(HW_LCDC_DSI_TRIG_RESET),
        LCDC_DELAY_MS(20),

        LCDC_MIPI_CMD_DATA(0xFE, 0x01),
        LCDC_MIPI_CMD_DATA(0x70, 0x55),

        LCDC_MIPI_CMD_DATA(0xFE, 0x0A),
        LCDC_MIPI_CMD_DATA(0x29, 0x10),

        LCDC_MIPI_CMD_DATA(0xFE, 0x05),
        LCDC_MIPI_CMD_DATA(0x05, 0x01),

        LCDC_MIPI_CMD_DATA(0xFE, 0x00),

        LCDC_MIPI_CMD_DATA(HW_LCDC_MIPI_DCS_SET_COLUMN_ADDRESS, GDI_DISP_OFFSETX >> 8, GDI_DISP_OFFSETX & 0xFF,
                (GDI_DISP_OFFSETX + GDI_DISP_RESX - 1) >> 8, (GDI_DISP_OFFSETX + GDI_DISP_RESX - 1) & 0xFF),
        LCDC_MIPI_CMD_DATA(HW_LCDC_MIPI_DCS_SET_PAGE_ADDRESS, GDI_DISP_OFFSETY >> 8, GDI_DISP_OFFSETY & 0xFF,
                (GDI_DISP_OFFSETY + GDI_DISP_RESY - 1) >> 8, (GDI_DISP_OFFSETY + GDI_DISP_RESY - 1) & 0xFF),

        LCDC_MIPI_CMD_DATA(HW_LCDC_MIPI_DCS_EXIT_SLEEP_MODE),
        LCDC_DELAY_MS(120),
        LCDC_MIPI_CMD_DATA(HW_LCDC_MIPI_DCS_SET_DISPLAY_ON),

#if BOE139F454SM_TE_ENABLE
        LCDC_MIPI_CMD_DATA(HW_LCDC_MIPI_DCS_SET_TEAR_ON, 0x00),
#else
        LCDC_MIPI_CMD_DATA(HW_LCDC_MIPI_DCS_SET_TEAR_OFF),
#endif
        LCDC_MIPI_CMD_DATA(HW_LCDC_MIPI_DCS_SET_PIXEL_FORMAT,
                GDI_DISP_COLOR == HW_LCDC_OCM_8RGB565 ? BOE139F454SM_PIXEL_FORMAT_RGB565 :
                GDI_DISP_COLOR == HW_LCDC_OCM_8RGB666 ? BOE139F454SM_PIXEL_FORMAT_RGB666 : BOE139F454SM_PIXEL_FORMAT_RGB888),

        LCDC_MIPI_DSI_RX_LEN(128),
};

static const uint8_t screen_power_on_cmds[] = {
};

static const uint8_t screen_enable_cmds[] = {
};

static const uint8_t screen_disable_cmds[] =
{
};

static const uint8_t screen_power_off_cmds[] = {
};

static const uint8_t screen_clear_cmds[] = {
};

__STATIC_INLINE void screen_set_partial_update_area(hw_lcdc_frame_t *frame)
{
        frame->startx = 0;
        frame->starty = 0;
        frame->endx = GDI_DISP_RESX - 1;
        frame->endy = GDI_DISP_RESY - 1;
}

#endif /* dg_configLCDC_ADAPTER */

#endif /* dg_configUSE_BOE139F454SM */

#endif /* BOE139F454SM_H_ */
