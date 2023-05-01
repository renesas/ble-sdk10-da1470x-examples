
 /****************************************************************************************
 *
 * @file spi_tack.c
 *
 * @brief SPI demonstration example for SYSCPU; Data consumer task definition.
 *
 * Copyright (C) 2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#include "snc.h"
#include "osal.h"
#include "app_shared_space.h"
#include "mailbox.h"
#include "rpmsg_lite.h"
#include "rpmsg_queue.h"
#include "rpmsg_platform.h"

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
/*
 * In this example, both the info and data structures (shared objects) are allocated and initialized in
 * the SNC context. As a result, the SYSCPU should only acquire the addresses of the shared objects.
 */
__RETAINED static app_shared_data_t *shared_data_ptr;
/* Pointer that holds the shared buffer that holds the SPI data retrieved by the SNC context. */
__RETAINED static uint8_t *rx_data_ptr;

/* Callback function to be called within SNC2SYS or MAILBOX handler. */
static void app_snc2sys_cb(void) {
        if (task_h) {
                OS_TASK_NOTIFY_FROM_ISR(task_h, TASK_SNC_NOTIF, OS_NOTIFY_SET_BITS);
        }
}
#endif /* dg_configUSE_RPMSG_LITE */


/**
 * Task responsible for fetching and printing on the serial console data received
 * over the SPI bus in SNC context.
 */
OS_TASK_FUNCTION(spi_master_slave_task, pvParameters)
{
        /*
         * The start of a DG coroutine must be marked with this macro.
         * No effect, when in FreeRTOS context.
         */
        OS_TASK_BEGIN();

        DBG_LOG("\n\rM33 Task Started...\n\r");

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
         * Keep in mind that the CMAC master is not accessible by the developer.
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

        /*
         * At this point the shared space should be initialized and all addresses be published
         * to the SNC service.
         */
        shared_data_ptr = app_shared_space_data_get();

        ASSERT_WARNING(shared_data_ptr);
        ASSERT_WARNING(shared_data_ptr->rx_data_ptr);

        /*
         * All addresses returned by snc_get_shared_space_addr() are de-normalized meaning
         * that they reflect the address space of the master that requests an address.
         * If a shared structure contains pointers it is important to know whether they
         * point to the SNC or SYSCPU address space.
         * In this example, the SNC has allocated some space in its own address space and so
         * a remote master should translate that address to its own address space.
         */
        rx_data_ptr = snc_convert_snc2sys_addr((const void *)shared_data_ptr->rx_data_ptr);
#endif /* dg_configUSE_RPMSG_LITE */

        for ( ;; ) {

#if dg_configUSE_RPMSG_LITE
                uint32_t ept_src, rx_len;
                int32_t status;
                char *rx_data;

                /*
                 * We simply print the received data on the serial console; no need to copy them
                 * in separate application-defined buffer.
                 */
                status = rpmsg_queue_recv_nocopy(master_rpmsg_instance_ptr, master_rpmsg_queue,
                                                              &ept_src, &rx_data, &rx_len, RL_BLOCK);

                ASSERT_WARNING(status == RL_SUCCESS);

                DBG_LOG("Successfully retrieved [%lu] bytes from the shared space. EP source address is: [%lu]\r\n", rx_len, ept_src);
                for (int i = 0; i < rx_len; i++) {
                        DBG_LOG("%d ", *((uint8_t *)rx_data + i));
                        if (!(i % 10)) DBG_LOG("\n\r");
                }
                DBG_LOG("\n\r\n");

                /* Once we have done with the no-copy buffer, the application should free the buffer. */
                rpmsg_queue_nocopy_free(master_rpmsg_instance_ptr, rx_data);
#else
                OS_TASK_NOTIFY_WAIT(0x0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);

                /* Check whether a notification has been sent by SNC. */
                if (notif & TASK_SNC_NOTIF) {
                        /* Acquire exclusive access on shared data */
                        app_semph_take();

                        DBG_LOG("Successfully retrieved [%d] bytes from the shared space.\r\n", shared_data_ptr->rx_data_size);
                        for (int i = 0; i < shared_data_ptr->rx_data_size; i++) {
                                DBG_LOG("%d ", rx_data_ptr[i]);
                                if (!(i % 10)) DBG_LOG("\n\r");
                        }
                        DBG_LOG("\n\r\n");

                        /* Release exclusive access on shared data. */
                        app_semph_give();
                }
#endif /* dg_configUSE_RPMSG_LITE */
        }

        /*
         * The end of a DG coroutine must be marked with this macro.
         * No effect, when in FreeRTOS context.
         */
        OS_TASK_END();
}
