/**
 ****************************************************************************************
 *
 * @file lsm6dsox_task.h
 *
 * @brief lsm6dsox task header file
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
#ifndef _LSM6DSOXC_TASK_H_
#define _LSM6DSOXC_TASK_H_
#include<stdint.h>

/**
 * @brief lsm6dsox task: handling lsm6dsox functionalities
 */
void lsm6dsox_task( void *pvParameters );
int lsm6dsox_i2c_init(void);
int lsm6dsox_i2c_reg_write(uint8_t reg,  const uint8_t *bufp,uint16_t len);
int lsm6dsox_i2c_reg_read(uint8_t reg,uint8_t *bufp,uint16_t len);
int lsm6dsox_i3c_init(void);
int lsm6dsox_i3c_reg_write(uint8_t reg,  const uint8_t *bufp,uint16_t len);
int lsm6dsox_i3c_reg_read(uint8_t reg,uint8_t *bufp,uint16_t len);
typedef enum {
LSM6DOX_OK=0,
EINVAL=1,
EIO=2,
}lsm6dsox_errcode;
#endif /* _LSM6DSOXC_TASK_H_ */
