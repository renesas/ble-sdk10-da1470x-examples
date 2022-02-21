/**
 ****************************************************************************************
 *
 * @file mrb3973_dbib.h
 *
 * @brief LCD configuration for MRB3973 with IC NT35510
 *
 * Copyright (C) 2020-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef MRB3973_DBIB_H_
#define MRB3973_DBIB_H_

#include <stdint.h>
#include "platform_devices.h"

#if dg_configUSE_MRB3973_DBIB

#if dg_configLCDC_ADAPTER

#define MRB3973_DBIB_LANDSCAPE           (1)

/*********************************************************************
 *
 *       Defines
 *
 *********************************************************************
 */
#define GDI_DISP_COLOR           (HW_LCDC_OCM_8RGB888)
#if MRB3973_DBIB_LANDSCAPE
#define GDI_DISP_RESX            (800)
#define GDI_DISP_RESY            (480)
#else
#define GDI_DISP_RESX            (480)
#define GDI_DISP_RESY            (800)
#endif /* MRB3973_DBIB_LANDSCAPE */
#define GDI_DISP_OFFSETX         (0)
#define GDI_DISP_OFFSETY         (0)
#define GDI_LCDC_CONFIG          (&mrb3973_dbib_cfg)
#define GDI_USE_CONTINUOUS_MODE  (0)

#define LCDC_MIPI_CMD16(cmd)                     LCDC_MIPI_CMD(((cmd) >> 8) & 0xFF),  \
                                                 LCDC_MIPI_CMD((cmd) & 0xFF)

#define LCDC_CMD_DATA16(cmd, data)               LCDC_GEN_CMD_DATA(2, ((cmd) >> 8) & 0xFF, (cmd) & 0xFF, ((data) >> 8) & 0xFF, (data) & 0xFF)

#define LCDC_MIPI_SET_POSITION16(sx, sy, ex, ey) LCDC_CMD_DATA16((HW_LCDC_MIPI_DCS_SET_COLUMN_ADDRESS << 8) + 0, ((sx) >> 8) & 0xFF), \
                                                 LCDC_CMD_DATA16((HW_LCDC_MIPI_DCS_SET_COLUMN_ADDRESS << 8) + 1, (sx) & 0xFF),        \
                                                 LCDC_CMD_DATA16((HW_LCDC_MIPI_DCS_SET_COLUMN_ADDRESS << 8) + 2, ((ex) >> 8) & 0xFF), \
                                                 LCDC_CMD_DATA16((HW_LCDC_MIPI_DCS_SET_COLUMN_ADDRESS << 8) + 3, (ex) & 0xFF),        \
                                                 LCDC_CMD_DATA16((HW_LCDC_MIPI_DCS_SET_PAGE_ADDRESS << 8) + 0, ((sy) >> 8) & 0xFF),   \
                                                 LCDC_CMD_DATA16((HW_LCDC_MIPI_DCS_SET_PAGE_ADDRESS << 8) + 1, (sy) & 0xFF),          \
                                                 LCDC_CMD_DATA16((HW_LCDC_MIPI_DCS_SET_PAGE_ADDRESS << 8) + 2, ((ey) >> 8) & 0xFF),   \
                                                 LCDC_CMD_DATA16((HW_LCDC_MIPI_DCS_SET_PAGE_ADDRESS << 8) + 3, (ey) & 0xFF)
#define LCDC_MIPI_SET_TEAR_ON16(mode)            LCDC_CMD_DATA16((HW_LCDC_MIPI_DCS_SET_TEAR_ON << 8), mode)
#define LCDC_MIPI_SET_MODE16(mode)               LCDC_CMD_DATA16((HW_LCDC_MIPI_DCS_SET_PIXEL_FORMAT << 8), mode)


static const uint8_t mrb3973_dbib_write_memory_cmd[] = {HW_LCDC_MIPI_DCS_WRITE_MEMORY_START, 0x00};

static void mrb3973_dbib_mipi_set_position(const hw_lcdc_frame_t *frame)
{
        uint8_t col_cmd0[] = { HW_LCDC_MIPI_DCS_SET_COLUMN_ADDRESS, 0x00 }; uint8_t col_data0[] = { 0x00, frame->startx >> 8 };
        uint8_t col_cmd1[] = { HW_LCDC_MIPI_DCS_SET_COLUMN_ADDRESS, 0x01 }; uint8_t col_data1[] = { 0x00, frame->startx & 0xFF };
        uint8_t col_cmd2[] = { HW_LCDC_MIPI_DCS_SET_COLUMN_ADDRESS, 0x02 }; uint8_t col_data2[] = { 0x00, frame->endx >> 8 };
        uint8_t col_cmd3[] = { HW_LCDC_MIPI_DCS_SET_COLUMN_ADDRESS, 0x03 }; uint8_t col_data3[] = { 0x00, frame->endx & 0xFF} ;

        uint8_t pag_cmd0[] = { HW_LCDC_MIPI_DCS_SET_PAGE_ADDRESS, 0x00 };   uint8_t pag_data0[] = { 0x00, frame->starty >> 8 };
        uint8_t pag_cmd1[] = { HW_LCDC_MIPI_DCS_SET_PAGE_ADDRESS, 0x01 };   uint8_t pag_data1[] = { 0x00, frame->starty & 0xFF };
        uint8_t pag_cmd2[] = { HW_LCDC_MIPI_DCS_SET_PAGE_ADDRESS, 0x02 };   uint8_t pag_data2[] = { 0x00, frame->endy >> 8 };
        uint8_t pag_cmd3[] = { HW_LCDC_MIPI_DCS_SET_PAGE_ADDRESS, 0x03 };   uint8_t pag_data3[] = { 0x00, frame->endy & 0xFF };

        hw_lcdc_gen_cmd_params(col_cmd0, sizeof(col_cmd0), col_data0, sizeof(col_data0));
        hw_lcdc_gen_cmd_params(col_cmd1, sizeof(col_cmd1), col_data1, sizeof(col_data1));
        hw_lcdc_gen_cmd_params(col_cmd2, sizeof(col_cmd2), col_data2, sizeof(col_data2));
        hw_lcdc_gen_cmd_params(col_cmd3, sizeof(col_cmd3), col_data3, sizeof(col_data3));

        hw_lcdc_gen_cmd_params(pag_cmd0, sizeof(pag_cmd0), pag_data0, sizeof(pag_data0));
        hw_lcdc_gen_cmd_params(pag_cmd1, sizeof(pag_cmd1), pag_data1, sizeof(pag_data1));
        hw_lcdc_gen_cmd_params(pag_cmd2, sizeof(pag_cmd2), pag_data2, sizeof(pag_data2));
        hw_lcdc_gen_cmd_params(pag_cmd3, sizeof(pag_cmd3), pag_data3, sizeof(pag_data3));
}

static GDI_DRV_CONF_ATTR ad_lcdc_driver_conf_t mrb3973_dbib_drv = {
        .hw_init.phy_type = HW_LCDC_PHY_MIPI_DBIB,
        .hw_init.format = GDI_DISP_COLOR,
        .hw_init.resx = GDI_DISP_RESX,
        .hw_init.resy = GDI_DISP_RESY,
        .hw_init.cfg_extra_flags = 0,
        .hw_init.mode = HW_LCDC_MODE_DISABLE,
        .hw_init.write_freq = LCDC_FREQ_24MHz, // Max. @30.3MHz
        .hw_init.read_freq = LCDC_FREQ_2MHz,  // Max. @2.5MHz (frame), @6.25MHz (ID)
        .hw_init.dither = HW_LCDC_DITHER_OFF,
        .hw_init.iface_conf.dbib.write_memory_cmd = mrb3973_dbib_write_memory_cmd,
        .hw_init.iface_conf.dbib.write_memory_cmd_len = sizeof(mrb3973_dbib_write_memory_cmd),
#if (DEVICE_REVISION == DEVICE_REV_A)
        .palette_lut = NULL,
#endif
        .set_position_cb = mrb3973_dbib_mipi_set_position,
        .ext_clk = HW_LCDC_EXT_CLK_OFF,
        .te_enable = false,
        .te_mode = HW_LCDC_TE_POL_HIGH,
};

static const ad_lcdc_controller_conf_t mrb3973_dbib_cfg = {
        .io = &mrb3973_dbib_io,
        .drv = &mrb3973_dbib_drv,
};

typedef enum {
        MRB3973_DBIB_PIXEL_FORMAT_RGB565 = 0x5,
        MRB3973_DBIB_PIXEL_FORMAT_RGB666 = 0x6,
        MRB3973_DBIB_PIXEL_FORMAT_RGB888 = 0x7,
} MRB3973_DBIB_PIXEL_FORMAT;

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
                value = MRB3973_DBIB_PIXEL_FORMAT_RGB565;
                break;
        case HW_LCDC_OCM_8RGB666:
                value = MRB3973_DBIB_PIXEL_FORMAT_RGB666;
                break;
        case HW_LCDC_OCM_8RGB888:
                value = MRB3973_DBIB_PIXEL_FORMAT_RGB888;
                break;
        default:
                return false; // Unsupported color mode
        }

        uint8_t cmd[] = {HW_LCDC_MIPI_DCS_SET_PIXEL_FORMAT, 0x00};
        uint8_t data[] = { 0x00, value };

        hw_lcdc_gen_cmd_params(cmd, sizeof(cmd), data, sizeof(data));

        return true;
}

static const uint8_t screen_init_cmds[] = {
        LCDC_GPIO_SET_INACTIVE(MRB3973_DBIB_RESX_PORT, MRB3973_DBIB_RESX_PIN),
        LCDC_DELAY_MS(50),
        LCDC_GPIO_SET_ACTIVE(MRB3973_DBIB_RESX_PORT, MRB3973_DBIB_RESX_PIN),
        LCDC_DELAY_MS(50),

        LCDC_CMD_DATA16(0xF000, 0x55),
        LCDC_CMD_DATA16(0xF001, 0xAA),
        LCDC_CMD_DATA16(0xF002, 0x52),
        LCDC_CMD_DATA16(0xF003, 0x08),
        LCDC_CMD_DATA16(0xF004, 0x01),

        //# AVDD: manual,
        LCDC_CMD_DATA16(0xB600, 0x34),
        LCDC_CMD_DATA16(0xB601, 0x34),
        LCDC_CMD_DATA16(0xB602, 0x34),

        LCDC_CMD_DATA16(0xB000, 0x0D), //09
        LCDC_CMD_DATA16(0xB001, 0x0D),
        LCDC_CMD_DATA16(0xB002, 0x0D),
        //# AVEE: manual,  -6V
        LCDC_CMD_DATA16(0xB700, 0x24),
        LCDC_CMD_DATA16(0xB701, 0x24),
        LCDC_CMD_DATA16(0xB702, 0x24),

        LCDC_CMD_DATA16(0xB100, 0x0D),
        LCDC_CMD_DATA16(0xB101, 0x0D),
        LCDC_CMD_DATA16(0xB102, 0x0D),
        //#Power Control for
        //VCL
        LCDC_CMD_DATA16(0xB800, 0x24),
        LCDC_CMD_DATA16(0xB801, 0x24),
        LCDC_CMD_DATA16(0xB802, 0x24),

        LCDC_CMD_DATA16(0xB200, 0x00),

        //# VGH: Clamp Enable,
        LCDC_CMD_DATA16(0xB900, 0x24),
        LCDC_CMD_DATA16(0xB901, 0x24),
        LCDC_CMD_DATA16(0xB902, 0x24),

        LCDC_CMD_DATA16(0xB300, 0x05),
        LCDC_CMD_DATA16(0xB301, 0x05),
        LCDC_CMD_DATA16(0xB302, 0x05),

        ///LCDC_CMD_DATA16(0xBF00, 0x01),

        //# VGL(LVGL):
        LCDC_CMD_DATA16(0xBA00, 0x34),
        LCDC_CMD_DATA16(0xBA01, 0x34),
        LCDC_CMD_DATA16(0xBA02, 0x34),
        //# VGL_REG(VGLO)
        LCDC_CMD_DATA16(0xB500, 0x0B),
        LCDC_CMD_DATA16(0xB501, 0x0B),
        LCDC_CMD_DATA16(0xB502, 0x0B),
        //# VGMP/VGSP:
        LCDC_CMD_DATA16(0xBC00, 0X00),
        LCDC_CMD_DATA16(0xBC01, 0xA3),
        LCDC_CMD_DATA16(0xBC02, 0X00),
        //# VGMN/VGSN
        LCDC_CMD_DATA16(0xBD00, 0x00),
        LCDC_CMD_DATA16(0xBD01, 0xA3),
        LCDC_CMD_DATA16(0xBD02, 0x00),
        //# VCOM=-0.1
        LCDC_CMD_DATA16(0xBE00, 0x00),
        LCDC_CMD_DATA16(0xBE01, 0x63),                //4f
        //  VCOMH+0x01,
        //#R+
        LCDC_CMD_DATA16(0xD100, 0x00),
        LCDC_CMD_DATA16(0xD101, 0x37),
        LCDC_CMD_DATA16(0xD102, 0x00),
        LCDC_CMD_DATA16(0xD103, 0x52),
        LCDC_CMD_DATA16(0xD104, 0x00),
        LCDC_CMD_DATA16(0xD105, 0x7B),
        LCDC_CMD_DATA16(0xD106, 0x00),
        LCDC_CMD_DATA16(0xD107, 0x99),
        LCDC_CMD_DATA16(0xD108, 0x00),
        LCDC_CMD_DATA16(0xD109, 0xB1),
        LCDC_CMD_DATA16(0xD10A, 0x00),
        LCDC_CMD_DATA16(0xD10B, 0xD2),
        LCDC_CMD_DATA16(0xD10C, 0x00),
        LCDC_CMD_DATA16(0xD10D, 0xF6),
        LCDC_CMD_DATA16(0xD10E, 0x01),
        LCDC_CMD_DATA16(0xD10F, 0x27),
        LCDC_CMD_DATA16(0xD110, 0x01),
        LCDC_CMD_DATA16(0xD111, 0x4E),
        LCDC_CMD_DATA16(0xD112, 0x01),
        LCDC_CMD_DATA16(0xD113, 0x8C),
        LCDC_CMD_DATA16(0xD114, 0x01),
        LCDC_CMD_DATA16(0xD115, 0xBE),
        LCDC_CMD_DATA16(0xD116, 0x02),
        LCDC_CMD_DATA16(0xD117, 0x0B),
        LCDC_CMD_DATA16(0xD118, 0x02),
        LCDC_CMD_DATA16(0xD119, 0x48),
        LCDC_CMD_DATA16(0xD11A, 0x02),
        LCDC_CMD_DATA16(0xD11B, 0x4A),
        LCDC_CMD_DATA16(0xD11C, 0x02),
        LCDC_CMD_DATA16(0xD11D, 0x7E),
        LCDC_CMD_DATA16(0xD11E, 0x02),
        LCDC_CMD_DATA16(0xD11F, 0xBC),
        LCDC_CMD_DATA16(0xD120, 0x02),
        LCDC_CMD_DATA16(0xD121, 0xE1),
        LCDC_CMD_DATA16(0xD122, 0x03),
        LCDC_CMD_DATA16(0xD123, 0x10),
        LCDC_CMD_DATA16(0xD124, 0x03),
        LCDC_CMD_DATA16(0xD125, 0x31),
        LCDC_CMD_DATA16(0xD126, 0x03),
        LCDC_CMD_DATA16(0xD127, 0x5A),
        LCDC_CMD_DATA16(0xD128, 0x03),
        LCDC_CMD_DATA16(0xD129, 0x73),
        LCDC_CMD_DATA16(0xD12A, 0x03),
        LCDC_CMD_DATA16(0xD12B, 0x94),
        LCDC_CMD_DATA16(0xD12C, 0x03),
        LCDC_CMD_DATA16(0xD12D, 0x9F),
        LCDC_CMD_DATA16(0xD12E, 0x03),
        LCDC_CMD_DATA16(0xD12F, 0xB3),
        LCDC_CMD_DATA16(0xD130, 0x03),
        LCDC_CMD_DATA16(0xD131, 0xB9),
        LCDC_CMD_DATA16(0xD132, 0x03),
        LCDC_CMD_DATA16(0xD133, 0xC1),
        //#G+
        LCDC_CMD_DATA16(0xD200, 0x00),
        LCDC_CMD_DATA16(0xD201, 0x37),
        LCDC_CMD_DATA16(0xD202, 0x00),
        LCDC_CMD_DATA16(0xD203, 0x52),
        LCDC_CMD_DATA16(0xD204, 0x00),
        LCDC_CMD_DATA16(0xD205, 0x7B),
        LCDC_CMD_DATA16(0xD206, 0x00),
        LCDC_CMD_DATA16(0xD207, 0x99),
        LCDC_CMD_DATA16(0xD208, 0x00),
        LCDC_CMD_DATA16(0xD209, 0xB1),
        LCDC_CMD_DATA16(0xD20A, 0x00),
        LCDC_CMD_DATA16(0xD20B, 0xD2),
        LCDC_CMD_DATA16(0xD20C, 0x00),
        LCDC_CMD_DATA16(0xD20D, 0xF6),
        LCDC_CMD_DATA16(0xD20E, 0x01),
        LCDC_CMD_DATA16(0xD20F, 0x27),
        LCDC_CMD_DATA16(0xD210, 0x01),
        LCDC_CMD_DATA16(0xD211, 0x4E),
        LCDC_CMD_DATA16(0xD212, 0x01),
        LCDC_CMD_DATA16(0xD213, 0x8C),
        LCDC_CMD_DATA16(0xD214, 0x01),
        LCDC_CMD_DATA16(0xD215, 0xBE),
        LCDC_CMD_DATA16(0xD216, 0x02),
        LCDC_CMD_DATA16(0xD217, 0x0B),
        LCDC_CMD_DATA16(0xD218, 0x02),
        LCDC_CMD_DATA16(0xD219, 0x48),
        LCDC_CMD_DATA16(0xD21A, 0x02),
        LCDC_CMD_DATA16(0xD21B, 0x4A),
        LCDC_CMD_DATA16(0xD21C, 0x02),
        LCDC_CMD_DATA16(0xD21D, 0x7E),
        LCDC_CMD_DATA16(0xD21E, 0x02),
        LCDC_CMD_DATA16(0xD21F, 0xBC),
        LCDC_CMD_DATA16(0xD220, 0x02),
        LCDC_CMD_DATA16(0xD221, 0xE1),
        LCDC_CMD_DATA16(0xD222, 0x03),
        LCDC_CMD_DATA16(0xD223, 0x10),
        LCDC_CMD_DATA16(0xD224, 0x03),
        LCDC_CMD_DATA16(0xD225, 0x31),
        LCDC_CMD_DATA16(0xD226, 0x03),
        LCDC_CMD_DATA16(0xD227, 0x5A),
        LCDC_CMD_DATA16(0xD228, 0x03),
        LCDC_CMD_DATA16(0xD229, 0x73),
        LCDC_CMD_DATA16(0xD22A, 0x03),
        LCDC_CMD_DATA16(0xD22B, 0x94),
        LCDC_CMD_DATA16(0xD22C, 0x03),
        LCDC_CMD_DATA16(0xD22D, 0x9F),
        LCDC_CMD_DATA16(0xD22E, 0x03),
        LCDC_CMD_DATA16(0xD22F, 0xB3),
        LCDC_CMD_DATA16(0xD230, 0x03),
        LCDC_CMD_DATA16(0xD231, 0xB9),
        LCDC_CMD_DATA16(0xD232, 0x03),
        LCDC_CMD_DATA16(0xD233, 0xC1),
        //#B+
        LCDC_CMD_DATA16(0xD300, 0x00),
        LCDC_CMD_DATA16(0xD301, 0x37),
        LCDC_CMD_DATA16(0xD302, 0x00),
        LCDC_CMD_DATA16(0xD303, 0x52),
        LCDC_CMD_DATA16(0xD304, 0x00),
        LCDC_CMD_DATA16(0xD305, 0x7B),
        LCDC_CMD_DATA16(0xD306, 0x00),
        LCDC_CMD_DATA16(0xD307, 0x99),
        LCDC_CMD_DATA16(0xD308, 0x00),
        LCDC_CMD_DATA16(0xD309, 0xB1),
        LCDC_CMD_DATA16(0xD30A, 0x00),
        LCDC_CMD_DATA16(0xD30B, 0xD2),
        LCDC_CMD_DATA16(0xD30C, 0x00),
        LCDC_CMD_DATA16(0xD30D, 0xF6),
        LCDC_CMD_DATA16(0xD30E, 0x01),
        LCDC_CMD_DATA16(0xD30F, 0x27),
        LCDC_CMD_DATA16(0xD310, 0x01),
        LCDC_CMD_DATA16(0xD311, 0x4E),
        LCDC_CMD_DATA16(0xD312, 0x01),
        LCDC_CMD_DATA16(0xD313, 0x8C),
        LCDC_CMD_DATA16(0xD314, 0x01),
        LCDC_CMD_DATA16(0xD315, 0xBE),
        LCDC_CMD_DATA16(0xD316, 0x02),
        LCDC_CMD_DATA16(0xD317, 0x0B),
        LCDC_CMD_DATA16(0xD318, 0x02),
        LCDC_CMD_DATA16(0xD319, 0x48),
        LCDC_CMD_DATA16(0xD31A, 0x02),
        LCDC_CMD_DATA16(0xD31B, 0x4A),
        LCDC_CMD_DATA16(0xD31C, 0x02),
        LCDC_CMD_DATA16(0xD31D, 0x7E),
        LCDC_CMD_DATA16(0xD31E, 0x02),
        LCDC_CMD_DATA16(0xD31F, 0xBC),
        LCDC_CMD_DATA16(0xD320, 0x02),
        LCDC_CMD_DATA16(0xD321, 0xE1),
        LCDC_CMD_DATA16(0xD322, 0x03),
        LCDC_CMD_DATA16(0xD323, 0x10),
        LCDC_CMD_DATA16(0xD324, 0x03),
        LCDC_CMD_DATA16(0xD325, 0x31),
        LCDC_CMD_DATA16(0xD326, 0x03),
        LCDC_CMD_DATA16(0xD327, 0x5A),
        LCDC_CMD_DATA16(0xD328, 0x03),
        LCDC_CMD_DATA16(0xD329, 0x73),
        LCDC_CMD_DATA16(0xD32A, 0x03),
        LCDC_CMD_DATA16(0xD32B, 0x94),
        LCDC_CMD_DATA16(0xD32C, 0x03),
        LCDC_CMD_DATA16(0xD32D, 0x9F),
        LCDC_CMD_DATA16(0xD32E, 0x03),
        LCDC_CMD_DATA16(0xD32F, 0xB3),
        LCDC_CMD_DATA16(0xD330, 0x03),
        LCDC_CMD_DATA16(0xD331, 0xB9),
        LCDC_CMD_DATA16(0xD332, 0x03),
        LCDC_CMD_DATA16(0xD333, 0xC1),

        //#R-///////////////////////////////////////////
        LCDC_CMD_DATA16(0xD400, 0x00),
        LCDC_CMD_DATA16(0xD401, 0x37),
        LCDC_CMD_DATA16(0xD402, 0x00),
        LCDC_CMD_DATA16(0xD403, 0x52),
        LCDC_CMD_DATA16(0xD404, 0x00),
        LCDC_CMD_DATA16(0xD405, 0x7B),
        LCDC_CMD_DATA16(0xD406, 0x00),
        LCDC_CMD_DATA16(0xD407, 0x99),
        LCDC_CMD_DATA16(0xD408, 0x00),
        LCDC_CMD_DATA16(0xD409, 0xB1),
        LCDC_CMD_DATA16(0xD40A, 0x00),
        LCDC_CMD_DATA16(0xD40B, 0xD2),
        LCDC_CMD_DATA16(0xD40C, 0x00),
        LCDC_CMD_DATA16(0xD40D, 0xF6),
        LCDC_CMD_DATA16(0xD40E, 0x01),
        LCDC_CMD_DATA16(0xD40F, 0x27),
        LCDC_CMD_DATA16(0xD410, 0x01),
        LCDC_CMD_DATA16(0xD411, 0x4E),
        LCDC_CMD_DATA16(0xD412, 0x01),
        LCDC_CMD_DATA16(0xD413, 0x8C),
        LCDC_CMD_DATA16(0xD414, 0x01),
        LCDC_CMD_DATA16(0xD415, 0xBE),
        LCDC_CMD_DATA16(0xD416, 0x02),
        LCDC_CMD_DATA16(0xD417, 0x0B),
        LCDC_CMD_DATA16(0xD418, 0x02),
        LCDC_CMD_DATA16(0xD419, 0x48),
        LCDC_CMD_DATA16(0xD41A, 0x02),
        LCDC_CMD_DATA16(0xD41B, 0x4A),
        LCDC_CMD_DATA16(0xD41C, 0x02),
        LCDC_CMD_DATA16(0xD41D, 0x7E),
        LCDC_CMD_DATA16(0xD41E, 0x02),
        LCDC_CMD_DATA16(0xD41F, 0xBC),
        LCDC_CMD_DATA16(0xD420, 0x02),
        LCDC_CMD_DATA16(0xD421, 0xE1),
        LCDC_CMD_DATA16(0xD422, 0x03),
        LCDC_CMD_DATA16(0xD423, 0x10),
        LCDC_CMD_DATA16(0xD424, 0x03),
        LCDC_CMD_DATA16(0xD425, 0x31),
        LCDC_CMD_DATA16(0xD426, 0x03),
        LCDC_CMD_DATA16(0xD427, 0x5A),
        LCDC_CMD_DATA16(0xD428, 0x03),
        LCDC_CMD_DATA16(0xD429, 0x73),
        LCDC_CMD_DATA16(0xD42A, 0x03),
        LCDC_CMD_DATA16(0xD42B, 0x94),
        LCDC_CMD_DATA16(0xD42C, 0x03),
        LCDC_CMD_DATA16(0xD42D, 0x9F),
        LCDC_CMD_DATA16(0xD42E, 0x03),
        LCDC_CMD_DATA16(0xD42F, 0xB3),
        LCDC_CMD_DATA16(0xD430, 0x03),
        LCDC_CMD_DATA16(0xD431, 0xB9),
        LCDC_CMD_DATA16(0xD432, 0x03),
        LCDC_CMD_DATA16(0xD433, 0xC1),

        //#G-//////////////////////////////////////////////
        LCDC_CMD_DATA16(0xD500, 0x00),
        LCDC_CMD_DATA16(0xD501, 0x37),
        LCDC_CMD_DATA16(0xD502, 0x00),
        LCDC_CMD_DATA16(0xD503, 0x52),
        LCDC_CMD_DATA16(0xD504, 0x00),
        LCDC_CMD_DATA16(0xD505, 0x7B),
        LCDC_CMD_DATA16(0xD506, 0x00),
        LCDC_CMD_DATA16(0xD507, 0x99),
        LCDC_CMD_DATA16(0xD508, 0x00),
        LCDC_CMD_DATA16(0xD509, 0xB1),
        LCDC_CMD_DATA16(0xD50A, 0x00),
        LCDC_CMD_DATA16(0xD50B, 0xD2),
        LCDC_CMD_DATA16(0xD50C, 0x00),
        LCDC_CMD_DATA16(0xD50D, 0xF6),
        LCDC_CMD_DATA16(0xD50E, 0x01),
        LCDC_CMD_DATA16(0xD50F, 0x27),
        LCDC_CMD_DATA16(0xD510, 0x01),
        LCDC_CMD_DATA16(0xD511, 0x4E),
        LCDC_CMD_DATA16(0xD512, 0x01),
        LCDC_CMD_DATA16(0xD513, 0x8C),
        LCDC_CMD_DATA16(0xD514, 0x01),
        LCDC_CMD_DATA16(0xD515, 0xBE),
        LCDC_CMD_DATA16(0xD516, 0x02),
        LCDC_CMD_DATA16(0xD517, 0x0B),
        LCDC_CMD_DATA16(0xD518, 0x02),
        LCDC_CMD_DATA16(0xD519, 0x48),
        LCDC_CMD_DATA16(0xD51A, 0x02),
        LCDC_CMD_DATA16(0xD51B, 0x4A),
        LCDC_CMD_DATA16(0xD51C, 0x02),
        LCDC_CMD_DATA16(0xD51D, 0x7E),
        LCDC_CMD_DATA16(0xD51E, 0x02),
        LCDC_CMD_DATA16(0xD51F, 0xBC),
        LCDC_CMD_DATA16(0xD520, 0x02),
        LCDC_CMD_DATA16(0xD521, 0xE1),
        LCDC_CMD_DATA16(0xD522, 0x03),
        LCDC_CMD_DATA16(0xD523, 0x10),
        LCDC_CMD_DATA16(0xD524, 0x03),
        LCDC_CMD_DATA16(0xD525, 0x31),
        LCDC_CMD_DATA16(0xD526, 0x03),
        LCDC_CMD_DATA16(0xD527, 0x5A),
        LCDC_CMD_DATA16(0xD528, 0x03),
        LCDC_CMD_DATA16(0xD529, 0x73),
        LCDC_CMD_DATA16(0xD52A, 0x03),
        LCDC_CMD_DATA16(0xD52B, 0x94),
        LCDC_CMD_DATA16(0xD52C, 0x03),
        LCDC_CMD_DATA16(0xD52D, 0x9F),
        LCDC_CMD_DATA16(0xD52E, 0x03),
        LCDC_CMD_DATA16(0xD52F, 0xB3),
        LCDC_CMD_DATA16(0xD530, 0x03),
        LCDC_CMD_DATA16(0xD531, 0xB9),
        LCDC_CMD_DATA16(0xD532, 0x03),
        LCDC_CMD_DATA16(0xD533, 0xC1),
        //#B-///////////////////////////////
        LCDC_CMD_DATA16(0xD600, 0x00),
        LCDC_CMD_DATA16(0xD601, 0x37),
        LCDC_CMD_DATA16(0xD602, 0x00),
        LCDC_CMD_DATA16(0xD603, 0x52),
        LCDC_CMD_DATA16(0xD604, 0x00),
        LCDC_CMD_DATA16(0xD605, 0x7B),
        LCDC_CMD_DATA16(0xD606, 0x00),
        LCDC_CMD_DATA16(0xD607, 0x99),
        LCDC_CMD_DATA16(0xD608, 0x00),
        LCDC_CMD_DATA16(0xD609, 0xB1),
        LCDC_CMD_DATA16(0xD60A, 0x00),
        LCDC_CMD_DATA16(0xD60B, 0xD2),
        LCDC_CMD_DATA16(0xD60C, 0x00),
        LCDC_CMD_DATA16(0xD60D, 0xF6),
        LCDC_CMD_DATA16(0xD60E, 0x01),
        LCDC_CMD_DATA16(0xD60F, 0x27),
        LCDC_CMD_DATA16(0xD610, 0x01),
        LCDC_CMD_DATA16(0xD611, 0x4E),
        LCDC_CMD_DATA16(0xD612, 0x01),
        LCDC_CMD_DATA16(0xD613, 0x8C),
        LCDC_CMD_DATA16(0xD614, 0x01),
        LCDC_CMD_DATA16(0xD615, 0xBE),
        LCDC_CMD_DATA16(0xD616, 0x02),
        LCDC_CMD_DATA16(0xD617, 0x0B),
        LCDC_CMD_DATA16(0xD618, 0x02),
        LCDC_CMD_DATA16(0xD619, 0x48),
        LCDC_CMD_DATA16(0xD61A, 0x02),
        LCDC_CMD_DATA16(0xD61B, 0x4A),
        LCDC_CMD_DATA16(0xD61C, 0x02),
        LCDC_CMD_DATA16(0xD61D, 0x7E),
        LCDC_CMD_DATA16(0xD61E, 0x02),
        LCDC_CMD_DATA16(0xD61F, 0xBC),
        LCDC_CMD_DATA16(0xD620, 0x02),
        LCDC_CMD_DATA16(0xD621, 0xE1),
        LCDC_CMD_DATA16(0xD622, 0x03),
        LCDC_CMD_DATA16(0xD623, 0x10),
        LCDC_CMD_DATA16(0xD624, 0x03),
        LCDC_CMD_DATA16(0xD625, 0x31),
        LCDC_CMD_DATA16(0xD626, 0x03),
        LCDC_CMD_DATA16(0xD627, 0x5A),
        LCDC_CMD_DATA16(0xD628, 0x03),
        LCDC_CMD_DATA16(0xD629, 0x73),
        LCDC_CMD_DATA16(0xD62A, 0x03),
        LCDC_CMD_DATA16(0xD62B, 0x94),
        LCDC_CMD_DATA16(0xD62C, 0x03),
        LCDC_CMD_DATA16(0xD62D, 0x9F),
        LCDC_CMD_DATA16(0xD62E, 0x03),
        LCDC_CMD_DATA16(0xD62F, 0xB3),
        LCDC_CMD_DATA16(0xD630, 0x03),
        LCDC_CMD_DATA16(0xD631, 0xB9),
        LCDC_CMD_DATA16(0xD632, 0x03),
        LCDC_CMD_DATA16(0xD633, 0xC1),

        //#Enable Page0
        LCDC_CMD_DATA16(0xF000, 0x55),
        LCDC_CMD_DATA16(0xF001, 0xAA),
        LCDC_CMD_DATA16(0xF002, 0x52),
        LCDC_CMD_DATA16(0xF003, 0x08),
        LCDC_CMD_DATA16(0xF004, 0x00),
        //# RGB I/F Setting
        LCDC_CMD_DATA16(0xB000, 0x08),
        LCDC_CMD_DATA16(0xB001, 0x05),
        LCDC_CMD_DATA16(0xB002, 0x02),
        LCDC_CMD_DATA16(0xB003, 0x05),
        LCDC_CMD_DATA16(0xB004, 0x02),
        //## SDT:
        LCDC_CMD_DATA16(0xB600, 0x08),
        LCDC_CMD_DATA16(0xB500, 0x50), //0x6b ???? 480x854       0x50 ???? 480x800

        //## Gate EQ:
        LCDC_CMD_DATA16(0xB700, 0x00),
        LCDC_CMD_DATA16(0xB701, 0x00),

        //## Source EQ:
        LCDC_CMD_DATA16(0xB800, 0x01),
        LCDC_CMD_DATA16(0xB801, 0x05),
        LCDC_CMD_DATA16(0xB802, 0x05),
        LCDC_CMD_DATA16(0xB803, 0x05),

        //# Inversion: Column inversion (NVT)
        LCDC_CMD_DATA16(0xBC00, 0x00),
        LCDC_CMD_DATA16(0xBC01, 0x00),
        LCDC_CMD_DATA16(0xBC02, 0x00),

        //# BOE's Setting(default)
        LCDC_CMD_DATA16(0xCC00, 0x03),
        LCDC_CMD_DATA16(0xCC01, 0x00),
        LCDC_CMD_DATA16(0xCC02, 0x00),

        //# Display Timing:
        LCDC_CMD_DATA16(0xBD00, 0x01),
        LCDC_CMD_DATA16(0xBD01, 0x84),
        LCDC_CMD_DATA16(0xBD02, 0x07),
        LCDC_CMD_DATA16(0xBD03, 0x31),
        LCDC_CMD_DATA16(0xBD04, 0x00),

        LCDC_CMD_DATA16(0xBA00, 0x01),

        LCDC_CMD_DATA16(0xFF00, 0xAA),
        LCDC_CMD_DATA16(0xFF01, 0x55),
        LCDC_CMD_DATA16(0xFF02, 0x25),
        LCDC_CMD_DATA16(0xFF03, 0x01),

#if MRB3973_DBIB_LANDSCAPE
        LCDC_CMD_DATA16(HW_LCDC_MIPI_DCS_SET_ADDRESS_MODE << 8, 0x60),
#else
        LCDC_CMD_DATA16(HW_LCDC_MIPI_DCS_SET_ADDRESS_MODE << 8, 0x00),
#endif

        LCDC_MIPI_SET_POSITION16(GDI_DISP_OFFSETX, GDI_DISP_OFFSETY,
                GDI_DISP_RESX + GDI_DISP_OFFSETX - 1, GDI_DISP_RESY + GDI_DISP_OFFSETY - 1),

        LCDC_MIPI_SET_MODE16(GDI_DISP_COLOR == HW_LCDC_OCM_8RGB565   ? MRB3973_DBIB_PIXEL_FORMAT_RGB565 :
                             GDI_DISP_COLOR == HW_LCDC_OCM_8RGB666   ? MRB3973_DBIB_PIXEL_FORMAT_RGB666 :
                                                                       MRB3973_DBIB_PIXEL_FORMAT_RGB888),

        LCDC_MIPI_SET_TEAR_ON16(0x0),
};

static const uint8_t screen_power_on_cmds[] = {
};

static const uint8_t screen_enable_cmds[] = {
        LCDC_MIPI_CMD16(HW_LCDC_MIPI_DCS_EXIT_SLEEP_MODE << 8),
        LCDC_DELAY_MS(120),
        LCDC_MIPI_CMD16(HW_LCDC_MIPI_DCS_SET_DISPLAY_ON << 8),
        LCDC_DELAY_MS(50),
};

static const uint8_t screen_disable_cmds[] =
{
        LCDC_MIPI_CMD16(HW_LCDC_MIPI_DCS_SET_DISPLAY_OFF << 8),
        LCDC_DELAY_MS(120),
        LCDC_MIPI_CMD16(HW_LCDC_MIPI_DCS_ENTER_SLEEP_MODE << 8),
        LCDC_DELAY_MS(50),
};

static const uint8_t screen_power_off_cmds[] = {
};

static const uint8_t screen_clear_cmds[] = {
};

__STATIC_INLINE void screen_set_partial_update_area(hw_lcdc_frame_t *frame)
{
        UNUSED_ARG(frame);
}

#endif /* dg_configLCDC_ADAPTER */

#endif /* dg_configUSE_MRB3973_DBIB */

#endif /* MRB3973_DBIB_H_ */
