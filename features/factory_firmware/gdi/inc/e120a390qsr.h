/**
 ****************************************************************************************
 *
 * @file e120a390qsr.h
 *
 * @brief LCD configuration for IC RM69091F13
 *
 * Copyright (C) 2021-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef E120A390QSR_H_
#define E120A390QSR_H_

#include <stdint.h>
#include "platform_devices.h"

#if dg_configUSE_E120A390QSR

#if dg_configLCDC_ADAPTER

/*********************************************************************
 *
 *       Defines
 *
 *********************************************************************
 */
#define GDI_DISP_COLOR           (HW_LCDC_OCM_8RGB565)
#define GDI_DISP_RESX            (390)
#define GDI_DISP_RESY            (390)
#define GDI_DISP_OFFSETX         (6)
#define GDI_DISP_OFFSETY         (0)
#define GDI_LCDC_CONFIG          (&e120a390qsr_cfg)
#define GDI_USE_CONTINUOUS_MODE  (0)

#define E120A390QSR_TE_ENABLE    (0)
#define E120A390QSR_SPI4         (0)

/*
 * static const ad_io_conf_t e120a390qsr_gpio_cfg[] = {
 *         { E120A390QSR_SCL_PORT, E120A390QSR_SCL_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_CLK, true  }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  }},
 *         { E120A390QSR_SD0_PORT, E120A390QSR_SD0_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DO,  true  }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  }},
 *         { E120A390QSR_SD1_PORT, E120A390QSR_SD1_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DC,  true  }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  }},
 *         { E120A390QSR_SD2_PORT, E120A390QSR_SD2_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,         true  }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  }},
 *         { E120A390QSR_SD3_PORT, E120A390QSR_SD3_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,         true  }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  }},
 *         { E120A390QSR_RST_PORT, E120A390QSR_RST_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        true  }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  }},
 *         { E120A390QSR_CSX_PORT, E120A390QSR_CSX_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_EN,  true  }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  }},
 *         { E120A390QSR_TE_PORT,  E120A390QSR_TE_PIN,  { HW_GPIO_MODE_INPUT,  HW_GPIO_FUNC_LCD,         true  }, { HW_GPIO_MODE_INPUT,  HW_GPIO_FUNC_GPIO, true  }},
 *         { E120A390QSR_IM0_PORT, E120A390QSR_IM0_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        true  }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  }},
 *         { E120A390QSR_IM1_PORT, E120A390QSR_IM1_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false }},
 * };
 *
 * const ad_lcdc_io_conf_t e120a390qsr_io = {
 *         .voltage_level = HW_GPIO_POWER_VDD1V8P,
 *         .io_cnt = sizeof(e120a390qsr_gpio_cfg) / sizeof(e120a390qsr_gpio_cfg[0]),
 *         .io_list = e120a390qsr_gpio_cfg,
 * };
 */

static GDI_DRV_CONF_ATTR ad_lcdc_driver_conf_t e120a390qsr_drv = {
#if E120A390QSR_SPI4
        .hw_init.phy_type = HW_LCDC_PHY_MIPI_SPI4,
#else
        .hw_init.phy_type = HW_LCDC_PHY_QUAD_SPI,
#endif

        .hw_init.format = GDI_DISP_COLOR,
        .hw_init.resx = GDI_DISP_RESX,
        .hw_init.resy = GDI_DISP_RESY,
        .hw_init.cfg_extra_flags = 0,
        .hw_init.mode = HW_LCDC_MODE_DISABLE,
        .hw_init.write_freq = LCDC_FREQ_48MHz,  // Max. @50MHz
        .hw_init.read_freq = LCDC_FREQ_3_2MHz, // Max. @3.3MHz

#if E120A390QSR_SPI4
        .hw_init.iface_conf.spi.si_on_so = true,
#else
        .hw_init.iface_conf.qspi.sss_write_cmd = 0x02,
        .hw_init.iface_conf.qspi.sss_read_cmd = 0x03,
        .hw_init.iface_conf.qspi.ssq_write_cmd = 0x32,
        .hw_init.iface_conf.qspi.cmd_width = HW_LCDC_CMD_WIDTH_24,
        .hw_init.iface_conf.qspi.si_on_so = true,
#endif

        .ext_clk = HW_LCDC_EXT_CLK_OFF,
        .te_enable = E120A390QSR_TE_ENABLE,
        .te_mode = HW_LCDC_TE_POL_HIGH,
};

static const ad_lcdc_controller_conf_t e120a390qsr_cfg = {
        .io = &e120a390qsr_io,
        .drv = &e120a390qsr_drv,
};

typedef enum {
        E120A390QSR_PIXEL_FORMAT_RGB111 = 0x3,
        E120A390QSR_PIXEL_FORMAT_L8     = 0x1,
        E120A390QSR_PIXEL_FORMAT_RGB332 = 0x2,
        E120A390QSR_PIXEL_FORMAT_RGB565 = 0x55,
        E120A390QSR_PIXEL_FORMAT_RGB666 = 0x66,
        E120A390QSR_PIXEL_FORMAT_RGB888 = 0x77,
} E120A390QSR_PIXEL_FORMAT;

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
                value = E120A390QSR_PIXEL_FORMAT_RGB565;
                break;
        case HW_LCDC_OCM_8RGB666:
                value = E120A390QSR_PIXEL_FORMAT_RGB666;
                break;
        case HW_LCDC_OCM_8RGB888:
                value = E120A390QSR_PIXEL_FORMAT_RGB888;
                break;
        default:
                return false; // Unsupported color mode
        }
        hw_lcdc_dcs_cmd_params(HW_LCDC_MIPI_DCS_SET_PIXEL_FORMAT, &value, sizeof(value));

        return true;
}

static const uint8_t screen_init_cmds[] = {
#if E120A390QSR_SPI4
        LCDC_GPIO_SET_ACTIVE(E120A390QSR_IM0_PORT, E120A390QSR_IM0_PIN),      //IM[1:0] = 01 (SPI4)
        LCDC_GPIO_SET_INACTIVE(E120A390QSR_IM1_PORT, E120A390QSR_IM1_PIN),
#else
        LCDC_GPIO_SET_INACTIVE(E120A390QSR_IM0_PORT, E120A390QSR_IM0_PIN),      //IM[1:0] = 10 (QSPI)
        LCDC_GPIO_SET_ACTIVE(E120A390QSR_IM1_PORT, E120A390QSR_IM1_PIN),
#endif

        LCDC_GPIO_SET_INACTIVE(E120A390QSR_RST_PORT, E120A390QSR_RST_PIN),
        LCDC_DELAY_MS(50),
        LCDC_GPIO_SET_ACTIVE(E120A390QSR_RST_PORT, E120A390QSR_RST_PIN),
        LCDC_DELAY_MS(120),

        LCDC_MIPI_CMD_DATA(0xFE, 0x01),

        LCDC_MIPI_CMD_DATA(0x04, 0xA0),

        LCDC_MIPI_CMD_DATA(0xFE, 0x01),

        LCDC_MIPI_CMD_DATA(0x6A, 0x00),  //! Normal ELVSS = -2.4V
        LCDC_MIPI_CMD_DATA(0xAB, 0x00),  //! HBM ELVSS = -2.4V

        LCDC_MIPI_CMD_DATA(0xFE, 0x00),

        LCDC_MIPI_CMD_DATA(0xC4, 0x80),  //! Enable SPI interface write RAM
        LCDC_MIPI_CMD_DATA(HW_LCDC_MIPI_DCS_WRITE_CONTROL_DISPLAY, 0x20), //! dimming
        LCDC_MIPI_CMD_DATA(HW_LCDC_MIPI_DCS_SET_DISPLAY_BRIGHTNESS, 0xFF),


        LCDC_MIPI_SET_POSITION(GDI_DISP_OFFSETX, GDI_DISP_OFFSETY,
                GDI_DISP_RESX + GDI_DISP_OFFSETX - 1, GDI_DISP_RESY + GDI_DISP_OFFSETY - 1),
        LCDC_MIPI_SET_MODE(GDI_DISP_COLOR == HW_LCDC_OCM_8RGB565 ? E120A390QSR_PIXEL_FORMAT_RGB565 :
                           GDI_DISP_COLOR == HW_LCDC_OCM_8RGB666 ? E120A390QSR_PIXEL_FORMAT_RGB666 :
                                                                   E120A390QSR_PIXEL_FORMAT_RGB888),
#if E120A390QSR_TE_ENABLE
        LCDC_MIPI_SET_TEAR_ON(0x00),
#else
        LCDC_MIPI_CMD_DATA(HW_LCDC_MIPI_DCS_SET_TEAR_OFF),
#endif
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

#endif /* dg_configUSE_E120A390QSR */

#endif /* E120A390QSR_H_ */
