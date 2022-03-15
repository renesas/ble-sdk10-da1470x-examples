/**
 ****************************************************************************************
 *
 * @file e1394aa65a.h
 *
 * @brief LCD configuration for E1394AA65 AMOLED with IC RM69330
 *
 * Copyright (C) 2020-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef E1394AA65A_H_
#define E1394AA65A_H_

#include <stdint.h>
#include "platform_devices.h"

#if dg_configUSE_E1394AA65A

#if dg_configLCDC_ADAPTER

/*********************************************************************
 *
 *       Defines
 *
 *********************************************************************
 */
#if 0
#define GDI_DISP_COLOR           (HW_LCDC_OCM_8RGB332)
#define GDI_DISP_RESX            (468)
#define GDI_DISP_RESY            (454)
#else
#define GDI_DISP_COLOR           (HW_LCDC_OCM_8RGB888)
#define GDI_DISP_RESX            (320)
#define GDI_DISP_RESY            (320)
#define GDI_DISP_OFFSETX         (74)
#define GDI_DISP_OFFSETY         (67)
#endif
#define GDI_LCDC_CONFIG          (&e1394aa65a_cfg)
#define GDI_USE_CONTINUOUS_MODE  (0)

/*
 * static const ad_io_conf_t e1394aa65a_gpio_cfg[] = {
 *       { E1394AA65A_SCK_PORT, E1394AA65A_SCK_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_CLK, true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { E1394AA65A_DO_PORT,  E1394AA65A_DO_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DO,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { E1394AA65A_DC_PORT,  E1394AA65A_DC_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DC,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { E1394AA65A_CS_PORT,  E1394AA65A_CS_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_EN,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { E1394AA65A_RST_PORT, E1394AA65A_RST_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { E1394AA65A_TE_PORT,  E1394AA65A_TE_PIN,  { HW_GPIO_MODE_INPUT,  HW_GPIO_FUNC_LCD,         true }, { HW_GPIO_MODE_INPUT,  HW_GPIO_FUNC_GPIO, true }},
 * };

 * const ad_lcdc_io_conf_t e1394aa65a_io = {
 *       .voltage_level = HW_GPIO_POWER_VDD1V8P,
 *       .io_cnt = sizeof(e1394aa65a_gpio_cfg) / sizeof(e1394aa65a_gpio_cfg[0]),
 *       .io_list = e1394aa65a_gpio_cfg,
 * };
 */

static GDI_DRV_CONF_ATTR ad_lcdc_driver_conf_t e1394aa65a_drv = {
        .hw_init.phy_type = HW_LCDC_PHY_MIPI_SPI4,
        .hw_init.format = GDI_DISP_COLOR,
        .hw_init.resx = GDI_DISP_RESX,
        .hw_init.resy = GDI_DISP_RESY,
        .hw_init.cfg_extra_flags = 0,
        .hw_init.mode = HW_LCDC_MODE_DISABLE,
        .hw_init.write_freq = LCDC_FREQ_48MHz,  // Max. @50MHz
        .hw_init.read_freq = LCDC_FREQ_3_2MHz,  // Max. @3.3MHz
        .ext_clk = HW_LCDC_EXT_CLK_OFF,
        .te_enable = false,
        .te_mode = HW_LCDC_TE_POL_HIGH,
};

static const ad_lcdc_controller_conf_t e1394aa65a_cfg = {
        .io = &e1394aa65a_io,
        .drv = &e1394aa65a_drv,
};

typedef enum {
        E1394AA65A_PIXEL_FORMAT_RGB111 = 0x3,
        E1394AA65A_PIXEL_FORMAT_L8     = 0x1,
        E1394AA65A_PIXEL_FORMAT_RGB332 = 0x2,
        E1394AA65A_PIXEL_FORMAT_RGB565 = 0x5,
        E1394AA65A_PIXEL_FORMAT_RGB666 = 0x6,
        E1394AA65A_PIXEL_FORMAT_RGB888 = 0x7,
} E1394AA65A_PIXEL_FORMAT;

#if (DEVICE_FAMILY == DA1470X)
typedef enum {
        E1394AA65A_PIXEL_FORMAT_OPT_RGB111_1 = 0x07,
        E1394AA65A_PIXEL_FORMAT_OPT_RGB111_3 = 0x47,
} E1394AA65A_PIXEL_FORMAT_OPT;

#endif
/* Required (and supported) output color modes */
static const HW_LCDC_OUTPUT_COLOR_MODE screen_color_modes[] = {
        HW_LCDC_OCM_8RGB111_1,
#if (DEVICE_FAMILY == DA1470X)
        HW_LCDC_OCM_8RGB111_3,
#endif
        HW_LCDC_OCM_8RGB332,
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
        case HW_LCDC_OCM_8RGB111_1:
#if (DEVICE_FAMILY == DA1470X)
                value = E1394AA65A_PIXEL_FORMAT_OPT_RGB111_1;
                hw_lcdc_dcs_cmd_params(0x80, &value, sizeof(value));
#endif
                value = E1394AA65A_PIXEL_FORMAT_RGB111;
                break;
#if (DEVICE_FAMILY == DA1470X)
        case HW_LCDC_OCM_8RGB111_3:
                value = E1394AA65A_PIXEL_FORMAT_OPT_RGB111_3;
                hw_lcdc_dcs_cmd_params(0x80, &value, sizeof(value));
                value = E1394AA65A_PIXEL_FORMAT_RGB111;
                break;
#endif
        case HW_LCDC_OCM_8RGB332:
                value = E1394AA65A_PIXEL_FORMAT_RGB332;
                break;
        case HW_LCDC_OCM_8RGB565:
                value = E1394AA65A_PIXEL_FORMAT_RGB565;
                break;
        case HW_LCDC_OCM_8RGB666:
                value = E1394AA65A_PIXEL_FORMAT_RGB666;
                break;
        case HW_LCDC_OCM_8RGB888:
                value = E1394AA65A_PIXEL_FORMAT_RGB888;
                break;
        default:
                return false; // Unsupported color mode
        }
        hw_lcdc_dcs_cmd_params(HW_LCDC_MIPI_DCS_SET_PIXEL_FORMAT, &value, sizeof(value));

        return true;
}

static const uint8_t screen_init_cmds[] = {
        LCDC_GPIO_SET_INACTIVE(E1394AA65A_RST_PORT, E1394AA65A_RST_PIN),
        LCDC_DELAY_MS(50),
        LCDC_GPIO_SET_ACTIVE(E1394AA65A_RST_PORT, E1394AA65A_RST_PIN),
        LCDC_DELAY_MS(50),

        LCDC_MIPI_CMD_DATA(0xC4, 0x80),  //! Enable SPI interface write RAM
        LCDC_MIPI_CMD_DATA(HW_LCDC_MIPI_DCS_SET_DISPLAY_BRIGHTNESS, 0x7F),

        LCDC_MIPI_SET_POSITION(GDI_DISP_OFFSETX, GDI_DISP_OFFSETY,
                GDI_DISP_RESX + GDI_DISP_OFFSETX - 1, GDI_DISP_RESY + GDI_DISP_OFFSETY - 1),
        LCDC_MIPI_SET_MODE(GDI_DISP_COLOR == HW_LCDC_OCM_8RGB111_1 ? E1394AA65A_PIXEL_FORMAT_RGB111 :
#if (DEVICE_FAMILY == DA1470X)
                           GDI_DISP_COLOR == HW_LCDC_OCM_8RGB111_3 ? E1394AA65A_PIXEL_FORMAT_RGB111 :
#endif
                           GDI_DISP_COLOR == HW_LCDC_OCM_8RGB332   ? E1394AA65A_PIXEL_FORMAT_RGB332 :
                           GDI_DISP_COLOR == HW_LCDC_OCM_8RGB565   ? E1394AA65A_PIXEL_FORMAT_RGB565 :
                           GDI_DISP_COLOR == HW_LCDC_OCM_8RGB666   ? E1394AA65A_PIXEL_FORMAT_RGB666 :
                                                                     E1394AA65A_PIXEL_FORMAT_RGB888),
#if (DEVICE_FAMILY == DA1470X)
        LCDC_MIPI_CMD_DATA(0x80, GDI_DISP_COLOR == HW_LCDC_OCM_8RGB111_3 ? E1394AA65A_PIXEL_FORMAT_OPT_RGB111_3 :
                                                                           E1394AA65A_PIXEL_FORMAT_OPT_RGB111_1),
#endif
        LCDC_MIPI_SET_TEAR_ON(0x0),
};

static const uint8_t screen_power_on_cmds[] = {
};

static const uint8_t screen_enable_cmds[] = {
        LCDC_MIPI_CMD_DATA(HW_LCDC_MIPI_DCS_EXIT_SLEEP_MODE),
        LCDC_DELAY_MS(120),
        LCDC_MIPI_CMD_DATA(HW_LCDC_MIPI_DCS_SET_DISPLAY_ON),
        LCDC_DELAY_MS(50),
};

static const uint8_t screen_disable_cmds[] =
{
        LCDC_MIPI_CMD_DATA(HW_LCDC_MIPI_DCS_SET_DISPLAY_OFF),
        LCDC_DELAY_MS(120),
        LCDC_MIPI_CMD_DATA(HW_LCDC_MIPI_DCS_ENTER_SLEEP_MODE),
        LCDC_DELAY_MS(50),
};

static const uint8_t screen_power_off_cmds[] = {
};

static const uint8_t screen_clear_cmds[] = {
};

__STATIC_INLINE void screen_set_partial_update_area(hw_lcdc_frame_t *frame)
{
        /* The SC[9:0] and EC[9:0]-SC[9:0]+1 must be divisible by 2 */
        /* The SP[9:0] and EP[9:0]-SP[9:0]+1 must be divisible by 2 */
        if (frame->startx & 0x1) {
                frame->startx--;
        }
        if (frame->starty & 0x1) {
                frame->starty--;
        }
        if ((frame->endx - frame->startx + 1) & 0x1) {
                frame->endx++;
        }
        if ((frame->endy - frame->starty + 1) & 0x1) {
                frame->endy++;
        }
}

#endif /* dg_configLCDC_ADAPTER */

#endif /* dg_configUSE_E1394AA65A */

#endif /* E1394AA65A_H_ */
