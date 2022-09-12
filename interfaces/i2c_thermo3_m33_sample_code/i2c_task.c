
 /****************************************************************************************
 *
 * @file spi_tack.c
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

#include "snc.h"
#include "osal.h"
#include "app_shared_space.h"
#include "mailbox.h"
#include "rpmsg_lite.h"
#include "rpmsg_queue.h"
#include "rpmsg_platform.h"
#include "math.h"
#include "tmp102_reg.h"

/* Task notifications. */
#define TASK_SNC_NOTIF              ( 1 << 0 )

__RETAINED static OS_TASK task_h;

#if dg_configUSE_RPMSG_LITE
__RETAINED static struct rpmsg_lite_instance *master_rpmsg_instance_ptr;
__RETAINED static rpmsg_queue_handle master_rpmsg_queue;
__RETAINED static struct rpmsg_lite_endpoint *master_rpmsg_ept_ptr;
__RETAINED static struct rpmsg_lite_instance master_rpmsg_instance;
__RETAINED static struct rpmsg_lite_ept_static_context master_rpmsg_ept_context;
#else
/* Callback function to be called within SNC2SYS or MAILBOX handler. */
static void app_snc2sys_cb(void) {
        if (task_h) {
                OS_TASK_NOTIFY_FROM_ISR(task_h, TASK_SNC_NOTIF, OS_NOTIFY_SET_BITS);
        }
}
#endif /* dg_configUSE_RPMSG_LITE */

/* Helper function to convert a float number to string */
static void app_float_2_string(char * str, float x)
{
    uint8_t singles = fmod(x, 10);
    uint8_t tens = (fmod(x, 100) - singles) / 10;
    uint8_t deci = fmod(x * 10, 10);

    str[0] = '+';

    if( x < 0 )
    {
        str[0] = '-';
    }
    str[1] = tens + '0';
    str[2] = singles+ '0';
    str[3] = '.';
    str[4] = deci + '0';
    str[5] = '\0';
}

/**
 * Task responsible for fetching and printing on the serial console data received
 * over the I2C bus in SNC context.
 */
OS_TASK_FUNCTION(thermo3_task, pvParameters)
{
        /*
         * A DG Coroutine should start with this macro structure.
         * No effect if called in FreeRTOS context.
         */
        OS_TASK_BEGIN();

        DBG_LOG("M33 TASK IS RUNNING...\n\r");

        task_h = OS_GET_CURRENT_TASK();

#if dg_configUSE_RPMSG_LITE
        /*
         * The SYSCPU should be initialized as master device. If this is not the case then
         * the RPMsg-Lite establishment will fail.
         */
        master_rpmsg_instance_ptr =
                rpmsg_lite_master_init(platform_get_base_addr(), RL_PLATFORM_SH_MEM_SIZE,
                        RL_PLATFORM_DA1470X_M33_SNC_LINK_ID, RL_NO_FLAGS, &master_rpmsg_instance);

        ASSERT_WARNING(master_rpmsg_instance_ptr == &master_rpmsg_instance);

        /*
         * Queue initialization is required for blocking RPMSG-lite receive operations and should
         * take place before any EndPoint creation.
         */
        master_rpmsg_queue = rpmsg_queue_create(master_rpmsg_instance_ptr);

        ASSERT_WARNING(master_rpmsg_queue != RL_NULL);

        /* One or more EP should be declared and assigned a unique address. */
        master_rpmsg_ept_ptr =
                rpmsg_lite_create_ept(master_rpmsg_instance_ptr, APP_RPMSG_LITE_SYSCPU_EPT_ADDR,
                                        rpmsg_queue_rx_cb, master_rpmsg_queue, &master_rpmsg_ept_context);

        ASSERT_WARNING(master_rpmsg_ept_ptr == &master_rpmsg_ept_context.ept);
#else
        uint32_t notif;

#if dg_configUSE_MAILBOX
        /*
         * The signaling between remote masters is based on system interrupts. The user can simply
         * trigger a remote master by asserting the corresponding interrupt line. This can be
         * achieved with the help of the following APIs:
         *
         * \sa snc_set_snc2sys_int()
         * \sa snc_set_sys2snc_int()
         *
         * Keep in mind that the CMAC master is not accessibly by the developer.
         *
         * The SDK exhibits a simple mailbox service which can be used to trigger the SNC and/or
         * SYSCPU master. For each master a 32-bit value is assigned and each bit index is
         * logically assigned by the developer a task/operation. For each bit position a callback
         * function can be registered and should be executed once the corresponding bit index is
         * asserted. Therefore, up to 32 different callback functions can be registered for each
         * master. When the RPMSG-Lite framework is employed that number is decreased by one as one
         * bit index is occupied by the framework. The bit indexes should be reflected in the
         * 'MAILBOX_INT_MAIN' and 'MAILBOX_INT_SNC' enumeration structures. In this example, we occupy
         * a single bit index for the SYSCPU and register a callback function associated to this bit index.
         * The mailbox service supports a simple prioritization scheme in case multiple bit indexes
         * are asserted at the same time. In this scheme the lower the bit index the lower the
         * execution priority of the associated callback function.
         *
         * Triggering a remote master is a two-step process:
         *
         * 1. Assert one or more bit index(s) of the remote master that should be triggered with the
         *    help of the mailbox_set_int() API.
         *
         * 2. Trigger the remote master by asserting the corresponding interrupt line.
         *    This will result in waking up the remote master, executing the corresponding
         *    interrupt handler which in turn will trigger the mailbox handler responsible
         *    for inspecting the 32-bit value and executing the associated callback
         *    function based on the mentioned prioritization scheme.
         *
         *    \sa snc_set_snc2sys_int()
         *    \sa snc_set_sys2snc_int()
         */

        mailbox_register_snc2sys_int(app_snc2sys_cb, MAILBOX_INT_MAIN_APP);
#else
        /* Directly register a callback function to be called when the SNC2SYS handler is executed. */
        snc_register_snc2sys_int(app_snc2sys_cb);
#endif /* dg_configUSE_MAILBOX */
#endif /* dg_configUSE_RPMSG_LITE */

        for ( ;; ) {

#if dg_configUSE_RPMSG_LITE
                uint32_t ept_src, rx_len;
                int32_t status;
                char *rx_buf;
                char temp_str[10];

                /*
                 * We simply print the received data on the serial console; no need to copy them
                 * in separate application-defined buffer.
                 */
                status = rpmsg_queue_recv_nocopy(master_rpmsg_instance_ptr, master_rpmsg_queue,
                                                                      &ept_src, &rx_buf, &rx_len, RL_BLOCK);

                ASSERT_WARNING(status == RL_SUCCESS);

                DBG_LOG("Successfully retrieved [%lu] bytes from the shared space. EP source address is: [%lu]\r\n", rx_len, ept_src);
                for (int i = 0; i < rx_len; i += 2) {
                        int16_t temp = *((int16_t *)rx_buf + i);

                        /* Convert float value to string */
                        app_float_2_string(temp_str, TMP102_RAW_TO_FLOAT(temp));
                        DBG_LOG("Temperature: %s\n\r", temp_str);
                }

                /* Once we have done with the no-copy buffer, the application should free the buffer. */
                rpmsg_queue_nocopy_free(master_rpmsg_instance_ptr, rx_buf);
#else
                OS_TASK_NOTIFY_WAIT(0x0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);

                /* Check whether a notification has been sent by SNC. */
                if (notif & TASK_SNC_NOTIF) {

                        /* Check if some data are available in the shared buffer */
                        while (app_shared_space_data_get_alloc_chunks()) {
                                uint8_t *buf = NULL;
                                size_t data_read;
                                uint32_t timestamp;
                                char temp_str[10];

                                /* Get the number of bytes available in the chunk (currently pointed)  */
                                size_t chunk_byte_size = app_shared_space_data_get_cur_chunk_bytes();

                                /*
                                 * If there are data available to be consumed, allocate some user space
                                 * in which data will be copied to.
                                 */
                                if (chunk_byte_size) {
                                        buf = OS_MALLOC(chunk_byte_size);
                                }
                                if (!buf) {
                                        continue;
                                }

                                /* Eventually pop/consume the data available in the current chunk. */
                                if (app_shared_space_data_queue_pop(buf, &data_read, &timestamp)) {
                                        DBG_LOG("\n\rSuccessfully retrieved [%d] bytes from the shared space.\r\n", data_read);
                                        DBG_LOG("Timestamp: 0x%lX\n\r", timestamp);

                                        for (int i = 0; i < data_read; i += 2) {
                                                int16_t temp = *((int16_t *)buf + i);

                                                /* Convert float value to string */
                                                app_float_2_string(temp_str, TMP102_RAW_TO_FLOAT(temp));
                                                DBG_LOG("Temperature: %s\n\r", temp_str);
                                        }
                                }
                                /*
                                 * We have processed the data retrieved; time to release
                                 * the previously allocated user space.
                                 */
                                OS_FREE(buf);
                        }

                }
#endif /* dg_configUSE_RPMSG_LITE */
        }

        /*
         * A DG Coroutine should end with this macro structure.
         * No effect if invoked in FreeRTOS context.
         */
        OS_TASK_END();
}
