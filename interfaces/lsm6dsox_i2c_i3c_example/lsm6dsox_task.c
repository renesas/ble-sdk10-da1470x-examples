/**
 ****************************************************************************************
 *
 * @file lsm6dsox_task.c
 *
 * @brief lsm6dsox task file
 *
 * Copyright (c) 2022 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor. By using this Software
 * you agree that Dialog Semiconductor retains all intellectual property and proprietary
 * rights in and to this Software and any use, reproduction, disclosure or distribution
 * of the Software without express written permission or a license agreement from Dialog
 * Semiconductor is strictly prohibited. This Software is solely for use on or in
 * conjunction with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR AS
 * REQUIRED BY LAW, THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE PROVIDED
 * IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR BY LAW, IN NO EVENT SHALL DIALOG
 * SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE.
 *
 ****************************************************************************************
 */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#if defined(dg_configUseI3CHandling)
#include <ad_i3c.h>
#elif defined(dg_configUseI2CHandling)
#include <ad_i2c.h>
#endif

#include "peripheral_setup.h"
#include "platform_devices.h"
#include "osal.h"
#include "lsm6dsox_task.h"
#include "lsm6dsox_reg.h"

/* Retained symbols */

//__RETAINED static OS_EVENT signal_i2c_async_done;

/*
 * Error code returned after an I2C operation. It can be used
 * to identify the reason of a failure.
 */
#if defined(dg_configUseI3CHandling)
__RETAINED ad_i3c_handle_t lsm6dsox_handle;
i3c_private_transfer_config i3c_write_config = {
        .i3c_tranfer_speed = HW_I3C_PRIVATE_TRANSFER_SPEED_SDR1_I3C_FAST_MODE_PLUS_I2C,
        .slave_dev_idx = HW_I3C_SLAVE_ADDRESS_TABLE_LOCATION_1,
        .i3c_tid = HW_I3C_TRANSACTION_ID_1,
        .termination_on_completion = HW_I3C_TRANSFER_TOC_STOP,
        .response_on_completion = 1,
        .cmd_response={0,0}

};
i3c_private_transfer_config i3c_read_config = {
        .i3c_tranfer_speed = HW_I3C_PRIVATE_TRANSFER_SPEED_SDR1_I3C_FAST_MODE_PLUS_I2C,
        .slave_dev_idx = HW_I3C_SLAVE_ADDRESS_TABLE_LOCATION_1,
        .i3c_tid = HW_I3C_TRANSACTION_ID_2,
        .termination_on_completion = HW_I3C_TRANSFER_TOC_STOP,
        .response_on_completion = 1,
        .cmd_response={0,0}

};
int lsm6dsox_i3c_init(void)
{
        lsm6dsox_handle = ad_i3c_open(LSM6DSOX_DEVICE);
        if(lsm6dsox_handle){
                return LSM6DOX_OK;
        }
        lsm6dsox_handle = NULL;

        return -EIO;

}
int lsm6dsox_i3c_reg_write(uint8_t reg, const uint8_t *bufp, uint16_t len)
{
        uint8_t buff[1 + len];
        buff[0] = reg;
        memcpy(buff + 1, bufp, len);
        lsm6dsox_errcode err = ad_i3c_private_write(lsm6dsox_handle, buff, 1 + len,
                &i3c_write_config,
                OS_EVENT_FOREVER);
        if (LSM6DOX_OK == err) {
                return LSM6DOX_OK;
        } else {
                printf("i3c write error(%d) [%x]\n",
                        err, reg);
        }

        return -EIO;
}
int lsm6dsox_i3c_reg_read(uint8_t reg, uint8_t *bufp, uint16_t len)
{

        lsm6dsox_errcode err = ad_i3c_private_write(lsm6dsox_handle, &reg, 1, &i3c_write_config,
                OS_EVENT_FOREVER);
        if (LSM6DOX_OK == err)
                {
                lsm6dsox_errcode err = ad_i3c_private_read(lsm6dsox_handle, bufp, len,
                        &i3c_read_config, OS_EVENT_FOREVER);

                if (LSM6DOX_OK == err) {
                        return LSM6DOX_OK;
                } else {
                        printf("i3c read error(%d) [0x%x]\n", err, reg);
                }
        }
        else
                printf("i3c write before read error(%d)[0x%x]\n", err, reg);
        return -EIO;
}
#elif defined(dg_configUseI2CHandling)
__RETAINED ad_i2c_handle_t lsm6dsox_handle;

int lsm6dsox_i2c_init(void)
{
        lsm6dsox_handle = ad_i2c_open(LSM6DSOX_DEVICE);
        if(lsm6dsox_handle){
                return LSM6DOX_OK;
        }
        lsm6dsox_handle = NULL;

        return -EIO;

}
int lsm6dsox_i2c_reg_write(uint8_t reg,  const uint8_t *bufp,uint16_t len)
{
        uint8_t buff[1+len];
        buff[0]=reg;
        memcpy(buff+1,bufp,len);
        lsm6dsox_errcode err = ad_i2c_write(lsm6dsox_handle, buff, 1+len, HW_I2C_F_ADD_STOP);
        if(LSM6DOX_OK == err){
               // printf("successfully WRITE  0x%x\n", reg);
                return LSM6DOX_OK;
        } else {
                printf("i2c write error(%d) [%x]\n",
                                        err, reg);
        }

        return -EIO;
}
int lsm6dsox_i2c_reg_read(uint8_t reg,uint8_t *bufp,uint16_t len)
{
        lsm6dsox_errcode err = ad_i2c_write_read(lsm6dsox_handle, &reg,1,bufp,len,HW_I2C_F_ADD_STOP);
        if(LSM6DOX_OK == err){
               // printf("successfully read  0x%x", reg);
                return LSM6DOX_OK;
        } else {
                printf("i2c read error(%d) [0x%x]\n", err, reg);
        }
        return -EIO;
}
#endif
void display_device_interface(void)
{
#if defined(dg_configUseI2CHandling)
            printf(" _________________________________________\n\r");
            printf("|                                         |\n\r");
            printf("|           ___     ___      _____        |\n\r");
            printf("|          |   |       |    /             |\n\r");
            printf("|           | |     __ |   |              |\n\r");
            printf("|           | |    |       |              |\n\r");
            printf("|          |___|   |___    \\_____        |\n\r");
            printf("|                                         |\n\r");
            printf("|_________________________________________|\n\r");

#elif defined(dg_configUseI3CHandling)
            printf(" _________________________________________\n\r");
            printf("|                                         |\n\r");
            printf("|           ___     ___      _____        |\n\r");
            printf("|          |   |       |    /             |\n\r");
            printf("|           | |     __ |   |              |\n\r");
            printf("|           | |        |   |              |\n\r");
            printf("|          |___|    ___|   \\_____        |\n\r");
            printf("|                                         |\n\r");
            printf("|_________________________________________|\n\r");
#endif
}
OS_TASK_FUNCTION(lsm6dsox_task, pvParameters)
{
        display_device_interface();
#if defined(dg_configUseI3CHandling)
         lsm6dsox_errcode ok = lsm6dsox_i3c_init();
#elif defined(dg_configUseI2CHandling)
         lsm6dsox_errcode ok = lsm6dsox_i2c_init();
#endif
         ASSERT_ERROR(ok== LSM6DOX_OK)
        for ( ;; ) {
#if defined(LSM6DSOX_FIFO)
            lsm6dsox_fifo();
#elif defined(LSM6DSOX_ACTIVITY_DETECT)
            lsm6dsox_activity();
#endif
        }
#if defined(dg_configUseI3CHandling)
        ad_i3c_close(lsm6dsox_handle, true);
#elif defined(dg_configUseI2CHandling)
        ad_i2c_close(lsm6dsox_handle, true);
#endif
        OS_TASK_DELETE( NULL ); //should never get here
}
