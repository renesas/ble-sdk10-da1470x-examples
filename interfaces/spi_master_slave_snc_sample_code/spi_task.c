
 /****************************************************************************************
 *
 * @file spi_tack.c
 *
 * @brief SPI demonstration example for SNC; Consumer and producer tasks definition
 *
 * Copyright (C) 2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#include "ad_spi.h"
#include "platform_devices.h"
#include "snc.h"
#include "app_shared_space.h"
#include "mailbox.h"
#include "rpmsg_lite.h"
#include "rpmsg_queue.h"
#include "rpmsg_platform.h"
#include "hw_clk.h"
#include "sys_watchdog.h"

/* Number of bytes sent in each TX iteration */
#define producer_DATA_LENGTH               10
/* Number of bytes retrieved in each RX iteration */
#define consumer_DATA_LENGTH               60

/* Buffer used to store the RX data received */
__RETAINED static uint8_t rx_data[consumer_DATA_LENGTH];
/* Semaphore to provide multi-thread protection for RETARGET operations */
__RETAINED static OS_MUTEX console_mutex;

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
#else
__RETAINED static app_shared_data_t *app_shared_data_ptr;
/* The shared buffer which contains the raw bytes received over the SPI bus. */
__SNC_SHARED static uint8_t app_rx_data[consumer_DATA_LENGTH];
#endif /* dg_configUSE_RPMSG_LITE */

/*
 * Helper macro to close a device. Code execution will be blocked until the device
 * is closed and for the max. timeout provided. If the timeout elapses and the
 * device cannot be close, a forceful close is attempted.
 */
#define APP_DEV_SPI_CLOSE(dev, timeout_ms)                                              \
                                                                                        \
                OS_TICK_TIME timeout = OS_GET_TICK_COUNT() + OS_MS_2_TICKS(timeout_ms); \
                                                                                        \
                while (ad_spi_close(dev, false) != AD_SPI_ERROR_NONE) {                 \
                        if (timeout <= OS_GET_TICK_COUNT()) {                           \
                                ad_spi_close(dev, true);                                \
                                break;                                                  \
                        }                                                               \
                        hw_clk_delay_usec(10);                                          \
                }

/* Helper function to fill a buffer with data */
static void generate_tx_data(uint8_t *data, uint16_t size)
{
        static uint8_t temp;

        for (int i = 0; i < size; i++) {
                *data++ = temp++;
        }
}

/* Callback function that trigger the SPI transmitter task */
static void spi_tx_timer_cb(OS_TIMER xTimer)
{
        /*
         * Blocking APIs cannot be invoked outside a task's scope. An attempt to do so, will raise compile errors.
         * Though the registered callback is not called from ISR context and because of the mentioned restriction
         * here we invoke the corresponding interrupt-safe API which does not block.
         */
        OS_EVENT_SIGNAL_FROM_ISR((OS_EVENT)OS_TIMER_GET_TIMER_ID(xTimer));
}

/*
 * Callback function to be called following a non-blocking SPI write complete.
 * The callback is called from interrupt context and so, we should not spend
 * too much time here. Any operations required should be deferred to a DG
 * co-routine by using events/notifications.
 */
static void spi_producer_cb(void *user_data, uint16_t transferred)
{
        OS_EVENT event = (OS_EVENT) user_data;
        OS_EVENT_SIGNAL_FROM_ISR(event);
}

/*
 * The task employs the SPI1 block instance in master mode, generates dummy data and transmits them over
 * the SPI bus at specified time intervals and with the help of an OS timer.
 */
OS_TASK_FUNCTION(spi_producer_task, pvParamsters)
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
        __APP_STATIC AD_SPI_ERROR spi_status;
        __APP_STATIC OS_TIMER spi_tx_tmr_h;
        __APP_STATIC ad_spi_handle_t spi_master_dev;
        __APP_STATIC uint8_t tx_data[producer_DATA_LENGTH];

        __APP_STATIC OS_EVENT spi_tx_event;
        __APP_STATIC OS_EVENT spi_tx_tmr_event;

        OS_MUTEX_CREATE(console_mutex);
        OS_EVENT_CREATE(spi_tx_event);
        OS_EVENT_CREATE(spi_tx_tmr_event);

        /*
         * Set up a timer that should trigger SPI write operations at regular time intervals.
         * The timer should be reloaded automatically.
         */
        spi_tx_tmr_h = OS_TIMER_CREATE("TX TRIGGER", OS_MS_2_TICKS(100), OS_TIMER_RELOAD,
                                                             spi_tx_tmr_event, spi_tx_timer_cb);
        ASSERT_WARNING(spi_tx_tmr_h);
        OS_TIMER_START(spi_tx_tmr_h, OS_TIMER_FOREVER);

        /*
         * Before performing any transaction on the SPI bus an SPI device should be opened.
         * By opening a device, an SPI block instance is enabled and initialized and the
         * associated I/O pins are configured.
         * It important to notice that the system cannot enter the sleep state as long as a device is opened.
         * The system can only enter the idle state (ARM WFI).
         */
        spi_master_dev = ad_spi_open(SPI_DEVICE_MASTER);

        for ( ;; ) {
                /* Wait for the next triggering event */
                OS_EVENT_WAIT(spi_tx_tmr_event, OS_EVENT_FOREVER);

                /* Generate some dummy values to transmit */
                generate_tx_data(tx_data, producer_DATA_LENGTH);

                /*
                 * When in master mode, it is important that the CS line is first activated;
                 * otherwise any following transactions will not take place.
                 */
                ad_spi_activate_cs(spi_master_dev);

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
                 *              handle = ad_spi_open(...);
                 *
                 *              // Perform non-blocking operations
                 *
                 *              ad_spi_close(handle, false);
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
                 *              sw_bsr_try_acquire(SYS_BSR_MASTER_SNC, SYS_BSR_PERIPH_ID_SPI1);
                 *              handle = ad_spi_open(...);
                 *
                 *              // Perform non-blocking read/write operations
                 *
                 *              ad_spi_close(handle, false);
                 *              sw_bsr_release(SYS_BSR_MASTER_SNC, SYS_BSR_PERIPH_ID_SPI1);
                 *
                 *       }
                 *
                 *       \endcode
                 *
                 */
                spi_status = ad_spi_write_async(spi_master_dev, tx_data, producer_DATA_LENGTH,
                                                                        spi_producer_cb, spi_tx_event);

                OS_MUTEX_GET(console_mutex, OS_MUTEX_FOREVER);

                if (spi_status != AD_SPI_ERROR_NONE) {
                        DBG_LOG("SPI Producer TX Error (code %d)\n\r", spi_status);
                } else {
                        DBG_LOG("SPI Producer TX OK!\n\r");

                        /*
                         * It is important that no further operations take place (by the same or other tasks)
                         * for as long as an SPI block instance is employed and actively used.
                         */
                        OS_EVENT_WAIT(spi_tx_event, OS_EVENT_FOREVER);
                }

                OS_MUTEX_PUT(console_mutex);

                /* Once the transaction is complete, the CS line can be de-asserted. */
                ad_spi_deactivate_cs_when_spi_done(spi_master_dev);
        }

        /* Recommended way to gracefully close a device */
        APP_DEV_SPI_CLOSE(spi_master_dev, 1000);

        /* Should not reach that point */
        OS_TASK_DELETE( NULL );

        /*
         * The end of a DG coroutine must be marked with this macro.
         * No effect, when in FreeRTOS context.
         */
        OS_TASK_END();
}

typedef struct {
        OS_EVENT spi_rx_event;
        uint32_t rx_data_cnt;
} rx_data_ctrl_t;

/*
 * Callback function to be called following a non-blocking SPI read complete.
 * The callback is called from interrupt context and so, we should not
 * spend too much time here. Any operations required should be deferred to a DG co-routine
 * by using events/notifications.
 */
static void spi_consumer_cb(void *user_data, uint16_t transferred)
{
        rx_data_ctrl_t *ctrl = (rx_data_ctrl_t *)user_data;

        ctrl->rx_data_cnt += transferred;

        OS_EVENT_SIGNAL_FROM_ISR(ctrl->spi_rx_event);
}

/* The task employs the SPI2 block instance in slave mode and performs read operations over the SPI bus. */
OS_TASK_FUNCTION(spi_consumer_task, pvParameters)
{
        OS_TASK_BEGIN();

        __APP_STATIC ad_spi_handle_t spi_slave_dev;
        __APP_STATIC rx_data_ctrl_t rx_data_ctrl;

        OPT_MEMSET((void *)&rx_data_ctrl, 0, sizeof(rx_data_ctrl_t));
        OS_EVENT_CREATE(rx_data_ctrl.spi_rx_event);

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
         * take place before any EP creations. Here, the queue is created for
         * demonstration only purposes as the SNC does not perform any receive operations.
         */
        remote_rpmsg_queue = rpmsg_queue_create(remote_rpmsg_instance_ptr);

        ASSERT_WARNING(remote_rpmsg_queue != RL_NULL);

        /* One or more EP should be declared and assigned a unique address. */
        remote_rpmsg_ept_ptr =
                rpmsg_lite_create_ept(remote_rpmsg_instance_ptr, APP_RPMSG_LITE_SNC_EPT_ADDR,
                                                rpmsg_queue_rx_cb, NULL, &remote_rpmsg_ept_context);

        ASSERT_WARNING(remote_rpmsg_ept_ptr == &remote_rpmsg_ept_context.ept);
#else
        /*
         * The shared data structure does not allocate any space for accommodating the raw data.
         * Instead, it contains a pointer that will eventually point to a buffer defined by the
         * developer. It is necessary that the buffer defined be marked with the '__SNC_SHARED'
         * attribute so it can be positioned the the shared memory pool and be accessible by other
         * remote masters.
         */
        app_shared_data_ptr = app_shared_space_data_get();
        app_shared_data_ptr->rx_data_ptr = app_rx_data;
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

        spi_slave_dev = ad_spi_open(SPI_DEVICE_SLAVE);

        for (;;) {
                /* When in the SPI slave mode, the CS line is automatically handled. */

                ad_spi_read_async(spi_slave_dev, &rx_data[rx_data_ctrl.rx_data_cnt],
                                                        consumer_DATA_LENGTH, spi_consumer_cb, &rx_data_ctrl);

                /*
                 * It's OK to wait even if the read operation fails to be executed; if so,
                 * the timeout will unblock the task.
                 */
                OS_EVENT_WAIT(rx_data_ctrl.spi_rx_event, OS_MS_2_TICKS(3000));

                OS_MUTEX_GET(console_mutex, OS_MUTEX_FOREVER);

                /*
                 * Check if the pre-defined number of bytes have been received and if so,
                 * copy them in the shared space and notify the SYSCPU remote master to
                 * further process them.
                 */
                if (rx_data_ctrl.rx_data_cnt >= consumer_DATA_LENGTH) {
                        DBG_LOG("Successfully received [%lu] bytes \n\r", rx_data_ctrl.rx_data_cnt);

#if dg_configUSE_RPMSG_LITE
                        int32_t rpmsg_status =
                                rpmsg_lite_send(remote_rpmsg_instance_ptr, remote_rpmsg_ept_ptr,
                                        (uint32_t)APP_RPMSG_LITE_SYSCPU_EPT_ADDR, (void *)rx_data,
                                                                        rx_data_ctrl.rx_data_cnt, RL_BLOCK);

                        ASSERT_WARNING(rpmsg_status == RL_SUCCESS);
#else
                        /*
                         * Use this custom-defined semaphore just before accessing shared data to avoid cases
                         * where both masters operate on the data simultaneously.
                         */
                        app_semph_take();

                        /*
                         * We simply copy the received SPI raw data to the shared buffer and update
                         * the counter which indicates the number of bytes available in the buffer.
                         */
                        size_t cnt =
                                (rx_data_ctrl.rx_data_cnt > consumer_DATA_LENGTH) ? consumer_DATA_LENGTH : rx_data_ctrl.rx_data_cnt;
                        OPT_MEMCPY((void *)app_shared_data_ptr->rx_data_ptr, rx_data, cnt);
                        app_shared_data_ptr->rx_data_size = cnt;

                        app_semph_give();

                        app_shared_data_notify_syscpu();
#endif /* dg_configUSE_RPMSG_LITE */

                        rx_data_ctrl.rx_data_cnt = 0;
                } else {
                        DBG_LOG("Consumer task; bytes threshold not reached yet...\n\r");
                }

                OS_MUTEX_PUT(console_mutex);
        }

        APP_DEV_SPI_CLOSE(spi_slave_dev, 1000);

        OS_TASK_DELETE( NULL );

        OS_TASK_END();
}
