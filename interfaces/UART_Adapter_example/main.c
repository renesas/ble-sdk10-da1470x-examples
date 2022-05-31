/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief UART Adapters example with all three UARTs
 *
 * Copyright (C) 2015-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "osal.h"
#include "resmgmt.h"
#include "hw_cpm.h"
#include "hw_gpio.h"
#include "sys_watchdog.h"
#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"

#if dg_configUSE_WDOG
__RETAINED_RW int8_t idle_task_wdog_id = -1;
#endif

/*
 * Tasks functions declarations
 */
OS_TASK_FUNCTION(prv_Uart1_echo_Task, pvParameters);
OS_TASK_FUNCTION(prv_Uart2_async_RX_Task, pvParameters);
OS_TASK_FUNCTION(prv_Uart2_async_TX_Task, pvParameters);
OS_TASK_FUNCTION(prv_Uart3_rts_cts_flow_ctrl_echo_Task, pvParameters);

extern OS_QUEUE uart2_Q;

/*
 * Perform any application specific hardware configuration.  The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware( void );

/**
 * @brief system_init() initialize the system (e.g. selects the desired clocks), selects operation parameters (e.g. sleep mode)
 *               and creates the uart_test_task task which which actually uses all three UARTs.
 *               The system_init runs once and exits when finishes its work.
 */
static OS_TASK_FUNCTION(system_init, pvParameters)
{
        OS_TASK_BEGIN();

        OS_TASK uart_test_task_h = NULL;

#if defined CONFIG_RETARGET
        extern void retarget_init(void);
#endif

        cm_sys_clk_init(sysclk_XTAL32M);

        cm_apb_set_clock_divider(apb_div1);
        cm_ahb_set_clock_divider(ahb_div1);
        cm_lp_clk_init();

        /* Initialize platform watchdog */
        sys_watchdog_init();

#if dg_configUSE_WDOG
        /* Register the Idle task first */
        idle_task_wdog_id = sys_watchdog_register(false);
        ASSERT_WARNING(idle_task_wdog_id != -1);
        sys_watchdog_configure_idle_id(idle_task_wdog_id);
#endif

        /* Prepare the hardware to run this demo. */
        prvSetupHardware();

#if defined CONFIG_RETARGET
        retarget_init();
#endif

        /* true - wait for the XTAL32M to be ready before run the app code on wake-up */
        pm_set_wakeup_mode(true);

        /* Set the desired sleep mode. */
        pm_sleep_mode_set(pm_mode_extended_sleep);

        /* Set the desired wakeup mode. */
        pm_set_sys_wakeup_mode(pm_sys_wakeup_mode_fast);

        /* UART1 echo task without flow control */
        OS_TASK_CREATE( "U1 ECHO",                                      /* The text name assigned to the task, for
                                                                           debug only; not used by the kernel. */
                        prv_Uart1_echo_Task,                            /* The function that implements the task. */
                        NULL,                                           /* The parameter passed to the task. */
                        configMINIMAL_STACK_SIZE * OS_STACK_WORD_SIZE,  /* The number of bytes to allocate to the
                                                                           stack of the task. */
                        OS_TASK_PRIORITY_NORMAL,                        /* The priority assigned to the task. */
                        uart_test_task_h );                             /* The task handle */
        OS_ASSERT(uart_test_task_h);                                    /* Check that the task created OK */

        OS_QUEUE_CREATE(uart2_Q, sizeof(char), 100);                    /* Create the uart2_Q */
        OS_ASSERT(uart2_Q);                                             /* Check that Q created OK */


        /* UART2 RX task with RTS/CTS flow control*/
        OS_TASK_CREATE( "U2 RX RTS/CTS",                                /* The text name assigned to the task, for
                                                                           debug only; not used by the kernel. */
                        prv_Uart2_async_RX_Task,                        /* The function that implements the task. */
                        NULL,                                           /* The parameter passed to the task. */
                        configMINIMAL_STACK_SIZE * OS_STACK_WORD_SIZE,  /* The number of bytes to allocate to the
                                                                           stack of the task. */
                        OS_TASK_PRIORITY_NORMAL,                        /* The priority assigned to the task. */
                        uart_test_task_h );                             /* The task handle */
        OS_ASSERT(uart_test_task_h);                                    /* Check that the task created OK */

        /* UART2 TX task with RTS/CTS flow control */
        OS_TASK_CREATE( "U2 TX RTS/CTS",                                /* The text name assigned to the task, for
                                                                           debug only; not used by the kernel. */
                        prv_Uart2_async_TX_Task,                        /* The function that implements the task. */
                        NULL,                                           /* The parameter passed to the task. */
                        configMINIMAL_STACK_SIZE * OS_STACK_WORD_SIZE,  /* The number of bytes to allocate to the
                                                                           stack of the task. */
                        OS_TASK_PRIORITY_NORMAL,                        /* The priority assigned to the task. */
                        uart_test_task_h );                             /* The task handle */
        OS_ASSERT(uart_test_task_h);                                    /* Check that the task created OK */

        /* UART3 ECHO task with RTS/CTS flow control*/
        OS_TASK_CREATE( "U3 ECHO RTS/CTS",                              /* The text name assigned to the task, for
                                                                           debug only; not used by the kernel. */
                        prv_Uart3_rts_cts_flow_ctrl_echo_Task,          /* The function that implements the task. */
                        NULL,                                           /* The parameter passed to the task. */
                        configMINIMAL_STACK_SIZE * OS_STACK_WORD_SIZE,  /* The number of bytes to allocate to the
                                                                           stack of the task. */
                        OS_TASK_PRIORITY_NORMAL,                        /* The priority assigned to the task. */
                        uart_test_task_h );                             /* The task handle */
        OS_ASSERT(uart_test_task_h);                                    /* Check that the task created OK */


        /* the work of the SysInit task is done
         * The task will be terminated */
        OS_TASK_DELETE( OS_GET_CURRENT_TASK() );

        OS_TASK_END();
}

/**
 * @brief main() creates a 'system_init' task and starts the FreeRTOS scheduler.
 */
int main( void )
{
        OS_BASE_TYPE status;
        static OS_TASK xsystem_init_Handle;


        /* Start the two tasks as described in the comments at the top of this
        file. */
        status = OS_TASK_CREATE("SysInit",                              /* The text name assigned to the task, for
                                                                           debug only; not used by the kernel. */
                        system_init,                                    /* The System Initialization task. */
                        ( void * ) 0,                                   /* The parameter passed to the task. */
                        configMINIMAL_STACK_SIZE * OS_STACK_WORD_SIZE,  /* The number of bytes to allocate to the
                                                                           stack of the task. */
                        OS_TASK_PRIORITY_HIGHEST,                       /* The priority assigned to the task.
                                                                           we need this task to run first with
                                                                           no interruption, that is why choose
                                                                           HIGHEST priority*/
                        xsystem_init_Handle );                          /* The task handle */

        OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);                    /* Check that the task created OK */

        /* Start the tasks and timer running. */
        vTaskStartScheduler();

        /* If all is well, the scheduler will now be running, and the following
        line will never be reached.  If the following line does execute, then
        there was insufficient FreeRTOS heap memory available for the idle and/or
        timer tasks to be created.  See the memory management section on the
        FreeRTOS web site for more details. */
        for ( ;; );

}

/**
 * @brief Initialize the peripherals domain after power-up.
 *
 */
static void periph_init(void)
{
        /* since the UART adapter is used, all the GPIOs configuration for the UARTs
         * is done by the adapters. Also the latch/unlatch is handled by the adapter too.
         * The UART configuration is in the platform_devices.c and the necessary declarations
         * of the three UART configuration instances are in the platform_devices.h
         */
}

/**
 * @brief Hardware Initialization
 */
static void prvSetupHardware( void )
{
        /* Init hardware */
        pm_system_init(periph_init);
}

/**
 * @brief Malloc fail hook
 */
OS_APP_MALLOC_FAILED(void)
{
        /* vApplicationMallocFailedHook() will only be called if
        configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
        function that will get called if a call to OS_MALLOC() fails.
        OS_MALLOC() is called internally by the kernel whenever a task, queue,
        timer or semaphore is created.  It is also called by various parts of the
        demo application.  If heap_1.c or heap_2.c are used, then the size of the
        heap available to OS_MALLOC() is defined by configTOTAL_HEAP_SIZE in
        FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
        to query the size of free heap space that remains (although it does not
        provide information on how the remaining heap might be fragmented). */
        ASSERT_ERROR(0);
}

/**
 * @brief Application idle task hook
 */
OS_APP_IDLE(void)
{
        /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
           to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
           task. It is essential that code added to this hook function never attempts
           to block in any way (for example, call xQueueReceive() with a block time
           specified, or call vTaskDelay()).  If the application makes use of the
           vTaskDelete() API function (as this demo application does) then it is also
           important that vApplicationIdleHook() is permitted to return to its calling
           function, because it is the responsibility of the idle task to clean up
           memory allocated by the kernel to any task that has since been deleted. */

#if (dg_configTRACK_OS_HEAP == 1)
        OS_BASE_TYPE i = 0;
        OS_BASE_TYPE uxMinimumEverFreeHeapSize;

        // Generate raw status information about each task.
        UBaseType_t uxNbOfTaskEntries = uxTaskGetSystemState(pxTaskStatusArray,
                                                        mainMAX_NB_OF_TASKS, &ulTotalRunTime);

        for (i = 0; i < uxNbOfTaskEntries; i++) {
                /* Check Free Stack*/
                OS_BASE_TYPE uxStackHighWaterMark;

                uxStackHighWaterMark = uxTaskGetStackHighWaterMark(pxTaskStatusArray[i].xHandle);
                OS_ASSERT(uxStackHighWaterMark >= mainMIN_STACK_GUARD_SIZE);
        }

        /* Check Minimum Ever Free Heap against defined guard. */
        uxMinimumEverFreeHeapSize = xPortGetMinimumEverFreeHeapSize();
        OS_ASSERT(uxMinimumEverFreeHeapSize >= mainTOTAL_HEAP_SIZE_GUARD);
#endif /* (dg_configTRACK_OS_HEAP == 1) */

#if dg_configUSE_WDOG
        sys_watchdog_notify(idle_task_wdog_id);
#endif
}

/**
 * @brief Application stack overflow hook
 */
OS_APP_STACK_OVERFLOW(OS_TASK pxTask, char *pcTaskName)
{
        ( void ) pcTaskName;
        ( void ) pxTask;

        /* Run time stack overflow checking is performed if
        configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
        function is called if a stack overflow is detected. */
        ASSERT_ERROR(0);
}

/**
 * @brief Application tick hook
 */
OS_APP_TICK(void)
{
}


