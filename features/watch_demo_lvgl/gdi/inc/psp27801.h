/**
 ****************************************************************************************
 *
 * @file psp27801.h
 *
 * @brief LCD configuration for PSP27801 LCD (c-click) with IC SSD1351
 *
 * Copyright (C) 2020-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef PSP27801_H_
#define PSP27801_H_

#include <stdint.h>
#include "platform_devices.h"

#if dg_configUSE_PSP27801

#if dg_configLCDC_ADAPTER

/*********************************************************************
 *
 *       Defines
 *
 **********************************************************************
 */
#define GDI_DISP_COLOR                  (HW_LCDC_OCM_8RGB565)
#define GDI_DISP_RESX                   (96)
#define GDI_DISP_RESY                   (96)
#define GDI_DISP_OFFSETX                (16)
#define GDI_DISP_OFFSETY                (0)
#define GDI_LCDC_CONFIG                 (&psp27801_cfg)
#define GDI_USE_CONTINUOUS_MODE         (0)
#define PSP27801_ROTATE_180D            (1)

typedef enum {
        SSD1351_CMD_SET_COLUMN_ADDRESS    = 0x15,
        SSD1351_CMD_WRITE_MEMORY_START    = 0x5C,
        SSD1351_CMD_SET_PAGE_ADDRESS      = 0x75,
        SSD1351_CMD_SET_REMAP_COLOR       = 0xA0,
        SSD1351_CMD_SET_DISP_START_LINE   = 0xA1,
        SSD1351_CMD_SET_DISP_OFFSET       = 0xA2,
        SSD1351_CMD_SET_NORMAL_DISP       = 0xA6,
        SSD1351_CMD_ENTER_INVERT_MODE     = 0xA7,
        SSD1351_CMD_NOP                   = 0xAD, //Also 0xB0. 0xD1, 0xE3
        SSD1351_CMD_SET_DISPLAY_OFF       = 0xAE,
        SSD1351_CMD_SET_DISPLAY_ON        = 0xAF,
        SSD1351_CMD_SET_PHASE_PERIOD      = 0xB1,
        SSD1351_CMD_SET_FRONT_CLK_DIV     = 0xB3,
        SSD1351_CMD_SET_VSL               = 0xB4,
        SSD1351_CMD_SET_2ND_PRECHARGE     = 0xB6,
        SSD1351_CMD_SET_VCOMH_VOLTAGE     = 0xBE,
        SSD1351_CMD_SET_CONTRAST_CUR      = 0xC1,
        SSD1351_CMD_SET_MASTER_CONTRAST   = 0xC7,
        SSD1351_CMD_SET_MUX_RATIO         = 0xCA,
        SSD1351_CMD_SET_LOCK              = 0xFD,
} SSD1351_CMD;

/*
 * static const ad_io_conf_t psp27801_gpio_cfg[] = {
 *       {PSP27801_SDI_PORT, PSP27801_SDI_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DO,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       {PSP27801_SCK_PORT, PSP27801_SCK_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_CLK, true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       {PSP27801_CS_PORT,  PSP27801_CS_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_EN,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       {PSP27801_DC_PORT,  PSP27801_DC_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DC,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       {PSP27801_EN_PORT,  PSP27801_EN_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        false}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false}},
 *       {PSP27801_RST_PORT, PSP27801_RST_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       {PSP27801_RW_PORT,  PSP27801_RW_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        false}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false}},
 * };

 * const ad_lcdc_io_conf_t psp27801_io = {
 *       .voltage_level = HW_GPIO_POWER_V33,
 *       .io_cnt = sizeof(psp27801_gpio_cfg) / sizeof(psp27801_gpio_cfg[0]),
 *       .io_list = psp27801_gpio_cfg,
 * };
 */
static const uint8_t psp27801_write_mem_start[] = { SSD1351_CMD_WRITE_MEMORY_START };

static void psp27801_set_position(const hw_lcdc_frame_t *frame)
{
        uint8_t col_data[] = {frame->startx + GDI_DISP_OFFSETX, frame->endx + GDI_DISP_OFFSETX };
        uint8_t row_data[] = {frame->starty + GDI_DISP_OFFSETY, frame->endy + GDI_DISP_OFFSETY };

        hw_lcdc_dcs_cmd_params(SSD1351_CMD_SET_COLUMN_ADDRESS, col_data, sizeof(col_data));
        hw_lcdc_dcs_cmd_params(SSD1351_CMD_SET_PAGE_ADDRESS, row_data, sizeof(row_data));
}

static GDI_DRV_CONF_ATTR ad_lcdc_driver_conf_t psp27801_drv = {
        .hw_init.phy_type = HW_LCDC_PHY_MIPI_SPI4,
        .hw_init.format = GDI_DISP_COLOR,
        .hw_init.resx = GDI_DISP_RESX,
        .hw_init.resy = GDI_DISP_RESY,
        .hw_init.cfg_extra_flags = 0,
        .hw_init.mode = HW_LCDC_MODE_DISABLE,
        .hw_init.write_freq = LCDC_FREQ_8MHz,   // Max. @4.55MHz
        .hw_init.read_freq = LCDC_FREQ_4MHz,    // Max. @4.55MHz
        .hw_init.iface_conf.spi.write_memory_cmd = psp27801_write_mem_start,
        .hw_init.iface_conf.spi.write_memory_cmd_len = ARRAY_LENGTH(psp27801_write_mem_start),
        .set_position_cb = psp27801_set_position,
        .ext_clk = HW_LCDC_EXT_CLK_OFF,
        .te_enable = false,
        .te_mode = HW_LCDC_TE_POL_LOW,
};

static const ad_lcdc_controller_conf_t psp27801_cfg = {
        .io = &psp27801_io,
        .drv = &psp27801_drv,
};

/* Required (and supported) output color modes */
static const HW_LCDC_OUTPUT_COLOR_MODE screen_color_modes[] = {
        HW_LCDC_OCM_8RGB565,
};

/* Callback function to configure display's color mode */
__STATIC_INLINE bool screen_set_color_mode(HW_LCDC_OUTPUT_COLOR_MODE color_mode)
{
        switch (color_mode) {
        case HW_LCDC_OCM_8RGB565:
                break;
        default:
                return false; // Unsupported color mode
        }

        return true;
}

static const uint8_t screen_init_cmds[] = {
        LCDC_GPIO_SET_INACTIVE(PSP27801_EN_PORT, PSP27801_EN_PIN),
        LCDC_DELAY_MS(1),
        LCDC_GPIO_SET_INACTIVE(PSP27801_RST_PORT, PSP27801_RST_PIN),
        LCDC_DELAY_US(10),
        LCDC_GPIO_SET_ACTIVE(PSP27801_RST_PORT, PSP27801_RST_PIN),
        LCDC_DELAY_US(10),
        LCDC_GPIO_SET_ACTIVE(PSP27801_EN_PORT, PSP27801_EN_PIN),

        LCDC_MIPI_CMD_DATA(SSD1351_CMD_SET_LOCK, 0x12),                                      /* Command Lock */
        LCDC_MIPI_CMD_DATA(SSD1351_CMD_SET_LOCK, 0xB1),                                      /* Command Lock */
        LCDC_MIPI_CMD_DATA(SSD1351_CMD_SET_DISPLAY_OFF),                                                     /* Display Off */
        LCDC_MIPI_CMD_DATA(SSD1351_CMD_SET_FRONT_CLK_DIV, 0xF1),                             /* Front Clock Div */
        LCDC_MIPI_CMD_DATA(SSD1351_CMD_SET_MUX_RATIO, 0x5F),                                 /* Set Mux Ratio */
        LCDC_MIPI_CMD_DATA(SSD1351_CMD_SET_REMAP_COLOR, PSP27801_ROTATE_180D ? 0x72 : 0x60),
        LCDC_MIPI_CMD_DATA(SSD1351_CMD_SET_COLUMN_ADDRESS, 0x10, 0x6F),                      /* Set Column Address */
        LCDC_MIPI_CMD_DATA(SSD1351_CMD_SET_PAGE_ADDRESS, 0x00, 0x5F),                        /* Set Row Address */
        LCDC_MIPI_CMD_DATA(SSD1351_CMD_SET_DISP_START_LINE, 0x80),                           /* Set Display Start Line */
        LCDC_MIPI_CMD_DATA(SSD1351_CMD_SET_DISP_OFFSET, PSP27801_ROTATE_180D ? 0x20 : 0x60), /* Set Display Offset */
        LCDC_MIPI_CMD_DATA(SSD1351_CMD_SET_PHASE_PERIOD, 0x32),                              /* Set Phase Length */
        LCDC_MIPI_CMD_DATA(SSD1351_CMD_SET_VCOMH_VOLTAGE, 0x05),                             /* Set VComH Voltage */
        LCDC_MIPI_CMD_DATA(SSD1351_CMD_SET_NORMAL_DISP),                                     /* Set Display Mode Reset */
        LCDC_MIPI_CMD_DATA(SSD1351_CMD_SET_CONTRAST_CUR, 0x8A, 0x51, 0x8A),                  /* Set Contrast */
        LCDC_MIPI_CMD_DATA(SSD1351_CMD_SET_MASTER_CONTRAST, 0xCF),                           /* Set Master Contrast */
        LCDC_MIPI_CMD_DATA(SSD1351_CMD_SET_VSL, 0xA0, 0xB5, 0x55),                           /* Set Segment Low Voltage */
        LCDC_MIPI_CMD_DATA(SSD1351_CMD_SET_2ND_PRECHARGE, 0x01),                             /* Set Second Precharge Period */

        LCDC_MIPI_CMD_DATA(SSD1351_CMD_SET_DISPLAY_ON),                                      /* Set Sleep Mode Display On */
};

static const uint8_t screen_power_on_cmds[] = {
        LCDC_GPIO_SET_ACTIVE(PSP27801_EN_PORT, PSP27801_EN_PIN)
};

static const uint8_t screen_enable_cmds[] = {
        LCDC_MIPI_CMD_DATA(SSD1351_CMD_SET_DISPLAY_ON),                                                      /* Set Sleep Mode Display On */
};

static const uint8_t screen_disable_cmds[] = {
        LCDC_MIPI_CMD_DATA(SSD1351_CMD_SET_DISPLAY_OFF),                                                     /* Display off */
};

static const uint8_t screen_power_off_cmds[] = {
        LCDC_GPIO_SET_INACTIVE(PSP27801_EN_PORT, PSP27801_EN_PIN)
};

static const uint8_t screen_clear_cmds[] = {
};

__STATIC_INLINE void screen_set_partial_update_area(hw_lcdc_frame_t *frame)
{
        UNUSED_ARG(frame); //! Just to suppress compiler warnings
}

#endif /* dg_configLCDC_ADAPTER */

#endif /* dg_configUSE_PSP27801 */

#endif /* PSP27801_H_ */
