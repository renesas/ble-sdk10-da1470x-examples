/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief BLE peripheral application
 *
 * Copyright (C) 2015-2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#include <string.h>
#include <stdbool.h>
#include "osal.h"
#include "resmgmt.h"
#include "ad_ble.h"
#include "ad_nvms.h"
#include "ble_mgr.h"
#include "hw_gpio.h"
#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"
#include "sys_watchdog.h"

/* Task priorities */
#define mainBLE_PERIPHERAL_TASK_PRIORITY              ( OS_TASK_PRIORITY_NORMAL )

#if dg_configUSE_WDOG
__RETAINED_RW int8_t idle_task_wdog_id = -1;
#endif

/*
 * Perform any application specific hardware configuration.  The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware( void );
/*
 * Task functions .
 */
OS_TASK_FUNCTION(ble_peripheral_task, params);

static OS_TASK handle = NULL;

/**
 * @brief System Initialization and creation of the BLE task
 */
static OS_TASK_FUNCTION(system_init, pvParameters)
{
#if defined CONFIG_RETARGET
        extern void retarget_init(void);
#endif

        /* Prepare clocks. Note: cm_cpu_clk_set() and cm_sys_clk_set() can be called only from a
         * task since they will suspend the task until the XTAL16M has settled and, maybe, the PLL
         * is locked.
         */
#if (DEVICE_FAMILY == DA1468X)
        cm_sys_clk_init(sysclk_XTAL16M);
#elif (DEVICE_FAMILY == DA1469X || DEVICE_FAMILY == DA1470X)
        cm_sys_clk_init(sysclk_XTAL32M);
#endif
        cm_apb_set_clock_divider(apb_div1);
        cm_ahb_set_clock_divider(ahb_div1);
        cm_lp_clk_init();

        /*
         * Initialize platform watchdog
         */
        sys_watchdog_init();

#if dg_configUSE_WDOG
        // Register the Idle task first.
        idle_task_wdog_id = sys_watchdog_register(false);
        ASSERT_WARNING(idle_task_wdog_id != -1);
        sys_watchdog_configure_idle_id(idle_task_wdog_id);
#endif

        /* Set system clock */
#if (DEVICE_FAMILY == DA1468X)
        cm_sys_clk_set(sysclk_XTAL16M);
#endif

        /* Prepare the hardware to run this demo. */
        prvSetupHardware();

        /* Set the desired sleep mode. */
        pm_set_wakeup_mode(true);
        pm_sleep_mode_set(pm_mode_extended_sleep);

#if defined CONFIG_RETARGET
        retarget_init();
#endif

        /* Initialize BLE Manager */
        ble_mgr_init();

        /* Start the BLE Peripheral application task. */
        OS_TASK_CREATE("BLE Peripheral",                /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                       ble_peripheral_task,             /* The function that implements the task. */
                       NULL,                            /* The parameter passed to the task. */
#if defined CONFIG_RETARGET
                       1024,                            /* The number of bytes to allocate to the
                                                           stack of the task. */
#else
                       200 * OS_STACK_WORD_SIZE,        /* The number of bytes to allocate to the
                                                           stack of the task. */
#endif
                       mainBLE_PERIPHERAL_TASK_PRIORITY,/* The priority assigned to the task. */
                       handle);                         /* The task handle. */
        OS_ASSERT(handle);

        /* the work of the SysInit task is done */
        OS_TASK_DELETE(OS_GET_CURRENT_TASK());
}
/*-----------------------------------------------------------*/

/**
 * @brief Basic initialization and creation of the system initialization task.
 */
int main( void )
{
        OS_BASE_TYPE status;

#if (DEVICE_FAMILY == DA1468X)
        cm_clk_init_low_level();                          /* Basic clock initializations. */
#endif

        /* Start SysInit task. */
        status = OS_TASK_CREATE("SysInit",                /* The text name assigned to the task, for
                                                             debug only; not used by the kernel. */
                                system_init,              /* The System Initialization task. */
                                ( void * ) 0,             /* The parameter passed to the task. */
                                1200,                     /* The number of bytes to allocate to the
                                                             stack of the task. */
                                OS_TASK_PRIORITY_HIGHEST, /* The priority assigned to the task. */
                                handle );                 /* The task handle */
        OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);

        /* Start the tasks and timer running. */
        OS_TASK_SCHEDULER_RUN();

        /* If all is well, the scheduler will now be running, and the following
        line will never be reached.  If the following line does execute, then
        there was insufficient FreeRTOS heap memory available for the idle and/or
        timer tasks     to be created.  See the memory management section on the
        FreeRTOS web site for more details. */
        for ( ;; );
}

/**
 * \brief Initialize the peripherals domain after power-up.
 *
 */
#if dg_configAUTOTEST_ENABLE
static void periph_init(void)
{

#if (DEVICE_FAMILY == DA1468X)
#if defined CONFIG_RETARGET
#if DEVICE_FPGA
        set_FPGA_SW3_21(false, false);
#endif
        HW_GPIO_SET_PIN_FUNCTION(SER1_TX);
        HW_GPIO_SET_PIN_FUNCTION(SER1_RX);
#endif
#endif

}
#endif /* dg_configAUTOTEST_ENABLE */

static void prvSetupHardware( void )
{

        /* Init hardware */
#if dg_configAUTOTEST_ENABLE
        pm_system_init(periph_init);
#else
        pm_system_init(NULL);
#endif

}

/**
 * @brief Malloc fail hook
 *
 * This function will be called only if it is enabled in the configuration of the OS
 * or in the OS abstraction layer header osal.h, by a relevant macro definition.
 * It is a hook function that will execute when a call to OS_MALLOC() returns error.
 * OS_MALLOC() is called internally by the kernel whenever a task, queue,
 * timer or semaphore is created. It can be also called by the application.
 * The size of the available heap is defined by OS_TOTAL_HEAP_SIZE in osal.h.
 * The OS_GET_FREE_HEAP_SIZE() API function can be used to query the size of
 * free heap space that remains, although it does not provide information on
 * whether the remaining heap is fragmented.
 */
OS_APP_MALLOC_FAILED( void )
{
        ASSERT_ERROR(0);
}

/**
 * @brief Application idle task hook
 *
 * This function will be called only if it is enabled in the configuration of the OS
 * or in the OS abstraction layer header osal.h, by a relevant macro definition.
 * It will be called on each iteration of the idle task.
 * It is essential that code added to this hook function never attempts
 * to block in any way (for example, call OS_QUEUE_GET() with a block time
 * specified, or call OS_TASK_DELAY()). If the application makes use of the
 * OS_TASK_DELETE() API function (as this demo application does) then it is also
 * important that OS_APP_IDLE() is permitted to return to its calling
 * function, because it is the responsibility of the idle task to clean up
 * memory allocated by the kernel to any task that has since been deleted.
 */
OS_APP_IDLE( void )
{

#if dg_configUSE_WDOG
        sys_watchdog_notify(idle_task_wdog_id);
#endif
}

/**
 * @brief Application stack overflow hook
 *
 * Run-time stack overflow checking is performed only if it is enabled in the configuration of the OS
 * or in the OS abstraction layer header osal.h, by a relevant macro definition.
 * This hook function is called if a stack overflow is detected.
 */
OS_APP_STACK_OVERFLOW( OS_TASK pxTask, char *pcTaskName )
{
        ( void ) pcTaskName;
        ( void ) pxTask;

        ASSERT_ERROR(0);
}

/**
 * @brief Application tick hook
 *
 * This function will be called only if it is enabled in the configuration of the OS
 * or in the OS abstraction layer header osal.h, by a relevant macro definition.
 * This hook function is executed each time a tick interrupt occurs.
 */
OS_APP_TICK( void )
{
}
