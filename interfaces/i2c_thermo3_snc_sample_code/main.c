
/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief I2C adapter demonstration example in SNC context
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
#include "sys_watchdog.h"
#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"
#include "snc.h"
#include "app_shared_space.h"
#include "ad_i2c.h"
#include "platform_devices.h"

/* Task priorities. */
#define PERIPHERAL_TASK_PRIORITY    ( OS_TASK_PRIORITY_NORMAL )

/*
 * Perform any application specific hardware configuration. The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware(void);

/*
 * Task functions.
 */
OS_TASK_FUNCTION(termo3_task, pvParameters);

#if dg_configUSE_WDOG
__RETAINED_RW static int8_t idle_task_wdog_id = -1;
#endif

/*
 * System initialization task.
 */
static OS_TASK_FUNCTION(system_init, pvParameters)
{
        OS_TASK_BEGIN();

        OS_TASK task_h;
        OS_BASE_TYPE status;

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

        /* Create a task that reads data from an external sensor over the I2C bus */
        status = OS_TASK_CREATE("THERMO3 TASK",        /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                        termo3_task,                    /* The function that implements the task. */
                        NULL,                           /* The parameter passed to the task. */
                        0,
                                                        /* The number of bytes to allocate to the
                                                           stack of the task (do not care in co-operative scheduling). */
                        PERIPHERAL_TASK_PRIORITY,       /* The priority assigned to the task. */
                        task_h );                       /* The task handle */
        OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);

        /*
         * Most of times, a master core needs to exchange data with other masters available in
         * the system. The DA1470x family of devices integrates three masters, that is the
         * SYSCPU, SNC and CMAC. Data exchange is accomplished via a memory area that is accessible
         * by all masters. In specific, a memory area which is part of SYSRAM and 128kB in size is
         * provided for that purpose. The steps required to manually allocate and the publish some
         * common memory space is briefly described below. If the RPMsg-Lite IPC scheme is employed
         * there is no need for the user to manually define the shared space and trigger a remote
         * master. In this example data exchange is done between the SYSCPU (M33) and SNC (M0+) masters.
         *
         * 1. One or more masters should allocate some space in the common memory pool with the help
         *    of the '__SNC_SHARED' attribute. In this example, only the SNC allocates some space for
         *    data and info sharing. In specific, the following two objects are defined:
         *
         *     a. Control information (e.g. ready flags, application-defined semaphores etc.)
         *     b. Raw data received over the peripheral interface. This information can then
         *        be processed by the SYSCPU which can run up to 160MHz.
         *
         *    The 'SNC_SHARED_SPACE_APP_COUNT' macro should reflect the number of the different shared
         *    objects defined by the application; here two objects are defined.
         *    Typically an SNC application is composed of two separate projects; one running in SYSCPU
         *    and one executed in the SNC context. It is imperative that this macro is common for both
         *    projects. Any value mismatch will result in each master interpreting the common memory
         *    space differently and thus, raising assertions.
         *
         * 2. The next step requires that all shared objects are published to the SNC service (part of the SDK)
         *     with the help of the snc_set_shared_space_addr() API.In this example, the space allocation,
         *     initialization and publish is abstracted with the following application-defined routines:
         *
         *     \p app_shared_space_ctrl_init()
         *     \p app_shared_space_data_init()
         *
         * 3. Once the addresses of all shared objects are published, other masters can then access the
         *    the memory pool by invoking the snc_get_shared_space_addr() API. It is important, that a
         *    remote master knows when the shared space allocation and initialization is complete.
         *    Typically, this is accomplished by defining a flag which should be evaluated before accessing
         *    the shared space.
         *
         *    \code{.c}
         *
         *    // The SNC allocates and initializes the shared space
         *
         *    static OS_TASK_FUNCTION(system_init, pvParameters) {
         *
         *      app_shared_space_ctrl_init();
         *      app_shared_space_data_init();
         *
         *      // Other operations related to the shared space establishment
         *
         *      // Designate end of shared space allocation and initialization
         *      app_shared_space_ctrl_set_ready();
         *
         *    }
         *
         *    \endcode
         *
         *
         *    // The SYSCPU should wait for the shared space allocation/initialization before
         *    // accessing the memory pool.
         *
         *    \code{.c}
         *
         *    while (!app_shared_space_ctrl_is_ready());
         *    while (!app_shared_space_data_is_ready());
         *
         *    app_shared_data_t *shared_data_ptr = app_shared_space_data_get();
         *
         *    for(;;) {
         *
         *      // Access the shared memory space/objects
         *    }
         *
         *    \endcode
         */

        /* Allocate and initialize the shared space that accommodates control data */
        app_shared_space_ctrl_init();
        /* Allocate and initialize the shared space that accommodates raw data */
        app_shared_space_data_init();

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

        /*
         * In SNC context, the [snc_is_ready] flag is set thus, designating to remote masters that
         * the SNC is up and running.
         */
        snc_init();

#if SNC_HOLD
        volatile bool on_halt = true;
        while (on_halt);
#endif

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
        /* Init. hardware */
        pm_system_init(periph_init);

#if dg_configI2C_ADAPTER
        /*
         * It is recommended that all signals assigned to peripheral blocks and controlled
         * by the adapter layer are configured to the 'off' state. That is the state when
         * a device is not employed and its associated adapter is closed.
         */
        ad_i2c_io_config(((ad_i2c_controller_conf_t *)I2C_DEVICE_MASTER)->id,
                ((ad_i2c_controller_conf_t *)I2C_DEVICE_MASTER)->io, AD_IO_CONF_OFF);
#endif
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
