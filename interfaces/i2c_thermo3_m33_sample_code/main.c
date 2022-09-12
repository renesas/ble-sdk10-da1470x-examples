

/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief I2C adapter demonstration example for SYSCPU
 *
 * Copyright (C) 2022 Dialog Semiconductor.
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
#include "hw_sys.h"
#include "hw_watchdog.h"
#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"
#include "snc.h"
#include "sys_watchdog.h"
#include "app_shared_space.h"


/* Task priorities. */
#define PERIPHERAL_LOG_TASK_PRIORITY           ( OS_TASK_PRIORITY_NORMAL )

#if dg_configUSE_WDOG
__RETAINED_RW static int8_t idle_task_wdog_id = -1;
#endif

/*
 * Perform any application specific hardware configuration.  The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware(void);

/*
 * Task functions.
 */
OS_TASK_FUNCTION(thermo3_task, pvParameters);

/*
 * System initialization task.
 */
static OS_TASK_FUNCTION(system_init, pvParameters)
{
        OS_TASK_BEGIN();

        OS_BASE_TYPE status;
        OS_TASK task_h;

        cm_sys_clk_init(sysclk_XTAL32M);
        cm_apb_set_clock_divider(apb_div1);
        cm_ahb_set_clock_divider(ahb_div1);
        cm_lp_clk_init();

        sys_watchdog_init();

#if dg_configUSE_WDOG
        idle_task_wdog_id = sys_watchdog_register(false);
        ASSERT_WARNING(idle_task_wdog_id != -1);
        sys_watchdog_configure_idle_id(idle_task_wdog_id);
#endif

        /* Prepare the hardware to run this demo. */
        prvSetupHardware();

        /* Set the desired sleep mode. */
        pm_sleep_mode_set(pm_mode_extended_sleep);

        /* Set the desired wakeup mode. */
        pm_set_sys_wakeup_mode(pm_sys_wakeup_mode_normal);

        /* Create a task triggered by the SNC master when data are available in the shared memory pool. */
        status = OS_TASK_CREATE("thermo3_task",         /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                        thermo3_task,                   /* The function that implements the task. */
                        0,                              /* The parameter passed to the task. */
                        1024,                           /* The number of bytes to allocate to the
                                                           stack of the task. */
                        PERIPHERAL_LOG_TASK_PRIORITY,   /* The priority assigned to the task. */
                        task_h );                       /* The task handle */
        OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);

        /*  Initialize and start SNC */
        snc_freeze();
        snc_init();
        snc_start();

        /*
         * Wait for SNC to finish its start-up process. The ready flag here designates that
         * the SNC has started executing its firmware.
         */
        while (!snc_is_ready());

        /*  Wait for the SNC application to allocate and initialize the application-defined shared space. */
        while (!app_shared_space_ctrl_is_ready());
        while (!app_shared_space_data_is_ready());

        /* The work of the SysInit task is done. */
        OS_TASK_DELETE(OS_GET_CURRENT_TASK());

        OS_TASK_END();
}

/**
 * @brief Template main creates a SysInit task, which creates a Template task
 */
int main(void)
{
        OS_TASK xHandle;
        OS_BASE_TYPE status;

        /* Start SysInit task. */
        status = OS_TASK_CREATE("SysInit",              /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                        system_init,                    /* The System Initialization task. */
                        0,                              /* The parameter passed to the task. */
                        OS_MINIMAL_TASK_STACK_SIZE,     /* The number of bytes to allocate to the
                                                           stack of the task. */
                        OS_TASK_PRIORITY_HIGHEST,       /* The priority assigned to the task. */
                        xHandle );                      /* The task handle */
        OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);

        /* Start the tasks and timer running. */
        OS_TASK_SCHEDULER_RUN();

        /* If all is well, the scheduler will now be running, and the following
        line will never be reached. */
        for ( ;; );
}

/**
 * @brief Initialize the peripherals domain after power-up.
 *
 */
static void periph_init(void)
{
}

/**
 * @brief Hardware Initialization
 */
static void prvSetupHardware(void)
{
        /* Init hardware */
        pm_system_init(periph_init);
}

/**
 * @brief Malloc fail hook
 */
OS_APP_MALLOC_FAILED(void)
{
        /* This function will only get called if a call to OS_MALLOC() fails. */
        ASSERT_ERROR(0);
}

/**
 * @brief Application idle task hook
 */
OS_APP_IDLE(void)
{
        /* This function will be called on each iteration of the idle task. */
#if dg_configUSE_WDOG
        sys_watchdog_notify(idle_task_wdog_id);
#endif
}

/**
 * @brief Application stack overflow hook
 */
OS_APP_STACK_OVERFLOW(OS_TASK pxTask, char *pcTaskName)
{
        (void) pcTaskName;
        (void) pxTask;

        /* This function is called if a stack overflow is detected. */
        ASSERT_ERROR(0);
}

/**
 * @brief Application tick hook
 */
OS_APP_TICK(void)
{
}
