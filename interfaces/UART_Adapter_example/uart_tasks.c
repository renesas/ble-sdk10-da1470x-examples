
 /****************************************************************************************
 *
 * @file uart_tasks.c
 *
 * @brief UART adapter demonstration example
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

#include "osal.h"
#include "ad_uart.h"
#include "sys_watchdog.h"
#include "platform_devices.h"

#define UART2_NOTIF_BYTE_SENT               ( 1 << 0 )
#define UART2_NOTIF_BYTE_NOT_SENT           ( 1 << 1 )
#define UART2_NOTIF_BYTE_RECEIVED           ( 1 << 2 )
#define UART2_NOTIF_BYTE_NOT_RECEIVED       ( 1 << 3 )

__RETAINED OS_QUEUE uart2_Q;    /* Q used to pass the data between the two UART2 tasks */

__RETAINED ad_uart_handle_t uart2_h; /* The Uart2 Handler is global because it is used by more than one tasks */

/**
 * @brief UART 1 echo task without UART flow control.
 *        The task reads a character on RX and sends it back on TX
 */
OS_TASK_FUNCTION(prv_Uart1_echo_Task, pvParameters)
{
        OS_TASK_BEGIN();

        char c=0;
        uint32_t bytes;
        ad_uart_handle_t uart1_h;

#if dg_configUSE_WDOG
        int8_t wakeup_task_wdog_id = -1;
        wakeup_task_wdog_id = sys_watchdog_register(false);
        ASSERT_WARNING(wakeup_task_wdog_id != -1);
#endif

        uart1_h = ad_uart_open(&uart1_uart_conf);                               /* Open the UART with the desired configuration    */
        ASSERT_ERROR(uart1_h != NULL);                                          /* Check if the UART1 opened OK */

        do {
                /* Suspend watchdog while blocking on getting 1 character */
                sys_watchdog_suspend(wakeup_task_wdog_id);

                bytes = ad_uart_read(uart1_h, &c, 1, OS_EVENT_FOREVER);         /* Wait for one char synchronously                 */

                /* Trigger the watchdog notification */
                sys_watchdog_notify_and_resume(wakeup_task_wdog_id);

                if (bytes > 0) {                                                /* if there is a successful read...                */
                        ad_uart_write(uart1_h, &c, bytes);                      /*       then write back the char to UART (echo)   */
                }
        } while( c != 27 );                                                     /* Exit the task if received ESC character (ASCII=27) */

        while (ad_uart_close(uart1_h, false) == AD_UART_ERROR_CONTROLLER_BUSY); /* Wait until the UART has finished all the transactions
                                                                                 * before exiting. */
#if dg_configUSE_WDOG
        sys_watchdog_unregister(wakeup_task_wdog_id);                           /* Unregister from watchdog before deleting the task */
        wakeup_task_wdog_id = -1;
#endif

        OS_TASK_DELETE( OS_GET_CURRENT_TASK() );                                /* Delete the task before exiting. It is not allowed in
                                                                                 * FreeRTOS a task to exit without being deleted from
                                                                                 * the OS's queues */

        OS_TASK_END();
}

/**
 * @brief UART 2 async TX callback.
 *        The function is called when the UART TX completes
 *        Then from the callback we notify the Task that TX completed
 *        IMPORTANT NOTE: The callback function runs in ISR context
 */
void uart2_write_arync_cb(void *user_data, uint16_t transferred)
{

        OS_TASK task_h = (OS_TASK)user_data;

        OS_TASK_NOTIFY_FROM_ISR(task_h, UART2_NOTIF_BYTE_SENT, OS_NOTIFY_SET_BITS);
}

/**
 * @brief UART 2 TX task.
 *        Receives bytes from uart2_Q and send them to UART2 pins
 *        RTS/CTS flow control is used
 */
OS_TASK_FUNCTION(prv_Uart2_async_TX_Task, pvParameters)
{
        OS_TASK_BEGIN();

        char c=0;
        OS_BASE_TYPE ret;
        uint32_t notif;

#if dg_configUSE_WDOG
        int8_t wakeup_task_wdog_id = -1;
        wakeup_task_wdog_id = sys_watchdog_register(false);
        ASSERT_WARNING(wakeup_task_wdog_id != -1);
#endif

        if (uart2_h == NULL) {
                uart2_h = ad_uart_open(&uart2_uart_conf);               /* Open the UART2 only if is not opened by the other task. */
        }

        ASSERT_ERROR(uart2_h != NULL);                                  /* Check if the UART2 opened with success */

        do {
                /* Suspend watchdog while blocking on reading the queue */
                sys_watchdog_suspend(wakeup_task_wdog_id);

                ret = OS_QUEUE_GET(uart2_Q , &c, OS_QUEUE_FOREVER);     /* Get a character from the Q. Task will be suspended if Q is empty */

                /* Trigger the watchdog notification */
                sys_watchdog_notify_and_resume(wakeup_task_wdog_id);

                OS_ASSERT(ret == OS_OK);                                /* Check that the Q operation was OK */

                ret = ad_uart_write_async(uart2_h, &c, 1, uart2_write_arync_cb, OS_GET_CURRENT_TASK());
                                                                        /* Wait for one char asynchronously TX */
                if (ret == AD_UART_ERROR_NONE) {                        /* if the async write successfully issued */

                        /* Suspend watchdog while blocking on waiting for the callback to occur */
                        sys_watchdog_suspend(wakeup_task_wdog_id);

                        ret = OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);
                                                                        /* wait to be notified from the callback */

                        /* Trigger the watchdog notification */
                        sys_watchdog_notify_and_resume(wakeup_task_wdog_id);
                }

        } while ( c != 27 );                                              /* Exit the task if received ESC character (ASCII=27) */

        while (ad_uart_close(uart2_h, false) == AD_UART_ERROR_CONTROLLER_BUSY); /* Wait until the UART has finished all the transactions
                                                                                 * before exiting. */

        OS_QUEUE_DELETE(uart2_Q);                                       /* Q is not needed anymore, so delete it */

#if dg_configUSE_WDOG
        sys_watchdog_unregister(wakeup_task_wdog_id);                           /* Unregister from watchdog before deleting the task */
        wakeup_task_wdog_id = -1;
#endif

        OS_TASK_DELETE( OS_GET_CURRENT_TASK() );                        /* Delete the task before exiting. It is not allowed in
                                                                         * FreeRTOS a task to exit without being deleted from
                                                                         * the OS's queues */
        OS_TASK_END();
}

/**
 * @brief UART 2 async RX callback.
 *        The function is called when the UART RX completes
 *        Then from the callback we notify the Task depending on the
 *        UART RX result (read data or not)
 *        IMPORTANT NOTE: The callback function runs in ISR context
 */
void uart2_read_arync_cb(void *user_data, uint16_t transferred)
{
        OS_TASK task_h = (OS_TASK)user_data;

        if (transferred>0) {
                OS_TASK_NOTIFY_FROM_ISR(task_h, UART2_NOTIF_BYTE_RECEIVED, OS_NOTIFY_SET_BITS);
        } else {
                OS_TASK_NOTIFY_FROM_ISR(task_h, UART2_NOTIF_BYTE_NOT_RECEIVED, OS_NOTIFY_SET_BITS);
        }
}

/**
 * @brief UART 2 RX task.
 *        Receives bytes from UART pins and put them in uart2_Q
 *        RTS/CTS flow control is used
 */
OS_TASK_FUNCTION(prv_Uart2_async_RX_Task, pvParameters)
{
        OS_TASK_BEGIN();

#if (dg_configUART_ADAPTER == 1)
        char c=0;
        OS_BASE_TYPE ret __UNUSED;
        uint32_t notif;

#if dg_configUSE_WDOG
        int8_t wakeup_task_wdog_id = -1;
        wakeup_task_wdog_id = sys_watchdog_register(false);
        ASSERT_WARNING(wakeup_task_wdog_id != -1);
#endif

        if (uart2_h == NULL) {
                uart2_h = ad_uart_open(&uart2_uart_conf);               /* Open the UART2 only if is not opened by the other task. */
        }

        ASSERT_ERROR(uart2_h != NULL);                                  /* Check if the UART2 opened with success */

        do {

                ret = ad_uart_read_async(uart2_h, &c, 1, uart2_read_arync_cb, OS_GET_CURRENT_TASK());

                /* Suspend watchdog while blocking on reading the queue */
                sys_watchdog_suspend(wakeup_task_wdog_id);
                                                                        /* Wait for one char asynchronously RX*/
                if (ret == AD_UART_ERROR_NONE) {                        /* if the async read successfully issued */
                        ret = OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);

                        /* Trigger the watchdog notification */
                        sys_watchdog_notify_and_resume(wakeup_task_wdog_id);

                                                                        /*       wait to be notified from the callback */
                        OS_ASSERT(ret == OS_OK);                        /*       Check that the task resumed OK */
                        if (notif & UART2_NOTIF_BYTE_RECEIVED) {
                                /* Suspend watchdog while blocking on placing data in the queue */
                                sys_watchdog_suspend(wakeup_task_wdog_id);

                                OS_QUEUE_PUT(uart2_Q, &c, OS_QUEUE_FOREVER);    /*       then write back the char to UART (echo) */

                                /* Trigger the watchdog notification */
                                sys_watchdog_notify_and_resume(wakeup_task_wdog_id);
                        }
                }

        } while( c != 27 );                                             /* Exit the task if received ESC character (ASCII=27) */

#endif

#if dg_configUSE_WDOG
        sys_watchdog_unregister(wakeup_task_wdog_id);                   /* Unregister from watchdog before deleting the task */
        wakeup_task_wdog_id = -1;
#endif

        OS_TASK_DELETE( OS_GET_CURRENT_TASK() );                        /* Delete the task before exiting. It is not allowed in
                                                                         * FreeRTOS a task to exit without being deleted from
                                                                         * the OS's queues */
        OS_TASK_END();
}


/**
 * @brief UART 3 echo task.
 *        The task exits when ESC character (ASCII = 27) is received.
 *        The task is using synchronous read/write to UART
 */
OS_TASK_FUNCTION(prv_Uart3_rts_cts_flow_ctrl_echo_Task, pvParameters)
{
        OS_TASK_BEGIN();

        char c=0;
        uint32_t bytes;
        ad_uart_handle_t uart3_h;

#if dg_configUSE_WDOG
        int8_t wakeup_task_wdog_id = -1;
        wakeup_task_wdog_id = sys_watchdog_register(false);
        ASSERT_WARNING(wakeup_task_wdog_id != -1);
#endif

        uart3_h = ad_uart_open(&uart3_uart_conf);                               /* Open the UART with the desired configuration    */
        ASSERT_ERROR(uart3_h != NULL);                                          /* Check if the UART1 opened OK */

        do {

                /* Suspend watchdog while blocking on reading the queue */
                sys_watchdog_suspend(wakeup_task_wdog_id);

                bytes = ad_uart_read(uart3_h, &c, 1, OS_EVENT_FOREVER);         /* Wait for one char synchronously                 */

                /* Trigger the watchdog notification */
                sys_watchdog_notify_and_resume(wakeup_task_wdog_id);

                if (bytes > 0) {                                                /* if there is a successful read...                */
                        ad_uart_write(uart3_h, &c, bytes);                      /*       then write back the char to UART (echo)   */
                }

        } while( c != 27 );                                                     /* Exit the task if received ESC character (ASCII=27) */

        while (ad_uart_close(uart3_h, false) == AD_UART_ERROR_CONTROLLER_BUSY); /* Wait until the UART has finished all the transactions
                                                                                 * before exiting. */

#if dg_configUSE_WDOG
        sys_watchdog_unregister(wakeup_task_wdog_id);                   /* Unregister from watchdog before deleting the task */
        wakeup_task_wdog_id = -1;
#endif

        OS_TASK_DELETE( OS_GET_CURRENT_TASK() );                        /* Delete the task before exiting. It is not allowed in
                                                                         * FreeRTOS a task to exit without being deleted from
                                                                         * the OS's queues */
        OS_TASK_END();
}
