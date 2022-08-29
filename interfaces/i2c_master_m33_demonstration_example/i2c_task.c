
 /****************************************************************************************
 *
 * @file i2c_task.c
 *
 * @brief I2C adapter demonstration example in SYSCPU context - THERMO3 click board task
 *
 * Copyright (C) 2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "ad_i2c.h"
#include "peripheral_setup.h"
#include "platform_devices.h"
#include "hw_clk.h"
#include "sys_timer.h"
#include "tmp102_reg.h"
#include "math.h"
#include "app_common.h"

/* Semaphore to provide multi-thread protection for RETARGET operations */
__RETAINED static OS_MUTEX console_mutex;

__RETAINED static uint16_t i2c_transferred;

/*
 * Callback function called once a non-blocking I2C transaction is complete.
 * The callback is called from interrupt context and so we should not spend
 * too much time here.
 */
static void i2c_cb_preempt(void *user_data, HW_I2C_ABORT_SOURCE error)
{
        *((uint16_t *)user_data) = error;
}

/* Helper macro to perform a non-blocking I2C write operation with timeout */
void app_i2c_write_preempt(void *handle, uint8_t *wbuf, size_t wlen, uint8_t flags,
                                                                                uint64_t timeout)
{
              i2c_transferred = (uint16_t)-1;
              uint64_t cur_timeout_tick = sys_timer_get_uptime_ticks();

              ad_i2c_write_async(handle, wbuf, wlen, i2c_cb_preempt,
                                                        &i2c_transferred, flags);
              while (i2c_transferred == (uint16_t)-1) {
                      if (sys_timer_get_uptime_ticks() - cur_timeout_tick > timeout) {
                              break;
                      }
                      OS_DELAY_MS(5);
              }
}

/* Helper macro to perform a non-blocking I2C read operation with timeout */
void app_i2c_write_read_preempt(void *handle, uint8_t *wbuf, size_t wlen, uint8_t *rbuf,
                                                    size_t rlen, uint8_t flags, uint64_t timeout)
{
              i2c_transferred = (uint16_t)-1;
              uint64_t cur_timeout_tick = sys_timer_get_uptime_ticks();

              ad_i2c_write_read_async(handle, wbuf, wlen, rbuf, rlen,
                                        i2c_cb_preempt, &i2c_transferred, flags);
              while (i2c_transferred == (uint16_t)-1) {
                      if (sys_timer_get_uptime_ticks() - cur_timeout_tick > timeout) {
                              break;
                      }
                      OS_DELAY_MS(5);
              }
}

/* Callback function to perform a write access to the TMP102 sensor */
int32_t app_tmp102_write_reg(void *dev, uint8_t reg, uint8_t *wbuf, size_t wlen)
{
        uint8_t tx_buf[ 256 ];
        ASSERT_WARNING(wlen < sizeof(tx_buf));

        /*
         * The first byte should be the address pointer that indicates which register
         * should respond to the write access.
         */
        tx_buf[ 0 ] = reg;

        /* Then copy the actual information that should be written to the address pointer provided. */
        for (int i = 1; i <= wlen; i++) {
                tx_buf[ i ] = wbuf[ i - 1 ];
        }

        /* Employ the I2C adapter to perform the actual write access */
        app_i2c_write_preempt(dev, tx_buf, wlen + 1, HW_I2C_F_ADD_STOP, OS_EVENT_FOREVER);

        return 0;
}

/* Callback function to perform a read access to the TMP102 sensor */
int32_t app_tmp102_read_reg(void *handle, uint8_t reg, uint8_t *rbuf, size_t rlen)
{
        /* Employ the I2C adapter to perform the actual read access */
        app_i2c_write_read_preempt(handle, &reg, 1, rbuf, rlen,
                        HW_I2C_F_ADD_RESTART | HW_I2C_F_ADD_STOP, OS_EVENT_FOREVER);

        return 0;
}

/* Context used to drive the TMP102 sensor */
__RETAINED_RW static tmp102_ctx_t tmp102_ctx = {
        .write_reg = app_tmp102_write_reg,
        .read_reg = app_tmp102_read_reg,
        .handle = NULL,
};

__RETAINED static OS_EVENT thermo3_alert_event;

/*
 * Helper macro to get the current temperature.
 *
 * \param[in]  hdl           The handler of the I2C device which should be used for the target operation.
 *
 * \param[out] temperature   The raw temperature value (as read from the register of the sensor).
 *                           To convert it to a real value use the TMP102_RAW_TO_FLOAT() helper macro.
 *
 * \param[out] ret           Status of the target operation. A zero value indicates that operation
 *                           has been executed successfully.
 */
#define app_tmp102_read_temperature(hdl, temperature, ret)          \
        tmp102_ctx.handle = hdl;                                    \
        ret = tmp102_temperature_read(&tmp102_ctx, &temperature)

/*
 * Helper macro to set the low temperature threshold.
 *
 * \param[in] hdl            The handler of the I2C device which should be used for the target operation.
 *
 * \param[in] low_limit      The low temperature threshold. The value should reflect the raw value
 *                           that should be written to the corresponding register. User should use
 *                           the TMP102_FLOAT_TO_RAW() helper macro to convert a real value to the
 *                           corresponding raw value.
 *
 * \param[in]  ext_res       True if \p low_limit reflects a 13-bit value; false if it reflects a 12-bit value.
 *
 * \param[out] ret           Status of the target operation. A zero value indicates that operation
 *                           has been executed successfully.
 */
#define app_tmp102_set_low_limit(hdl, low_limit, ext_res, ret)        \
        tmp102_ctx.handle = hdl;                                             \
        ret = tmp102_low_limit_set(&tmp102_ctx, low_limit, ext_res)

/*
 * Helper macro to set the high temperature threshold.
 *
 * \param[in] hdl            The handler of the I2C device which should be used for the target operation.
 *
 * \param[in] high_limit     The high temperature threshold. The value should reflect the raw value
 *                           that should be written to the corresponding register. User should use
 *                           the TMP102_FLOAT_TO_RAW() helper macro to convert a real value to the
 *                           corresponding raw value.
 *
 * \param[in]  ext_res       True if \p high_limit reflects a 13-bit value; false if it reflects a 12-bit value.
 *
 * \param[out] ret           Status of the target operation. A zero value indicates that operation
 *                           has been executed successfully.
 */
#define app_tmp102_set_high_limit(hdl, high_limit, ext_res, ret)        \
        tmp102_ctx.handle = hdl;                                        \
        ret = tmp102_high_limit_set(&tmp102_ctx, high_limit, ext_res)

/*
 * Helper macro to get the low temperature threshold.
 *
 * \param[in]  hdl           The handler of the I2C device which should be used for the target operation.
 *
 * \param[out] low_limit     The low temperature threshold. The value should reflect the raw value as read
 *                           from the corresponding register. User should use the TMP102_RAW_TO_FLOAT()
 *                           helper macro to convert it to a real temperature value.
 *
 * \param[in]  ext_res       True if \p low_limit reflects a 13-bit value; false if it reflects a 12-bit value.
 *
 * \param[out] ret           Status of the target operation. A zero value indicates that operation
 *                           has been executed successfully.
 */
#define app_tmp102_get_low_limit(hdl, low_limit, ext_res, ret)         \
        tmp102_ctx.handle = hdl;                                       \
        ret = tmp102_low_limit_get(&tmp102_ctx, &low_limit, ext_res)

/*
 * Macro to get the high temperature threshold.
 *
 * \param[in]  hdl           The handler of the I2C device which should be used for the target operation.
 *
 * \param[out] high_limit    The low temperature threshold. The value should reflect the raw value as read
 *                           from the corresponding register. User should use the TMP102_RAW_TO_FLOAT()
 *                           helper macro to convert it to a real temperature value.
 *
 * \param[in]  ext_res       True if \p high_limit reflects a 13-bit value; false if it reflects a 12-bit value.
 *
 * \param[out] ret           Status of the target operation. A zero value indicates that operations
 *                           has been executed successfully.
 */
#define app_tmp102_get_high_limit(hdl, high_limit, ext_res, ret)          \
        tmp102_ctx.handle = hdl;                                          \
        ret = tmp102_high_limit_get(&tmp102_ctx, &high_limit, ext_res)

/*
 * Helper macro to configure the sensor. The TMP102 sensor exhibits a single 16-bit control register.
 *
 * \param[in] hdl            The handler of the I2C device which should be used for the target operation.
 *
 * \param[in] cfg            The configuration value that should be written to the corresponding register.
 *                           User can use the \sa tmp102_cfg_t structure to write the value either
 *                           directly or use the bit-field definitions to get each sub-entry separately.
 *
 * \param[out] ret           Status of the target operation. A zero value indicates that operation
 *                           has been executed successfully.
 */
#define app_tmp102_set_configuration(hdl, cfg, ret)         \
        tmp102_ctx.handle = hdl;                            \
        ret = tmp102_configuration_set(&tmp102_ctx, &cfg)

/*
 * Helper macro to get sensor's control value. The TMP102 sensor exhibits a single 16-bit control register.
 *
 * \param[in] hdl            The handler of the I2C device which should be used for the target operation.
 *
 * \param[in] cfg            The configuration value as read from the corresponding register. User can
 *                           use the \sa tmp102_cfg_t structure to read the value either directly or
 *                           use the  bit-field definitions to get each sub-entry separately.
 *
 * \param[out] ret           Status of the target operation. A zero value indicates that operation
 *                           has been executed successfully.
 */
#define app_tmp102_get_configuration(hdl, cfg, ret)          \
        tmp102_ctx.handle = hdl;                             \
        ret = tmp102_configuration_get(&tmp102_ctx, &cfg)


/*
 * Helper macro to to gracefully close an adapter instance.
 *
 * \param[in] dev          The handler of the I2C device which should be closed
 *
 * \param[in] timeout_ms   Device close timeout expressed in millisecond. Once the timeout
 *                         elapses and the device is still opened then the latter is closed
 *                         forcefully.
 */
#define APP_DEV_I2C_CLOSE(dev, timeout_ms)                                              \
                                                                                        \
                OS_TICK_TIME timeout = OS_GET_TICK_COUNT() + OS_MS_2_TICKS(timeout_ms); \
                                                                                        \
                while (ad_i2c_close(dev, false) != AD_I2C_ERROR_NONE) {                 \
                        if (timeout <= OS_GET_TICK_COUNT()) {                           \
                                ad_i2c_close(dev, true);                                \
                                break;                                                  \
                        }                                                               \
                        OS_DELAY_MS(10);                                                \
                }

/* Helper function to convert a float number to string */
static void app_float_2_string(char * str, float x)
{
    uint8_t singles = fmod(x, 10);
    uint8_t tens = (fmod(x, 100) - singles) / 10;
    uint8_t deci = fmod(x*10, 10);

    str[0] = '+';

    if(x<0)
    {
        str[0] = '-';
    }
    str[1] = tens + '0';
    str[2] = singles+ '0';
    str[3] = '.';
    str[4] = deci + '0';
    str[5] = '\0';

}

/*
 * This routine should be called when an alert signal is asserted so, the target task starts
 * reading the temperature value. Typically, this call is done from the WKUP user callback
 * function (in this example defined in the main.c source file).
 */
void thermo3_alert_signal(void)
{
        /*
         * Make sure the semaphore has been created. It might happen that the WKUP
         * controller gets triggered before the main task is up and running.
         */
        if (thermo3_alert_event) {
                if (in_interrupt()) {
                        OS_EVENT_SIGNAL_FROM_ISR(thermo3_alert_event);
                } else {
                        OS_EVENT_SIGNAL(thermo3_alert_event);
                }
        }
}

/*
 * Task to interact with the THERMO3 click board; the task is notified once the alert signal is
 * asserted and reads the temperature value every 1'' and for as long as the alert signal
 * is asserted.
 */
OS_TASK_FUNCTION(termo3_task, pvParameters)
{
        OS_TASK_BEGIN();

        DBG_LOG("Running thermo3 sensor demo...\n\r");

        ad_i2c_handle_t i2c_master_dev;
        __UNUSED int32_t ret;
        char temp_str[10];
        int16_t val;

        OS_MUTEX_CREATE(console_mutex);
        OS_EVENT_CREATE(thermo3_alert_event);

        /* Open the I2C device which will be used to interact with the THERMO3 temperature sensor. */
        i2c_master_dev = ad_i2c_open(I2C_DEVICE_MASTER);
        ASSERT_WARNING(i2c_master_dev);

        /* Set the Thigh temperature threshold */
        app_tmp102_set_high_limit(i2c_master_dev, TMP102_FLOAT_TO_RAW(THERMO3_ALERT_T_HIGH), false, ret);
        /* Set the Tlow temperature threshold */
        app_tmp102_set_low_limit(i2c_master_dev, TMP102_FLOAT_TO_RAW(THERMO3_ALERT_T_LOW), false, ret);

        /* Read back the alert boundaries; just for demonstration purposes */
        app_tmp102_get_high_limit(i2c_master_dev, val, false, ret);
        ASSERT_WARNING((float)THERMO3_ALERT_T_HIGH == TMP102_RAW_TO_FLOAT(val));

        app_tmp102_get_low_limit(i2c_master_dev, val, false, ret);
        ASSERT_WARNING((float)THERMO3_ALERT_T_LOW == TMP102_RAW_TO_FLOAT(val));

        /*
         * Configure the sensor in comparator mode. In this mode the alert signal is asserted once
         * the temperature exceeds the high boundary and for as long as the temperate falls below
         * the low boundary (for more info refer to the datasheet of the sensor).
         */
        tmp102_cfg_t cfg_w = {
                .extended_mode   = TMP102_EXTENDED_MODE_12_BIT,
                .conversion_rate = TMP102_CONVERSION_RATE_8_HZ,
                .shutdown_mode   = false,
                .thermostat_mode = TMP102_MODE_COMPARATOR,
                .alert_polarity  = THERMO3_ALEERT_POL,
                .fault_queue     = TMP102_FAULT_QUEUE_1_SAMPLE,
                .one_shot        = false,
        };
        app_tmp102_set_configuration(i2c_master_dev, cfg_w, ret);

        for (;;) {
                /* Wait for the alert signal to be asserted based on the alert polarity */
                OS_EVENT_WAIT(thermo3_alert_event, OS_EVENT_FOREVER);

                OS_MUTEX_GET(console_mutex, OS_MUTEX_FOREVER);

                do {
                        app_tmp102_read_temperature(i2c_master_dev, val, ret);

                        /* Convert the float value to string */
                        app_float_2_string(temp_str, TMP102_RAW_TO_FLOAT(val));
                        DBG_LOG("Temperature: %s\n\r", temp_str);

                        OS_DELAY_MS(1000);

                /* Get a new temperature value for as long as the alert signal is asserted */
                } while (hw_gpio_get_pin_status(THERMO3_ALERT_PORT, THERMO3_ALERT_PIN) ==
                        (THERMO3_ALEERT_POL == TMP102_ALERT_POLARITY_ACTIVE_HIGH ? true : false));

                OS_MUTEX_PUT(console_mutex);
                fflush(stdout);
        }

        APP_DEV_I2C_CLOSE(i2c_master_dev, 1000);

        OS_TASK_DELETE( NULL );

        OS_TASK_END();
}
