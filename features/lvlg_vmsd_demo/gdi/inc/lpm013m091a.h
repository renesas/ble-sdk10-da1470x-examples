/**
 ****************************************************************************************
 *
 * @file lpm013m091a.h
 *
 * @brief LCD configuration for LCD LPM013M091A with IC NT35350
 *
 * Copyright (C) 2020-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef LPM013M091A_H_
#define LPM013M091A_H_

#include <stdint.h>
#include "platform_devices.h"

#if dg_configUSE_LPM013M091A

#if dg_configLCDC_ADAPTER

/*********************************************************************
 *
 *       Defines
 *
 *********************************************************************
 */
#define GDI_DISP_COLOR           (HW_LCDC_OCM_8RGB565)
#define GDI_DISP_RESX            (320)
#define GDI_DISP_RESY            (300)
#define GDI_DISP_OFFSETX         (0)
#define GDI_DISP_OFFSETY         (0)
#define GDI_LCDC_CONFIG          (&lpm013m091a_cfg)
#define GDI_USE_CONTINUOUS_MODE  (0)

/*
 * static const ad_io_conf_t lpm013m091a_gpio_cfg[] = {
 *       { LPM013M091A_SCL_PORT, LPM013M091A_SCL_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_CLK, true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { LPM013M091A_SDI_PORT, LPM013M091A_SDI_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DO,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { LPM013M091A_RST_PORT, LPM013M091A_RST_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 * #if 0
 *       { LPM013M091A_DCX_PORT, LPM013M091A_DCX_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  }},
 *       { LPM013M091A_IM0_PORT, LPM013M091A_IM0_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        false}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false }},
 * #else
 *       { LPM013M091A_DCX_PORT, LPM013M091A_DCX_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DC,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  }},
 *       { LPM013M091A_IM0_PORT, LPM013M091A_IM0_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  }},
 * #endif
 *       { LPM013M091A_CSX_PORT, LPM013M091A_CSX_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_EN,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { LPM013M091A_TE_PORT,  LPM013M091A_TE_PIN,  { HW_GPIO_MODE_INPUT,  HW_GPIO_FUNC_LCD,         true }, { HW_GPIO_MODE_INPUT,  HW_GPIO_FUNC_GPIO, true }},
 * };

 * const ad_lcdc_io_conf_t lpm013m091a_io = {
 *       .voltage_level = HW_GPIO_POWER_VDD1V8P,
 *       .io_cnt = sizeof(lpm013m091a_gpio_cfg) / sizeof(lpm013m091a_gpio_cfg[0]),
 *       .io_list = lpm013m091a_gpio_cfg,
 * };
 */

static GDI_DRV_CONF_ATTR ad_lcdc_driver_conf_t lpm013m091a_drv = {
        .hw_init.phy_type = HW_LCDC_PHY_MIPI_SPI4,
        .hw_init.format = GDI_DISP_COLOR,
        .hw_init.resx = GDI_DISP_RESX,
        .hw_init.resy = GDI_DISP_RESY,
        .hw_init.cfg_extra_flags = 0,
        .hw_init.mode = HW_LCDC_MODE_DISABLE,
        .hw_init.write_freq = LCDC_FREQ_48MHz,
        .ext_clk = HW_LCDC_EXT_CLK_OFF,
        .te_enable = false, //true,
        .te_mode = HW_LCDC_TE_POL_LOW,
};

static const ad_lcdc_controller_conf_t lpm013m091a_cfg = {
        .io = &lpm013m091a_io,
        .drv = &lpm013m091a_drv,
};

typedef enum {
        LPM013M091A_PIXEL_FORMAT_RGB111   = 0x1,
        LPM013M091A_PIXEL_FORMAT_RGB111_1 = 0x9,
        LPM013M091A_PIXEL_FORMAT_RGB565   = 0x5,
        LPM013M091A_PIXEL_FORMAT_RGB666   = 0x6,
} LPM013M091A_PIXEL_FORMAT;

/* Required (and supported) output color modes */
static const HW_LCDC_OUTPUT_COLOR_MODE screen_color_modes[] = {
        HW_LCDC_OCM_RGB111,
        HW_LCDC_OCM_8RGB111_1,
        HW_LCDC_OCM_8RGB565,
        HW_LCDC_OCM_8RGB666,
};

/* Callback function to configure display's color mode */
__STATIC_INLINE bool screen_set_color_mode(HW_LCDC_OUTPUT_COLOR_MODE color_mode)
{
        uint8_t value = 0;

        /* Exercise color mode */
        switch (color_mode) {
        case HW_LCDC_OCM_RGB111:
                value = LPM013M091A_PIXEL_FORMAT_RGB111;
                break;
        case HW_LCDC_OCM_8RGB111_1:
                value = LPM013M091A_PIXEL_FORMAT_RGB111_1;
                break;
        case HW_LCDC_OCM_8RGB565:
                value = LPM013M091A_PIXEL_FORMAT_RGB565;
                break;
        case HW_LCDC_OCM_8RGB666:
                value = LPM013M091A_PIXEL_FORMAT_RGB666;
                break;
        default:
                return false; // Unsupported color mode
        }
        hw_lcdc_dcs_cmd_params(HW_LCDC_MIPI_DCS_SET_PIXEL_FORMAT, &value, sizeof(value));

        return true;
}

static const uint8_t screen_init_cmds[] = {
        /* Power on */
        LCDC_GPIO_SET_INACTIVE(LPM013M091A_RST_PORT, LPM013M091A_RST_PIN),
        LCDC_DELAY_MS(10),
        LCDC_GPIO_SET_ACTIVE(LPM013M091A_RST_PORT, LPM013M091A_RST_PIN),
        LCDC_DELAY_MS(120),

        /* On to Analog Mode */
        LCDC_MIPI_SW_RST(),
        LCDC_DELAY_MS(10),
        LCDC_MIPI_CMD_DATA(0xF3, 0x02),      // SPI GRAM access enable

        LCDC_MIPI_SET_POSITION(GDI_DISP_OFFSETX, GDI_DISP_OFFSETY,
                GDI_DISP_RESX + GDI_DISP_OFFSETX - 1, GDI_DISP_RESY + GDI_DISP_OFFSETY - 1),

        LCDC_MIPI_SET_MODE(GDI_DISP_COLOR == HW_LCDC_OCM_RGB111    ? LPM013M091A_PIXEL_FORMAT_RGB111 :
                           GDI_DISP_COLOR == HW_LCDC_OCM_8RGB111_1 ? LPM013M091A_PIXEL_FORMAT_RGB111_1 :
                           GDI_DISP_COLOR == HW_LCDC_OCM_8RGB565   ? LPM013M091A_PIXEL_FORMAT_RGB565 :
                                                                     LPM013M091A_PIXEL_FORMAT_RGB666),

        /* Refresh frequency to 30Hz */
        LCDC_MIPI_CMD_DATA(0xFF, 0x24),
        LCDC_MIPI_CMD_DATA(0xD8, 0xC1),
        LCDC_MIPI_CMD_DATA(0xD9, 0x3E),
        LCDC_MIPI_CMD_DATA(0xFF, 0x10),
};

static const uint8_t screen_power_on_cmds[] = {
};

static const uint8_t screen_enable_cmds[] = {

        LCDC_MIPI_CMD_DATA(HW_LCDC_MIPI_DCS_EXIT_SLEEP_MODE),
        LCDC_DELAY_MS(10),
        LCDC_MIPI_CMD_DATA(HW_LCDC_MIPI_DCS_SET_DISPLAY_ON),
};

static const uint8_t screen_disable_cmds[] =
{
        LCDC_MIPI_CMD_DATA(HW_LCDC_MIPI_DCS_SET_DISPLAY_OFF),
        LCDC_DELAY_MS(120),
        LCDC_MIPI_CMD_DATA(HW_LCDC_MIPI_DCS_ENTER_SLEEP_MODE),
        LCDC_DELAY_MS(5),
};

static const uint8_t screen_power_off_cmds[] = {
};


static const uint8_t screen_clear_cmds[] = {
};

__STATIC_INLINE void screen_set_partial_update_area(hw_lcdc_frame_t *frame)
{
        if (frame->startx & 0x1) {
                frame->startx--;
        }
        if (frame->starty & 0x1) {
                frame->starty--;
        }
        if (!(frame->endx & 0x1)) {
                frame->endx++;
        }
        if (!(frame->endy & 0x1)) {
                frame->endy++;
        }
}

#endif /* dg_configLCDC_ADAPTER */

#endif /* dg_configUSE_LPM013M091A */

#endif /* LPM013M091A_H_ */
