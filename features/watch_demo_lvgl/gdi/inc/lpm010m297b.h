/**
 ****************************************************************************************
 *
 * @file lpm010m297b.h
 *
 * @brief LCD configuration for LPM010M297B LCD
 *
 * Copyright (C) 2020-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef LPM010M297B_H_
#define LPM010M297B_H_

#include <stdint.h>
#include "platform_devices.h"

#if dg_configUSE_LPM010M297B

#if dg_configLCDC_ADAPTER

/*********************************************************************
 *
 *       Defines
 *
 *********************************************************************
 */
#define GDI_DISP_COLOR           (HW_LCDC_OCM_RGB111)
#define GDI_DISP_RESX            (208)
#define GDI_DISP_RESY            (208)
#define GDI_DISP_OFFSETX         (0)
#define GDI_DISP_OFFSETY         (0)
#define GDI_LCDC_CONFIG          (&lpm010m297b_cfg)
#define GDI_USE_CONTINUOUS_MODE  (0)

/*
 * static const ad_io_conf_t lpm010m297b_gpio_cfg[] = {
 *       { LPM010M297B_SCK_PORT,      LPM010M297B_SCK_PIN,      { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_CLK, true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  }},
 *       { LPM010M297B_SDA_PORT,      LPM010M297B_SDA_PIN,      { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DO,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  }},
 *       { LPM010M297B_CS_PORT,       LPM010M297B_CS_PIN,       { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_EN,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  }},
 *       { LPM010M297B_EXTCOMIN_PORT, LPM010M297B_EXTCOMIN_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,         false}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false }},
 *       { LPM010M297B_DISP_PORT,     LPM010M297B_DISP_PIN,     { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        false}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false }},
 * };

 * const ad_lcdc_io_conf_t lpm010m297b_io = {
 *       .voltage_level = HW_GPIO_POWER_V33,
 *       .io_cnt = sizeof(lpm010m297b_gpio_cfg) / sizeof(lpm010m297b_gpio_cfg[0]),
 *       .io_list = lpm010m297b_gpio_cfg,
 * };
 */

static GDI_DRV_CONF_ATTR ad_lcdc_driver_conf_t lpm010m297b_drv = {
        .hw_init.phy_type = HW_LCDC_PHY_JDI_SPI,
        .hw_init.format = GDI_DISP_COLOR,
        .hw_init.resx = GDI_DISP_RESX,
        .hw_init.resy = GDI_DISP_RESY,
        .hw_init.cfg_extra_flags = 0,
        .hw_init.mode = HW_LCDC_MODE_DISABLE,
        .hw_init.write_freq = LCDC_FREQ_2MHz,        //! 2MHz
        .ext_clk = HW_LCDC_EXT_CLK_62_5HZ,
        .te_enable = false,
        .te_mode = HW_LCDC_TE_POL_LOW,
};

static const ad_lcdc_controller_conf_t lpm010m297b_cfg = {
        .io = &lpm010m297b_io,
        .drv = &lpm010m297b_drv,
};

/* Required (and supported) output color modes */
static const HW_LCDC_OUTPUT_COLOR_MODE screen_color_modes[] = {
        HW_LCDC_OCM_L1,
        HW_LCDC_OCM_RGB111,
        HW_LCDC_OCM_8RGB111_2,
};

/* Callback function to configure display's color mode */
__STATIC_INLINE bool screen_set_color_mode(HW_LCDC_OUTPUT_COLOR_MODE color_mode)
{
        /* Exercise color mode */
        switch (color_mode) {
        case HW_LCDC_OCM_L1:
        case HW_LCDC_OCM_RGB111:
        case HW_LCDC_OCM_8RGB111_2:
                break;
        default:
                return false; // Unsupported color mode
        }

        return true;
}

static const uint8_t screen_init_cmds[] = {
        LCDC_JDI_CLEAR(),
};

static const uint8_t screen_power_on_cmds[] = {
        LCDC_GPIO_SET_ACTIVE(LPM010M297B_DISP_PORT, LPM010M297B_DISP_PIN),
};

static const uint8_t screen_enable_cmds[] = {
        LCDC_EXT_CLK_SET(true),
};

static const uint8_t screen_disable_cmds[] = {
        LCDC_EXT_CLK_SET(false),
};

static const uint8_t screen_power_off_cmds[] = {
        LCDC_GPIO_SET_INACTIVE(LPM010M297B_DISP_PORT, LPM010M297B_DISP_PIN),
};

static const uint8_t screen_clear_cmds[] = {
        LCDC_JDI_CLEAR(),
};

__STATIC_INLINE void screen_set_partial_update_area(hw_lcdc_frame_t *frame)
{
        /* Minimum addressable item is one line */
        frame->startx = 0;
        frame->endx = GDI_DISP_RESX - 1;
}

#endif /* dg_configLCDC_ADAPTER */

#endif /* dg_configUSE_LPM010M297B */

#endif /* LPM010M297B_H_ */
