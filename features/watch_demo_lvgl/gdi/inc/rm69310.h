/**
 ****************************************************************************************
 *
 * @file rm69310.h
 *
 * @brief LCD configuration for LCD with IC RM69310
 *
 * Copyright (C) 2020-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef RM69310_H_
#define RM69310_H_

#include "stdint.h"
#include "platform_devices.h"

#if dg_configUSE_RM69310

#if dg_configLCDC_ADAPTER

/*********************************************************************
 *
 *       Defines
 *
 *********************************************************************
 */
#define GDI_DISP_COLOR                  (HW_LCDC_OCM_8RGB888)
#define GDI_DISP_RESX                   (120)
#define GDI_DISP_RESY                   (240)
#define GDI_DISP_OFFSETX                (0)
#define GDI_DISP_OFFSETY                (0)
#define GDI_LCDC_CONFIG                 (&rm69310_cfg)
#define GDI_USE_CONTINUOUS_MODE         (0)
#define GDI_PWMLED_BACKLIGHT            (1)
#define GDI_PWMLED_DUTY_CYCLE_1         (60)
#define GDI_PWMLED_DUTY_CYCLE_2         (60)

static GDI_DRV_CONF_ATTR ad_lcdc_driver_conf_t rm69310_drv = {
        .hw_init.phy_type = HW_LCDC_PHY_MIPI_SPI4,
        .hw_init.format = GDI_DISP_COLOR,
        .hw_init.resx = GDI_DISP_RESX,
        .hw_init.resy = GDI_DISP_RESY,
        .hw_init.cfg_extra_flags = 0,
        .hw_init.mode = HW_LCDC_MODE_DISABLE,
        .hw_init.write_freq = LCDC_FREQ_48MHz,
        .ext_clk = HW_LCDC_EXT_CLK_OFF,
        .te_enable = true,
        .te_mode = HW_LCDC_TE_POL_HIGH,
};

static const ad_lcdc_controller_conf_t rm69310_cfg = {
        .io = &rm69310_io,
        .drv = &rm69310_drv,
};

typedef enum {
        RM69310_PIXEL_FORMAT_RGB111 = 0x3,
        RM69310_PIXEL_FORMAT_L8     = 0x1,
        RM69310_PIXEL_FORMAT_RGB332 = 0x2,
        RM69310_PIXEL_FORMAT_RGB565 = 0x5,
        RM69310_PIXEL_FORMAT_RGB666 = 0x6,
        RM69310_PIXEL_FORMAT_RGB888 = 0x7,
} RM69310_PIXEL_FORMAT;

/* Required (and supported) output color modes */
static const HW_LCDC_OUTPUT_COLOR_MODE screen_color_modes[] = {
        HW_LCDC_OCM_8RGB111_1,
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
                value = RM69310_PIXEL_FORMAT_RGB111;
                break;
        case HW_LCDC_OCM_8RGB332:
                value = RM69310_PIXEL_FORMAT_RGB332;
                break;
        case HW_LCDC_OCM_8RGB565:
                value = RM69310_PIXEL_FORMAT_RGB565;
                break;
        case HW_LCDC_OCM_8RGB666:
                value = RM69310_PIXEL_FORMAT_RGB666;
                break;
        case HW_LCDC_OCM_8RGB888:
                value = RM69310_PIXEL_FORMAT_RGB888;
                break;
        default:
                return false; // Unsupported color mode
        }
        hw_lcdc_dcs_cmd_params(HW_LCDC_MIPI_DCS_SET_PIXEL_FORMAT, &value, sizeof(value));

        return true;
}

static const uint8_t screen_init_cmds[] = {
        /* LCD Rest */
//        LCDC_GPIO_SET_ACTIVE(RM69310_RST_PORT, RM69310_RST_PIN),
//        LCDC_DELAY_MS(200),
//        LCDC_GPIO_SET_INACTIVE(RM69310_RST_PORT, RM69310_RST_PIN),
//        LCDC_DELAY_MS(800),
//        LCDC_GPIO_SET_ACTIVE(RM69310_RST_PORT, RM69310_RST_PIN),
        LCDC_DELAY_MS(800),

        LCDC_MIPI_CMD_DATA(0x02, 0x00),

        //CMD2 P0
        LCDC_MIPI_CMD_DATA(0xFE, 0x01),

        //SPIWRAM, enable spi_wram
        LCDC_MIPI_CMD_DATA(0x04, 0xa0),

        //CGMCTR, resolution, 128RGB x XXX
        LCDC_MIPI_CMD_DATA(0x05, 0x70),

        //DPRSLCTR, resolution, XXX x 240
        LCDC_MIPI_CMD_DATA(0x06, 0x3C),

        //LFRCTR, Low Frame Rate Control Normal Mode
        //GMA2_EN_A = 0, D_CM_A = 0x00(24bit color normal mode)
        //PSEL_A = 0x01(pixel clock normal mode)
        //T1A    = 0x357(display time of one scan line normal mode)
        //VBPDA  = 0x12(vertical back porch lines normal mode)
        //VFPDF  = 0x12(vertical front porch lines normal mode)
        LCDC_MIPI_CMD_DATA(0x25, 0x06),
        LCDC_MIPI_CMD_DATA(0x26, 0x57),
        LCDC_MIPI_CMD_DATA(0x27, 0x12),
        LCDC_MIPI_CMD_DATA(0x28, 0x12),

        //LFRCTRIDM, Low Frame Rate Control Idle Mode
        //GMA2_EN_A = 0, D_CM_A = 0x00(24bit color normal mode)
        //PSEL_A = 0x01(pixel clock normal mode)
        //T1A    = 0x357(display time of one scan line normal mode)
        //VBPDA  = 0x12(vertical back porch lines normal mode)
        //VFPDF  = 0x12(vertical front porch lines normal mode)
        LCDC_MIPI_CMD_DATA(0x2A, 0x06),
        LCDC_MIPI_CMD_DATA(0x2B, 0x57),
        LCDC_MIPI_CMD_DATA(0x2D, 0x12),
        LCDC_MIPI_CMD_DATA(0x2F, 0x12),

        LCDC_MIPI_CMD_DATA(0x37, 0x0C),

        LCDC_MIPI_CMD_DATA(0x6D, 0x18),

        LCDC_MIPI_CMD_DATA(0x29, 0x01),

        LCDC_MIPI_CMD_DATA(0x30, 0x41),

        //SWTIMCTR1, Switch Timing Control
        LCDC_MIPI_CMD_DATA(0x3A, 0x1D),
        LCDC_MIPI_CMD_DATA(0x3B, 0x00),
        LCDC_MIPI_CMD_DATA(0x3D, 0x16),
        LCDC_MIPI_CMD_DATA(0x3F, 0x2D),
        LCDC_MIPI_CMD_DATA(0x40, 0x14),
        LCDC_MIPI_CMD_DATA(0x41, 0x0D),

        //SWSELCTR, Switch Output Selection
        LCDC_MIPI_CMD_DATA(0x42, 0x63),
        LCDC_MIPI_CMD_DATA(0x43, 0x36),
        LCDC_MIPI_CMD_DATA(0x44, 0x41),
        LCDC_MIPI_CMD_DATA(0x45, 0x14),
        LCDC_MIPI_CMD_DATA(0x46, 0x52),
        LCDC_MIPI_CMD_DATA(0x47, 0x25),
        LCDC_MIPI_CMD_DATA(0x48, 0x63),
        LCDC_MIPI_CMD_DATA(0x49, 0x36),
        LCDC_MIPI_CMD_DATA(0x4A, 0x41),
        LCDC_MIPI_CMD_DATA(0x4B, 0x14),
        LCDC_MIPI_CMD_DATA(0x4C, 0x52),
        LCDC_MIPI_CMD_DATA(0x4D, 0x25),

        //SDSELCTR, Source Data Output Selection
        LCDC_MIPI_CMD_DATA(0x4E, 0x63),
        LCDC_MIPI_CMD_DATA(0x4F, 0x36),
        LCDC_MIPI_CMD_DATA(0x50, 0x41),
        LCDC_MIPI_CMD_DATA(0x51, 0x14),
        LCDC_MIPI_CMD_DATA(0x52, 0x52),
        LCDC_MIPI_CMD_DATA(0x53, 0x25),
        LCDC_MIPI_CMD_DATA(0x54, 0x63),
        LCDC_MIPI_CMD_DATA(0x55, 0x36),
        LCDC_MIPI_CMD_DATA(0x56, 0x41),
        LCDC_MIPI_CMD_DATA(0x57, 0x14),
        LCDC_MIPI_CMD_DATA(0x58, 0x52),
        LCDC_MIPI_CMD_DATA(0x59, 0x25),

        LCDC_MIPI_CMD_DATA(0x66, 0x10),

        LCDC_MIPI_CMD_DATA(0x67, 0x40),

        LCDC_MIPI_CMD_DATA(0x70, 0xA5),

        LCDC_MIPI_CMD_DATA(0x72, 0x1A),

        LCDC_MIPI_CMD_DATA(0x73, 0x15),

        LCDC_MIPI_CMD_DATA(0x74, 0x0C),

        //OVSSDEF, OVSS Control
        LCDC_MIPI_CMD_DATA(0x6A, 0x1F),

        //CMD2 P3, GOA Timing Control
        LCDC_MIPI_CMD_DATA(0xFE, 0x04),

        LCDC_MIPI_CMD_DATA(0x00, 0xDC),
        LCDC_MIPI_CMD_DATA(0x01, 0x00),
        LCDC_MIPI_CMD_DATA(0x02, 0x02),
        LCDC_MIPI_CMD_DATA(0x03, 0x00),
        LCDC_MIPI_CMD_DATA(0x04, 0x00),
        LCDC_MIPI_CMD_DATA(0x05, 0x01),
        LCDC_MIPI_CMD_DATA(0x06, 0x09),
        LCDC_MIPI_CMD_DATA(0x07, 0x0A),
        LCDC_MIPI_CMD_DATA(0x08, 0x00),
        LCDC_MIPI_CMD_DATA(0x09, 0xDC),
        LCDC_MIPI_CMD_DATA(0x0A, 0x00),
        LCDC_MIPI_CMD_DATA(0x0B, 0x02),
        LCDC_MIPI_CMD_DATA(0x0C, 0x00),
        LCDC_MIPI_CMD_DATA(0x0D, 0x00),
        LCDC_MIPI_CMD_DATA(0x0E, 0x00),
        LCDC_MIPI_CMD_DATA(0x0F, 0x09),
        LCDC_MIPI_CMD_DATA(0x10, 0x0A),
        LCDC_MIPI_CMD_DATA(0x11, 0x00),
        LCDC_MIPI_CMD_DATA(0x12, 0xDC),
        LCDC_MIPI_CMD_DATA(0x13, 0x00),
        LCDC_MIPI_CMD_DATA(0x14, 0x02),
        LCDC_MIPI_CMD_DATA(0x15, 0x00),
        LCDC_MIPI_CMD_DATA(0x16, 0x08),
        LCDC_MIPI_CMD_DATA(0x17, 0x01),
        LCDC_MIPI_CMD_DATA(0x18, 0xA3),
        LCDC_MIPI_CMD_DATA(0x19, 0x00),
        LCDC_MIPI_CMD_DATA(0x1A, 0x00),
        LCDC_MIPI_CMD_DATA(0x1B, 0xDC),
        LCDC_MIPI_CMD_DATA(0x1C, 0x00),
        LCDC_MIPI_CMD_DATA(0x1D, 0x02),
        LCDC_MIPI_CMD_DATA(0x1E, 0x00),
        LCDC_MIPI_CMD_DATA(0x1F, 0x08),
        LCDC_MIPI_CMD_DATA(0x20, 0x00),
        LCDC_MIPI_CMD_DATA(0x21, 0xA3),
        LCDC_MIPI_CMD_DATA(0x22, 0x00),
        LCDC_MIPI_CMD_DATA(0x23, 0x00),
        LCDC_MIPI_CMD_DATA(0x4C, 0x89),
        LCDC_MIPI_CMD_DATA(0x4D, 0x00),
        LCDC_MIPI_CMD_DATA(0x4E, 0x01),
        LCDC_MIPI_CMD_DATA(0x4F, 0x08),
        LCDC_MIPI_CMD_DATA(0x50, 0x01),
        LCDC_MIPI_CMD_DATA(0x51, 0x85),
        LCDC_MIPI_CMD_DATA(0x52, 0x7C),
        LCDC_MIPI_CMD_DATA(0x53, 0x8A),
        LCDC_MIPI_CMD_DATA(0x54, 0x50),
        LCDC_MIPI_CMD_DATA(0x55, 0x02),
        LCDC_MIPI_CMD_DATA(0x56, 0x48),
        LCDC_MIPI_CMD_DATA(0x58, 0x34),
        LCDC_MIPI_CMD_DATA(0x59, 0x00),
        LCDC_MIPI_CMD_DATA(0x5E, 0xBB),
        LCDC_MIPI_CMD_DATA(0x5F, 0xBB),
        LCDC_MIPI_CMD_DATA(0x60, 0x09),
        LCDC_MIPI_CMD_DATA(0x61, 0xB1),
        LCDC_MIPI_CMD_DATA(0x62, 0xBB),
        LCDC_MIPI_CMD_DATA(0x65, 0x05),
        LCDC_MIPI_CMD_DATA(0x66, 0x04),
        LCDC_MIPI_CMD_DATA(0x67, 0x00),
        LCDC_MIPI_CMD_DATA(0x78, 0xBB),
        LCDC_MIPI_CMD_DATA(0x79, 0x8B),
        LCDC_MIPI_CMD_DATA(0x7A, 0x32),

        //CMD2 P0, Display Options + Power
        LCDC_MIPI_CMD_DATA(0xFE, 0x01),

        //BT1CTR
        LCDC_MIPI_CMD_DATA(0x0E, 0x85),

        //BT1CTRIDM
        LCDC_MIPI_CMD_DATA(0x0F, 0x85),

        //BT1BT3REGEN
        LCDC_MIPI_CMD_DATA(0x10, 0x11),

        //BT3CTR
        LCDC_MIPI_CMD_DATA(0x11, 0xA0),

        //BT3CTRIDM
        LCDC_MIPI_CMD_DATA(0x12, 0xA0),

        //BT4CTR
        LCDC_MIPI_CMD_DATA(0x13, 0x81),

        //BT4CTRIDM
        LCDC_MIPI_CMD_DATA(0x14, 0x81),

        //BT5CTR
        LCDC_MIPI_CMD_DATA(0x15, 0x82),

        //BT5CTRIDM
        LCDC_MIPI_CMD_DATA(0x16, 0x82),

        //SETVGHR
        LCDC_MIPI_CMD_DATA(0x18, 0x55),

        //SETVGLR
        LCDC_MIPI_CMD_DATA(0x19, 0x33),

        //SWEQCTR
        LCDC_MIPI_CMD_DATA(0x1E, 0x02),

        //SDSELCTR5
        LCDC_MIPI_CMD_DATA(0x5B, 0x10),

        //SDSELCTR10
        LCDC_MIPI_CMD_DATA(0x62, 0x15),

        //SDSELCTR11
        LCDC_MIPI_CMD_DATA(0x63, 0x15),

        LCDC_MIPI_CMD_DATA(0x6A, 0x1f),

        LCDC_MIPI_CMD_DATA(0x70, 0x55),

        //SWEQCTR
        LCDC_MIPI_CMD_DATA(0x1D, 0x02),

        //VGMPTR
        LCDC_MIPI_CMD_DATA(0x89, 0xF8),

        //VGSPTR
        LCDC_MIPI_CMD_DATA(0x8A, 0x80),

        //VGMSPTR
        LCDC_MIPI_CMD_DATA(0x8B, 0x01),

        //CMD1
        LCDC_MIPI_CMD_DATA(0xFE, 0x00),


        //CASET, Set Column Start Address
        //SC=0x04, EC=0x7B
        LCDC_MIPI_CMD_DATA(HW_LCDC_MIPI_DCS_SET_COLUMN_ADDRESS, 0x00, 0x04, 0x00, 0x7B),

        //TEON, Tearing effect line on
        //TELOM=0x1, V-blanking +H-blanking
        LCDC_MIPI_SET_TEAR_ON(0x00),

        //COLMOD, interface pixel format
        LCDC_MIPI_SET_MODE(GDI_DISP_COLOR == HW_LCDC_OCM_8RGB111_1 ? RM69310_PIXEL_FORMAT_RGB111 :
                           GDI_DISP_COLOR == HW_LCDC_OCM_8RGB332   ? RM69310_PIXEL_FORMAT_RGB332 :
                           GDI_DISP_COLOR == HW_LCDC_OCM_8RGB565   ? RM69310_PIXEL_FORMAT_RGB565 :
                           GDI_DISP_COLOR == HW_LCDC_OCM_8RGB666   ? RM69310_PIXEL_FORMAT_RGB666 :
                                                                     RM69310_PIXEL_FORMAT_RGB888),
};

static const uint8_t screen_power_on_cmds[] = {
};

static const uint8_t screen_enable_cmds[] = {
        //SLPOUT, sleep out
        LCDC_MIPI_CMD_DATA(HW_LCDC_MIPI_DCS_EXIT_SLEEP_MODE),
        LCDC_DELAY_MS(244),
        //DISPON, display on
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
        /* Add restrictions imposed by the RM69310 IC */
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

#endif /* dg_configUSE_RM69310 */

#endif /* RM69310_H_ */
