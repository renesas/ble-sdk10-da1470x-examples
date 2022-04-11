/**
 ****************************************************************************************
 *
 * @file zt2628.h
 *
 * @brief Touch configuration for ZT2628
 *
 * Copyright (C) 2021-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#ifndef ZT2628_H_
#define ZT2628_H_

#include <stdio.h>
#include <stdint.h>
#include "platform_devices.h"
#include "ad_i2c.h"

#if dg_configUSE_ZT2628

/*********************************************************************
 *
 *       Defines
 *
 *********************************************************************
 */
#define GDI_TOUCH_ENABLE                                        (1)
#define GDI_TOUCH_INTERFACE                                     (GDI_TOUCH_INTERFACE_I2C)
#define GDI_TOUCH_CONFIG                                        (&zt2628_cfg)
#define GDI_TOUCH_INIT                                          (zt2628_init)
#define GDI_TOUCH_READ_EVENT                                    (zt2628_read_event)
#define ZT2628_DEBUG_EN                                         ( 0 )

#define ZT2628_I2C_ADDRESS                                      0x20

/* Register Map */
#define ZT2628_SWRESET_CMD                                      0x0000
#define ZT2628_WAKEUP_CMD                                       0x0001
#define ZT2628_IDLE_CMD                                         0x0004
#define ZT2628_SLEEP_CMD                                        0x0005
#define ZT2628_CLEAR_INT_STATUS_CMD                             0x0003
#define ZT2628_CALIBRATE_CMD                                    0x0006
#define ZT2628_SAVE_STATUS_CMD                                  0x0007
#define ZT2628_SAVE_CALIBRATION_CMD                             0x0008
#define ZT2628_RECALL_FACTORY_CMD                               0x000f
#define ZT2628_THRESHOLD                                        0x0020
#define ZT2628_DEBUG                                            0x0115
#define ZT2628_TOUCH_MODE                                       0x0010
#define ZT2628_CHIP_REVISION                                    0x0011
#define ZT2628_FIRMWARE_VERSION                                 0x0012
#define ZT2628_MINOR_FW_VERSION                                 0x0121
#define ZT2628_VENDOR_ID                                        0x001C
#define ZT2628_HW_ID                                            0x0014
#define ZT2628_DATA_VERSION                                     0x0013
#define ZT2628_SUPPORTED_FINGER_NUM                             0x0015
#define ZT2628_EEPROM_INFO                                      0x0018
#define ZT2628_INITIAL_TOUCH_MODE                               0x0019
#define ZT2628_TOTAL_NUMBER_OF_X                                0x0060
#define ZT2628_TOTAL_NUMBER_OF_Y                                0x0061
#define ZT2628_DELAY_RAW_FOR_HOST                               0x007f
#define ZT2628_BUTTON_SUPPORTED_NUM                             0x00B0
#define ZT2628_BUTTON_SENSITIVITY                               0x00B2
#define ZT2628_DUMMY_BUTTON_SENSITIVITY                         0x00C8
#define ZT2628_X_RESOLUTION                                     0x00C0
#define ZT2628_Y_RESOLUTION                                     0x00C1
#define ZT2628_POINT_STATUS                                     0x0080
#define ZT2628_ICON_STATUS                                      0x00AA
#define ZT2628_DND_SHIFT_VALUE                                  0x012B
#define ZT2628_AFE_FREQUENCY                                    0x0100
#define ZT2628_DND_N_COUNT                                      0x0122
#define ZT2628_DND_U_COUNT                                      0x0135
#define ZT2628_RAWDATA                                          0x0200
#define ZT2628_INT_EN                                           0x00f0
#define ZT2628_PERIODICAL_INT_INTV                              0x00f1
#define ZT2628_CHECKSUM_RESULT                                  0x012c
#define ZT2628_INIT_FLASH                                       0x01d0
#define ZT2628_WRITE_FLASH                                      0x01d1
#define ZT2628_READ_FLASH                                       0x01d2
#define ZT2628_OPTIONAL_SETTING                                 0x0116
#define ZT2628_COVER_CONTROL                                    0x023E

/* ZT2628_COVER_CONTROL */
#define ZT2628_WALLET_COVER_CLOSE                               0x0000
#define ZT2628_VIEW_COVER_CLOSE                                 0x0100
#define ZT2628_COVER_OPEN                                       0x0200
#define ZT2628_LED_COVER_CLOSE                                  0x0700
#define ZT2628_CLEAR_COVER_CLOSE                                0x0800

/* ZT2628_INT_EN */
#define ZT2628_INT_PT_CNT_CHANGE                                0x0001 // 0
#define ZT2628_INT_DOWN                                         0x0002 // 1
#define ZT2628_INT_MOVE                                         0x0004 // 2
#define ZT2628_INT_UP                                           0x0008 // 3
#define ZT2628_INT_PALM                                         0x0010 // 4
#define ZT2628_INT_PALM_REJECT                                  0x0020 // 5
#define ZT2628_INT_GESTURE                                      0x0040 // 6
#define ZT2628_INT_WEIGHT_CHANGE                                0x0100 // 8
#define ZT2628_INT_PT_NO_CHANGE                                 0x0200 // 9
#define ZT2628_INT_REJECT                                       0x0400 // 10
#define ZT2628_INT_PT_EXIST                                     0x0800 // 11
#define ZT2628_INT_MUST_ZERO                                    0x2000 // 13
#define ZT2628_INT_DEBUG                                        0x4000 // 14
#define ZT2628_INT_ICON_EVENT                                   0x8000 // 15

/* ZT2628_POINT_STATUS */
#define ZT2628_POINT_STATUS_EXIST                               0x0001 // 0
#define ZT2628_POINT_STATUS_DOWN                                0x0002 // 1
#define ZT2628_POINT_STATUS_MOVE                                0x0004 // 2
#define ZT2628_POINT_STATUS_UP                                  0x0008 // 3
#define ZT2628_POINT_STATUS_UPDATE                              0x0010 // 4
#define ZT2628_POINT_STATUS_WAIT                                0x0020 // 5

#define ZT2628_CHIP_CODE                                        0xE628
#define ZT2628_CHIP_ON_DELAY                                    50     // ms
#define ZT2628_FIRMWARE_ON_DELAY                                150    // ms

#define ZT2628_TOUCH_POINT_MODE                                 0

/*********************************************************************
 *
 *       Types
 *
 **********************************************************************
 */
typedef struct {
        uint16_t     x;
        uint16_t     y;
        uint8_t      width;
        uint8_t      status;
} z2628_coord_t ;

typedef struct {
        uint16_t     status;
        uint8_t      finger_cnt;
        uint8_t      timestamp;
        z2628_coord_t coord;
} z2628_point_info_t ;

/*********************************************************************
 *
 *       Macros
 *
 **********************************************************************
 */

/*********************************************************************
 *
 *       Static code
 *
 *********************************************************************
 */

/* I2C driver configurations  */
static const ad_i2c_driver_conf_t zt2628_drv = {
        I2C_DEFAULT_CLK_CFG,
        .i2c.speed              = HW_I2C_SPEED_FAST,
        .i2c.mode               = HW_I2C_MODE_MASTER,
        .i2c.addr_mode          = HW_I2C_ADDRESSING_7B,
        .i2c.address            = ZT2628_I2C_ADDRESS,
        .dma_channel            = HW_DMA_CHANNEL_2
};

/* I2C controller configurations */
static const ad_i2c_controller_conf_t zt2628_cfg = {
        .id  = HW_I2C1,
        .drv = &zt2628_drv,
        .io  = &zt2628_io
};

/**
 * Map touch events to display coordinates (physical dimensions)
 */
__STATIC_INLINE uint16_t zt2628_map_touch_to_display(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min,
                                                                                                   uint16_t out_max)
{
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/* User-defined routine used by GDI to initialize the target touch controller */
__STATIC_INLINE void zt2628_init(void *dev)
{
        uint16_t addr[] = { 0 };
        uint16_t cmd[] = { 0, 0 };
        uint16_t chip_code;

        /* Enable I2C pullups */
        hw_gpio_configure_pin(DA_PWR_ON_PORT, DA_PWR_ON_PIN, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true);
        hw_gpio_pad_latch_enable(DA_PWR_ON_PORT, DA_PWR_ON_PIN);
        hw_gpio_pad_latch_disable(DA_PWR_ON_PORT, DA_PWR_ON_PIN);

        hw_clk_delay_usec(1500);
        /* Exit from reset */
        hw_gpio_configure_pin(ZT2628_RST_PORT, ZT2628_RST_PIN, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true);
        hw_gpio_configure_pin_power(ZT2628_RST_PORT, ZT2628_RST_PIN, GDI_TOUCH_CONFIG->io->voltage_level);
        hw_gpio_pad_latch_enable(ZT2628_RST_PORT, ZT2628_RST_PIN);
        hw_gpio_pad_latch_disable(ZT2628_RST_PORT, ZT2628_RST_PIN);
        hw_clk_delay_usec(1000);

        /*
         * power_sequence
         */
        /* send power sequence(vendor cmd enable) */
        cmd[0] = 0xC000;
        cmd[1] = 0x0001;
        ad_i2c_write(dev, (uint8_t *)cmd, sizeof(cmd), HW_I2C_F_ADD_STOP);

        hw_clk_delay_usec(10);

        /* read chip code */
        addr[0] = 0xcc00;
        ad_i2c_write_read(dev, (uint8_t *)addr, sizeof(addr), (uint8_t *)&chip_code, sizeof(chip_code), HW_I2C_F_ADD_STOP);

#if ZT2628_DEBUG_EN
        printf("chip_code: 0x%.4X\r\n", chip_code);
#endif
        if (chip_code != ZT2628_CHIP_CODE) {
                return;
        }

        /* send power sequence(intn clear) */
        addr[0] = 0xc004;
        ad_i2c_write(dev, (uint8_t *)addr, sizeof(addr), HW_I2C_F_ADD_STOP);

        hw_clk_delay_usec(10);

        /* send power sequence(nvm init) */
        cmd[0] = 0xc002;
        cmd[1] = 0x0001;
        ad_i2c_write(dev, (uint8_t *)cmd, sizeof(cmd), HW_I2C_F_ADD_STOP);

        hw_clk_delay_usec(2 * 1000);

        /* send power sequence(program start) */
        cmd[0] = 0xc001;
        cmd[1] = 0x0001;
        ad_i2c_write(dev, (uint8_t *)cmd, sizeof(cmd), HW_I2C_F_ADD_STOP);

        OS_DELAY_MS(ZT2628_FIRMWARE_ON_DELAY);      /* wait for checksum cal */

#if ZT2628_DEBUG_EN
        /*
         * ic_version_check
         */
        uint16_t vendor_id, fw_version, fw_minor_version, ic_revision, reg_data_version, hw_id;
        uint8_t version_data[8];

        addr[0] = ZT2628_VENDOR_ID;
        ad_i2c_write_read(dev, (uint8_t *)addr, sizeof(addr), (uint8_t *)&vendor_id, sizeof(vendor_id), HW_I2C_F_ADD_STOP);

        addr[0] = ZT2628_MINOR_FW_VERSION;
        ad_i2c_write_read(dev, (uint8_t *)addr, sizeof(addr), (uint8_t *)&fw_minor_version, sizeof(fw_minor_version), HW_I2C_F_ADD_STOP);

        addr[0] = ZT2628_CHIP_REVISION;
        ad_i2c_write_read(dev, (uint8_t *)addr, sizeof(addr), version_data, sizeof(version_data), HW_I2C_F_ADD_STOP);

        ic_revision = version_data[0] | (version_data[1] << 8);
        fw_version = version_data[2] | (version_data[3] << 8);
        reg_data_version = version_data[4] | (version_data[5] << 8);
        hw_id = version_data[6] | (version_data[7] << 8);
        printf("vendor_id: 0x%.4X\r\n", vendor_id);
        printf("fw_minor_version: 0x%.4X\r\n", fw_minor_version);
        printf("ic_revision: 0x%.4X\r\n", ic_revision);
        printf("fw_version: 0x%.4X\r\n", fw_version);
        printf("reg_data_version: 0x%.4X\r\n", reg_data_version);
        printf("hw_id: 0x%.4X\r\n", hw_id);

#endif
        /*
         * init_touch
         */
#if ZT2628_DEBUG_EN
        uint16_t x_node_num, y_node_num;
        uint8_t res_data[4];

        addr[0] = ZT2628_TOTAL_NUMBER_OF_X;
        ad_i2c_write_read(dev, (uint8_t *)addr, sizeof(addr), (uint8_t *)&res_data, sizeof(res_data), HW_I2C_F_ADD_STOP);
        x_node_num = res_data[0] | (res_data[1] << 8);
        y_node_num = res_data[2] | (res_data[3] << 8);

        printf("x_node_num: 0x%.4X\r\n", x_node_num);
        printf("y_node_num: 0x%.4X\r\n", y_node_num);

#endif
        /* write reset command */
        addr[0] = ZT2628_SWRESET_CMD;
        ad_i2c_write(dev, (uint8_t *)addr, sizeof(addr), HW_I2C_F_ADD_STOP);

        cmd[0] = ZT2628_TOUCH_MODE;
        cmd[1] = ZT2628_TOUCH_POINT_MODE;
        ad_i2c_write(dev, (uint8_t *)cmd, sizeof(cmd), HW_I2C_F_ADD_STOP);

        /* cover_set */
        cmd[0] = ZT2628_COVER_CONTROL;
        cmd[1] = ZT2628_COVER_OPEN;
        ad_i2c_write(dev, (uint8_t *)cmd, sizeof(cmd), HW_I2C_F_ADD_STOP);

        cmd[0] = ZT2628_INT_EN;
        cmd[1] = ZT2628_INT_PT_CNT_CHANGE | ZT2628_INT_DOWN | ZT2628_INT_MOVE | ZT2628_INT_UP;
        ad_i2c_write(dev, (uint8_t *)cmd, sizeof(cmd), HW_I2C_F_ADD_STOP);

        /* read garbage data */
        addr[0] = ZT2628_CLEAR_INT_STATUS_CMD;
        for (int i = 0; i < 10; i++) {
                ad_i2c_write(dev, (uint8_t *)addr, sizeof(addr), HW_I2C_F_ADD_STOP);
                hw_clk_delay_usec(10);
        }
}

/* User-defined routine used by GDI to read touch events */
__STATIC_INLINE void zt2628_read_event(void *dev, gdi_touch_data_t *touch_data)
{
        uint8_t status;
        uint16_t x, y;
        uint16_t addr[] = { 0 };
        z2628_point_info_t touch_info;

#if ZT2628_DEBUG_EN
        uint16_t ic_status;

        addr[0] = ZT2628_DEBUG;
        ad_i2c_write_read(dev, (uint8_t *)addr, sizeof(addr), (uint8_t *)&ic_status, sizeof(ic_status), HW_I2C_F_ADD_STOP);

#endif
        /* read point info */
        addr[0] = ZT2628_POINT_STATUS;
        ad_i2c_write_read(dev, (uint8_t *)addr, sizeof(addr), (uint8_t *)&touch_info, sizeof(touch_info), HW_I2C_F_ADD_STOP);

        if ((touch_info.status & ZT2628_INT_MUST_ZERO) ||(touch_info.status == 0x1)) {
                printf("abnormal point info read\r\n");
        }

        addr[0] = ZT2628_CLEAR_INT_STATUS_CMD;
        ad_i2c_write(dev, (uint8_t *)addr, sizeof(addr), HW_I2C_F_ADD_STOP);

        if (touch_info.finger_cnt > 1) {
                printf("finger count %d\r\n", touch_info.finger_cnt);
        }

        status = touch_info.coord.status;
        x = touch_info.coord.x;
        y = touch_info.coord.y;

        touch_data->pressed = 0;
        if (status & ZT2628_POINT_STATUS_EXIST) {
                if (status & ZT2628_POINT_STATUS_DOWN) {
                        touch_data->pressed = 1;
                } else if (status & ZT2628_POINT_STATUS_MOVE) {
                        touch_data->pressed = 1;
                }
        }

#if ZT2628_DEBUG_EN
        static uint8_t prev_status = 0;
        uint16_t w = touch_info.coord.width;

        if (status & ZT2628_POINT_STATUS_EXIST) {
                if (status & ZT2628_POINT_STATUS_DOWN) {
                        printf("Finger down x = %d, y = %d, w = %d\r\n", x, y, w);
                } else if (status & ZT2628_POINT_STATUS_MOVE) {
                        printf("Finger move x = %d, y = %d, w = %d\r\n", x, y, w);
                }
        } else if ((status & ZT2628_POINT_STATUS_UP) || (prev_status & ZT2628_POINT_STATUS_EXIST)) {
                printf("Finger up %s\r\n",
                        ((status & ZT2628_POINT_STATUS_UP) && (prev_status & ZT2628_POINT_STATUS_EXIST)) ? "both" :
                        (status & ZT2628_POINT_STATUS_UP) ? "up" : "exist");
        } else {
                printf("Finger unknown\r\n");
        }

        prev_status = status;

#endif
        /* Convert touch data to display coordinates */
        touch_data->x = zt2628_map_touch_to_display(x, 0, GDI_DISP_RESX, 0, GDI_DISP_RESX);
        touch_data->y = zt2628_map_touch_to_display(y, 0, GDI_DISP_RESY, 0, GDI_DISP_RESY);
}

#endif /* dg_configUSE_ZT2628 */

#endif /* ZT2628_H_ */
