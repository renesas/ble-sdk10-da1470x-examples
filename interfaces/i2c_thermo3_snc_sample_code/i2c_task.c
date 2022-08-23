
 /****************************************************************************************
 *
 * @file i2c_tack.c
 *
 * @brief I2C adapter demonstration example in SNC context - THERMO3 click board task
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
#include "snc.h"
#include "app_shared_space.h"
#include "mailbox.h"
#include "rpmsg_lite.h"
#include "rpmsg_queue.h"
#include "rpmsg_platform.h"
#include "hw_clk.h"
#include "sys_timer.h"
#include "tmp102_reg.h"

/* Semaphore to provide multi-thread protection for RETARGET operations */
__RETAINED static OS_MUTEX console_mutex;

__RETAINED static uint16_t i2c_transferred;

/*
 * Callback function called once a non-blocking I2C transaction is complete.
 * The callback is called from interrupt context and so we should not spend
 * too much time here.
 */
static void i2c_cb_no_preempt(void *user_data, HW_I2C_ABORT_SOURCE error)
{
        *((uint16_t *)user_data) = error;
}

/* Helper macro to perform a non-blocking I2C write operation with timeout */
#define __ad_i2c_write_no_preempt(handle, wbuf, wlen, i2c_flags, timeout) \
        {                                                                 \
              i2c_transferred = (uint16_t)-1;                             \
              uint64_t __cur_timeout_tick = sys_timer_get_uptime_ticks(); \
              ad_i2c_write_async((handle), (wbuf), (wlen), i2c_cb_no_preempt, &i2c_transferred, (i2c_flags)); \
              while (i2c_transferred == (uint16_t)-1) {                   \
                      if (sys_timer_get_uptime_ticks() - __cur_timeout_tick > timeout) { \
                              break;                                      \
                      }                                                   \
                      hw_clk_delay_usec(5);                               \
              }                                                           \
        }

/* Helper macro to perform a non-blocking I2C read operation with timeout */
#define __ad_spi_write_read_no_preempt(handle, wbuf, wlen, rbuf, rlen, i2c_flags, timeout) \
        {                                                                 \
              i2c_transferred = (uint16_t)-1;                             \
              uint64_t __cur_timeout_tick = sys_timer_get_uptime_ticks(); \
              ad_i2c_write_read_async((handle), (wbuf), (wlen), (rbuf), (rlen), i2c_cb_no_preempt, &i2c_transferred, (i2c_flags)); \
              while (i2c_transferred == (uint16_t)-1) {                   \
                      if (sys_timer_get_uptime_ticks() - __cur_timeout_tick > timeout) { \
                              break;                                      \
                      }                                                   \
                      hw_clk_delay_usec(5);                               \
              }                                                           \
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

        for (int i = 1; i <= wlen; i++) {
                tx_buf[ i ] = wbuf[ i - 1 ];
        }

        /* Employ the I2C adapter to perform the actual write access */
        __ad_i2c_write_no_preempt(dev, tx_buf, wlen + 1, HW_I2C_F_ADD_STOP, OS_EVENT_FOREVER);

        return 0;
}

/* Callback function to perform a read access to the TMP102 sensor */
int32_t app_tmp102_read_reg(void *handle, uint8_t reg, uint8_t *rbuf, size_t rlen)
{
        /* Employ the I2C adapter to perform the actual read access */
        __ad_spi_write_read_no_preempt(handle, &reg, 1, rbuf, rlen,
                        HW_I2C_F_ADD_RESTART | HW_I2C_F_ADD_STOP, OS_EVENT_FOREVER);
        return 0;
}

/* Context used to drive the TMP102 sensor */
__RETAINED_RW static tmp102_ctx_t tmp102_ctx = {
        .write_reg = app_tmp102_write_reg,
        .read_reg = app_tmp102_read_reg,
        .handle = NULL,
};

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
 * \param[in]                True if \p low_limit reflects a 13-bit value; false if it reflects a 12-bit value.
 *
 * \param[out] ret           Status of the target operation. A zero value indicates that operation
 *                           has been executed successfully.
 */
#define app_tmp102_set_low_limit(hdl, low_limit, extension_mode, ret)        \
        tmp102_ctx.handle = hdl;                                             \
        ret = tmp102_low_limit_set(&tmp102_ctx, low_limit, extension_mode)

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
 * \param[in]                True if \p high_limit reflects a 13-bit value; false if it reflects a 12-bit value.
 *
 * \param[out] ret           Status of the target operation. A zero value indicates that operation
 *                           has been executed successfully.
 */
#define app_tmp102_set_high_limit(hdl, high_limit, extension_mode, ret)        \
        tmp102_ctx.handle = hdl;                                               \
        ret = tmp102_high_limit_set(&tmp102_ctx, high_limit, extension_mode)

/*
 * Helper macro to get the low temperature threshold.
 *
 * \param[in]  hdl           The handler of the I2C device which should be used for the target operation.
 *
 * \param[out] low_limit     The low temperature threshold. The value should reflect the raw value as read
 *                           from the corresponding register. User should use the TMP102_RAW_TO_FLOAT()
 *                           helper macro to convert it to a real temperature value.
 *
 * \param[in]                True if \p low_limit reflects a 13-bit value; false if it reflects a 12-bit value.
 *
 * \param[out] ret           Status of the target operation. A zero value indicates that operation
 *                           has been executed successfully.
 */
#define app_tmp102_get_low_limit(hdl, low_limit, extension_mode, ret)         \
        tmp102_ctx.handle = hdl;                                              \
        ret = tmp102_low_limit_get(&tmp102_ctx, &low_limit, extension_mode)

/*
 * Macro to get the high temperature threshold.
 *
 * \param[in]  hdl           The handler of the I2C device which should be used for the target operation.
 *
 * \param[out] high_limit    The low temperature threshold. The value should reflect the raw value as read
 *                           from the corresponding register. User should use the TMP102_RAW_TO_FLOAT()
 *                           helper macro to convert it to a real temperature value.
 *
 * \param[in]                True if \p high_limit reflects a 13-bit value; false if it reflects a 12-bit value.
 *
 * \param[out] ret           Status of the target operation. A zero value indicates that operations
 *                           has been executed successfully.
 */
#define app_tmp102_get_high_limit(hdl, high_limit, extension_mode, ret)          \
        tmp102_ctx.handle = hdl;                                                 \
        ret = tmp102_high_limit_get(&tmp102_ctx, &high_limit, extension_mode)

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

#if dg_configUSE_RPMSG_LITE
__RETAINED static struct rpmsg_lite_instance *remote_rpmsg_instance_ptr;
__RETAINED static rpmsg_queue_handle remote_rpmsg_queue;
__RETAINED static struct rpmsg_lite_endpoint *remote_rpmsg_ept_ptr;
/*
 * The RPMSG-Lite framework is configured to use static contexts meaning that
 * configuration structures should be allocated and provided explicitly at
 * application level.
 */
__RETAINED static struct rpmsg_lite_instance remote_rpmsg_instance;
__RETAINED static struct rpmsg_lite_ept_static_context remote_rpmsg_ept_context;
#endif /* dg_configUSE_RPMSG_LITE */

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
                        hw_clk_delay_usec(10);                                          \
                }

static void i2c_master_tmr_cb(OS_TIMER xTimer)
{
        /*
         * Blocking APIs cannot be invoked outside a task's scope. An attempt to do so, will raise compile errors.
         * Though the registered callback is not called from ISR context and because of the mentioned restriction
         * here we invoke the corresponding interrupt-safe API which does not block.
         */
        OS_EVENT_SIGNAL_FROM_ISR((OS_EVENT)OS_TIMER_GET_TIMER_ID(xTimer));
}

/*
 * Task to interact with the THERMO3 click board; the task is notified at a predefined time
 * interval and reads a single temperature value.
 */
OS_TASK_FUNCTION(termo3_task, pvParameters)
{
        /*
         * The start of a DG coroutine must be marked with this macro.
         * No effect, when in FreeRTOS context.
         */
        OS_TASK_BEGIN();

        /*
         * The SNC context employs a co-operative scheduler which adopts a single stack environment;
         * in contrast to the FreeRTOS environment where each task is assigned a separate stack
         * instance.
         * As a result, local variables of a task might loose their values following a context switch.
         * (e.g waiting for a semaphore). This requires that local variables that should retain their
         * values after the blocking executing point be statically declared.
         */
        __APP_STATIC ad_i2c_handle_t i2c_master_dev;
        __APP_STATIC OS_TIMER i2c_master_tmr_h;
        __APP_STATIC OS_EVENT i2c_master_tmr_event;
        __APP_STATIC int16_t val;
        __UNUSED int32_t ret;

        OS_EVENT_CREATE(i2c_master_tmr_event);
        OS_MUTEX_CREATE(console_mutex);

        /*
         * Set up a timer that will trigger the task at specified time intervals.
         * The timer should be reloaded automatically.
         */
        i2c_master_tmr_h = OS_TIMER_CREATE("TEMP. TRIGGER", OS_MS_2_TICKS(500), OS_TIMER_RELOAD,
                                                                i2c_master_tmr_event, i2c_master_tmr_cb);
        OS_TIMER_START(i2c_master_tmr_h, OS_TIMER_FOREVER);

#if dg_configUSE_RPMSG_LITE
        /*
         * The SNC should be initialized as remote device. If this is not the case then
         * the RPMsg-Lite establishment will fail.
         */
        remote_rpmsg_instance_ptr =
                rpmsg_lite_remote_init(platform_get_base_addr(),
                        RL_PLATFORM_DA1470X_M33_SNC_LINK_ID, RL_NO_FLAGS, &remote_rpmsg_instance);

        ASSERT_WARNING(remote_rpmsg_instance_ptr == &remote_rpmsg_instance);

        /*
         * Queue initialization is required for blocking receive operations and should
         * take place before any EP creations. Here the queue is created for
         * demonstration only purposes as the SNC does not perform any receive operations.
         */
        remote_rpmsg_queue = rpmsg_queue_create(remote_rpmsg_instance_ptr);

        ASSERT_WARNING(remote_rpmsg_queue != RL_NULL);

        /* One or more EP should be declared and assigned a unique address. */
        remote_rpmsg_ept_ptr =
                rpmsg_lite_create_ept(remote_rpmsg_instance_ptr, APP_RPMSG_LITE_SNC_EPT_ADDR,
                                                rpmsg_queue_rx_cb, NULL, &remote_rpmsg_ept_context);

        ASSERT_WARNING(remote_rpmsg_ept_ptr == &remote_rpmsg_ept_context.ept);
#endif

        /*
         * Once the user-defined shared memory if fully allocated and initialized it's time for the
         * ready flag to be updated, thus indicating to all masters that the memory pool is now
         * accessible.
         *
         * \warning When the RPMSG-Lite framework is used, it is necessary that the remote role (SNC)
         *          is configured before the master role (SYSCPU) is established.
         */
        app_shared_space_ctrl_set_ready();

        i2c_master_dev = ad_i2c_open(I2C_DEVICE_MASTER);
        ASSERT_WARNING(i2c_master_dev);

        /*
         * When in SNC context the following restrictions should be taken into consideration
         * by the developer:
         *
         * 1. Only non-blocking operations can be executed. The main reason is that OS operations
         *    that result in blocking a task, should be done only within a tasks's scope.
         *
         * 2. The following restrictions is a function of the above statement. It's user's responsibility
         *    to:
         *
         *     2a. Acquire/release the corresponding resources using a custom MUTEX (if a resource
         *         is acquired by multiple DG co-routines).
         *
         *         A possible implementation is briefly demonstrated:
         *
         *       \code{.c}
         *
         *       OS_MUTEX static resource_protection;
         *       OS_MUTEX_CREATE(resource_protection);
         *
         *       ad_spi_handle_t handle;
         *
         *       for (;;) {
         *
         *              OS_MUTEX_GET(resource_protection);
         *              handle = ad_i2c_open(...);
         *
         *              // Perform non-blocking operations
         *
         *              ad_i2c_close(handle, false);
         *              OS_MUTEX_PUT(resource_protection);
         *
         *       }
         *
         *       \endcode
         *
         *     2b. To program the busy status register (BSR) providing multi-core protection
         *         (if multiple masters attempt to access a peripheral concurrently).
         *
         *       A possible implementation is briefly provided:
         *
         *       \code{.c}
         *
         *       ad_spi_handle_t handle;
         *
         *       for(;;) {
         *
         *              sw_bsr_try_acquire(SYS_BSR_MASTER_SNC, SYS_BSR_PERIPH_ID_I2C1);
         *              handle = ad_i2c_open(...);
         *
         *              // Perform non-blocking read/write operations
         *
         *              ad_i2c_close(handle, false);
         *              sw_bsr_release(SYS_BSR_MASTER_SNC, SYS_BSR_PERIPH_ID_I2C1);
         *
         *       }
         *
         *       \endcode
         *
         */
        for (;;) {
                /* Wait for the OS timer to expire */
                OS_EVENT_WAIT(i2c_master_tmr_event, OS_EVENT_FOREVER);

                app_tmp102_read_temperature(i2c_master_dev, val, ret);

                OS_MUTEX_GET(console_mutex, OS_MUTEX_FOREVER);

#if dg_configUSE_RPMSG_LITE
                        /* Send the raw temperature value(s) to the peer device. */
                        int32_t rpmsg_status =
                                rpmsg_lite_send(remote_rpmsg_instance_ptr, remote_rpmsg_ept_ptr,
                                        (uint32_t)APP_RPMSG_LITE_SYSCPU_EPT_ADDR, (void *)&val,
                                                                                sizeof(val), RL_BLOCK);

                        ASSERT_WARNING(rpmsg_status == RL_SUCCESS);
#else
                        /* Push sensor data into the shared space */
                        if (app_shared_space_data_queue_push((uint8_t *)&val, sizeof(val))) {
                                DBG_LOG("Data pushed into the shared space...\n\r");
                        } else {
                                DBG_LOG("No more space available in the shared space...\n\r");
                        }

                        /*
                         * Check if the predefined number of chunks have been pushed to the shared space
                         * and if so trigger the SYSCPU master to further process them.
                         */
                        if (app_shared_space_data_get_alloc_chunks() >= APP_DATA_NUM_OF_CHUNKS) {
                                DBG_LOG("%lu chunks are occupied; time to notify the remote master...\n\r",
                                        app_shared_space_data_get_alloc_chunks());
                                app_shared_data_notify_syscpu();
                        }

#endif /* dg_configUSE_RPMSG_LITE */

                OS_MUTEX_PUT(console_mutex);
                fflush(stdout);
        }

        APP_DEV_I2C_CLOSE(i2c_master_dev, 1000);

        OS_TASK_DELETE( NULL );

        /*
         * The end of a DG coroutine must be marked with this macro.
         * No effect, when in FreeRTOS context.
         */
        OS_TASK_END();
}
