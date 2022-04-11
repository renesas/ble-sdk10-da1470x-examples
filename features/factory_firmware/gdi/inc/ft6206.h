/**
 ****************************************************************************************
 *
 * @file ft6206.h
 *
 * @brief Touch configuration for FT6206
 *
 * Copyright (C) 2020-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#ifndef FT6206_H_
#define FT6206_H_

#include <stdio.h>
#include <stdint.h>
#include "platform_devices.h"
#include "ad_i2c.h"

#if dg_configUSE_FT6206

/*********************************************************************
 *
 *       Defines
 *
 *********************************************************************
 */
#define GDI_TOUCH_ENABLE                (1)
#define GDI_TOUCH_INTERFACE             (GDI_TOUCH_INTERFACE_I2C)
#define GDI_TOUCH_CONFIG                (&ft6206_cfg)
#define GDI_TOUCH_INIT                  (ft6206_init)
#define GDI_TOUCH_READ_EVENT            (ft6206_read_event)
#define FT6206_LANDSCAPE                (0)

#define FT6206_REG_DEVICE_MODE          0x00 // (R/W)
#define FT6206_REG_GEST_ID              0x01 // (R)
#define FT6206_REG_TD_STATUS            0x02 // (R)
#define FT6206_REG_P1_XH                0x03 // (R)
#define FT6206_REG_P1_XL                0x04 // (R)
#define FT6206_REG_P1_YH                0x05 // (R)
#define FT6206_REG_P1_YL                0x06 // (R)
#define FT6206_REG_P1_WEIGHT            0x07 // (R)
#define FT6206_REG_P1_MISC              0x08 // (R)
#define FT6206_REG_P2_XH                0x09 // (R)
#define FT6206_REG_P2_XL                0x0A // (R)
#define FT6206_REG_P2_YH                0x0B // (R)
#define FT6206_REG_P2_YL                0x0C // (R)
#define FT6206_REG_P2_WEIGHT            0x0D // (R)
#define FT6206_REG_P2_MISC              0x0E // (R)
#define FT6206_REG_TH_GROUP             0x80 // (R/W)
#define FT6206_REG_TH_DIFF              0x85 // (R/W)
#define FT6206_REG_CTRL                 0x86 // (R/W)
#define FT6206_REG_TIMEENTERMONITOR     0x87 // (R/W)
#define FT6206_REG_PERIODACTIVE         0x88 // (R/W)
#define FT6206_REG_PERIODMONITOR        0x89 // (R/W)
#define FT6206_REG_RADIAN_VALUE         0x91 // (R/W)
#define FT6206_REG_OFFSET_LEFT_RIGHT    0x92 // (R/W)
#define FT6206_REG_OFFSET_UP_DOWN       0x93 // (R/W)
#define FT6206_REG_DISTANCE_UP_DOWN     0x94 // (R/W)
#define FT6206_REG_DISTANCE_LEFT_RIGHT  0x95 // (R/W)
#define FT6206_REG_DISTANCE_ZOOM        0x96 // (R/W)
#define FT6206_REG_LIB_VER_H            0xA1 // (R)
#define FT6206_REG_LIB_VER_L            0xA2 // (R)
#define FT6206_REG_CIPHER               0xA3 // (R)
#define FT6206_REG_G_MODE               0xA4 // (R/W)
#define FT6206_REG_PWR_MODE             0xA5 // (R/W)
#define FT6206_REG_FIRMID               0xA6 // (R)
#define FT6206_REG_FOCALTECH_ID         0xA8 // (R)
#define FT6206_REG_RELEASE_CODE_ID      0xAF // (R)
#define FT6206_REG_STATE                0xDB // (R/W)

#define FT6206_I2C_ADDRESS              0x38

/*********************************************************************
 *
 *       Types
 *
 **********************************************************************
 */
typedef enum {
        FT6206_EVENT_FLAG_PRESS_DOWN = 0,
        FT6206_EVENT_FLAG_LIFT_UP,
        FT6206_EVENT_FLAG_CONTACT,
        FT6206_EVENT_FLAG_NO_EVENT
} FT6206_EVENT_FLAG;

typedef enum {
        FT6206_GEST_ID_NO_GESTURE   = 0,
        FT6206_GEST_ID_MOVE_UP      = 0x10,
        FT6206_GEST_ID_MOVE_RIGHT   = 0x14,
        FT6206_GEST_ID_MOVE_DOWN    = 0x18,
        FT6206_GEST_ID_MOVE_LEFT    = 0x1C,
        FT6206_GEST_ID_ZOOM_IN      = 0x48,
        FT6206_GEST_ID_ZOOM_OUT     = 0x49,
} FT6206_GEST_ID;

/*********************************************************************
 *
 *       Macros
 *
 **********************************************************************
 */
#define FT6206_GET_EVENT_FLAG(Pn_H)         ((Pn_H & 0xC0) >> 6)
#define FT6206_GET_XY_COORDINATE(PnH, PnL)  (((PnH & 0x0F) << 8) | PnL)

/*
 * const ad_i2c_io_conf_t io_ft6206 = {
 *        .scl = {
 *              .port = FT6206_SCL_PORT, .pin = FT6206_SCL_PIN,
 *              .on  = { HW_GPIO_MODE_OUTPUT_OPEN_DRAIN, HW_GPIO_FUNC_I2C_SCL, false },
 *              .off = { HW_GPIO_MODE_INPUT,             HW_GPIO_FUNC_GPIO,    true  },
 *        },
 *        .sda = {
 *             .port = FT6206_SDA_PORT, .pin = FT6206_SDA_PORT,
 *             .on  = { HW_GPIO_MODE_OUTPUT_OPEN_DRAIN, HW_GPIO_FUNC_I2C_SDA, false },
 *             .off = { HW_GPIO_MODE_INPUT,             HW_GPIO_FUNC_GPIO,    true  },
 *        },
 *        .voltage_level = HW_GPIO_POWER_V33
 * };
 */

/*********************************************************************
 *
 *       Static code
 *
 *********************************************************************
 */

/* I2C driver configurations  */
static const ad_i2c_driver_conf_t ft6206_drv = {
        I2C_DEFAULT_CLK_CFG,
        .i2c.speed              = HW_I2C_SPEED_STANDARD, /* 100kb/s */
        .i2c.mode               = HW_I2C_MODE_MASTER,
        .i2c.addr_mode          = HW_I2C_ADDRESSING_7B,
        .i2c.address            = FT6206_I2C_ADDRESS,
        .dma_channel            = HW_DMA_CHANNEL_2
};

/* I2C controller configurations */
static const ad_i2c_controller_conf_t ft6206_cfg = {
        .id  = HW_I2C1,
        .drv = &ft6206_drv,
        .io  = &ft6206_io
};

/**
 * Map touch events to display coordinates (physical dimensions)
 */
__STATIC_INLINE uint16_t ft6206_map_touch_to_display(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min,
                                                                                                   uint16_t out_max)
{
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/* User-defined routine used by GDI to initialize the target touch controller */
__STATIC_INLINE void ft6206_init(void *dev)
{
        uint8_t write_addr[] = { FT6206_REG_FOCALTECH_ID };

        uint8_t read_buf[1];
        ad_i2c_write_read(dev, (const uint8_t *)write_addr, sizeof(write_addr), read_buf, sizeof(read_buf),
                                                                                                HW_I2C_F_ADD_STOP);

        if (read_buf[0] == 0x11) {
        }
        else {
                OS_ASSERT(0);
        }
}

/* User-defined routine used by GDI to read touch events */
__STATIC_INLINE void ft6206_read_event(void *dev, gdi_touch_data_t *touch_data)
{
        uint8_t write_addr[] = { FT6206_REG_GEST_ID };
        uint8_t read_buf[6];

        uint16_t touch_x, touch_y;

        ad_i2c_write_read(dev, (const uint8_t *)write_addr, sizeof(write_addr), read_buf, sizeof(read_buf),
                                                                                                HW_I2C_F_ADD_STOP);
        /* Get and exercise event flag */
        switch(FT6206_GET_EVENT_FLAG(read_buf[2])) {
        case FT6206_EVENT_FLAG_PRESS_DOWN:
                touch_data->pressed = 1;
                break;
        case FT6206_EVENT_FLAG_LIFT_UP:
                touch_data->pressed = 0;
                break;
        case FT6206_EVENT_FLAG_CONTACT:
                touch_data->pressed = 1;
                break;
        case FT6206_EVENT_FLAG_NO_EVENT:
                touch_data->pressed = 0;
                break;
        default:
                /* Invalid status */
                OS_ASSERT(0);
                break;
        }

#if FT6206_LANDSCAPE
        /* Swap coordinates and mirror vertical */
        touch_x = FT6206_GET_XY_COORDINATE(read_buf[4], read_buf[5]);
        touch_y = GDI_DISP_RESY - FT6206_GET_XY_COORDINATE(read_buf[2], read_buf[3]);
#else
        touch_x = FT6206_GET_XY_COORDINATE(read_buf[2], read_buf[3]);
        touch_y = FT6206_GET_XY_COORDINATE(read_buf[4], read_buf[5]);
#endif
        /* Convert touch data to display coordinates */
        touch_data->x = ft6206_map_touch_to_display(touch_x, 0, GDI_DISP_RESX, 0, GDI_DISP_RESX);
        touch_data->y = ft6206_map_touch_to_display(touch_y, 0, GDI_DISP_RESY, 0, GDI_DISP_RESY);
}

#endif /* dg_configUSE_FT6206 */

#endif /* FT6206_H_ */
