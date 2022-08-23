

/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief I2C adapter demonstration example in SYSCPU context
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
#include "sys_watchdog.h"
#include "ad_i2c.h"
#include "platform_devices.h"
#include "hw_wkup.h"
#include "hw_pdc.h"
#include "peripheral_setup.h"
#include "app_common.h"

/* Task priorities. */
#define I2C_MASTER_TASK_PRIORITY           ( OS_TASK_PRIORITY_NORMAL )

#if dg_configUSE_WDOG
__RETAINED_RW static int8_t idle_task_wdog_id = -1;
#endif

/*
 * Perform any application specific hardware configuration.  The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware(void);

extern void thermo3_alert_signal(void);

/*
 * Task functions.
 */
OS_TASK_FUNCTION(termo3_task, pvParameters);

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

        /* THERMO3 click board task */
        status = OS_TASK_CREATE("THERMO3 TASK",        /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                        termo3_task,                    /* The function that implements the task. */
                        0,                              /* The parameter passed to the task. */
                        1024,                           /* The number of bytes to allocate to the
                                                           stack of the task. */
                        I2C_MASTER_TASK_PRIORITY,       /* The priority assigned to the task. */
                        task_h );                       /* The task handle */
        OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);

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

/* Callback function to be called when the alert signal of the sensor is asserted. */
static void app_thermo3_alert_cb(void)
{
        uint32_t status = hw_wkup_get_gpio_status(THERMO3_ALERT_PORT);

        /* Check if the alert signal triggered the WKUP controller */
        if (status & (0x1 << THERMO3_ALERT_PIN)) {
                thermo3_alert_signal();
        }

        /*
         * It's imperative that the interrupt latch status be cleared so, the system
         * can be re-triggered in the next event.
         */
        hw_wkup_clear_gpio_status(THERMO3_ALERT_PORT, status);
}

/*
 * Helper function to register callback function associated to the GPIO sup-block of the WKUP controller.
 * The SYSPCU core supports three separate IRQ lines for all three GPIO ports, that is GPIO_P0_IRQ,
 * GPIO_P1_IRQ and GPIO_P2_IRQ.
 */
static void app_wkup_gpio_register_cb(HW_GPIO_PORT port, HW_GPIO_PIN pin, hw_wkup_interrupt_cb cb)
{
        hw_wkup_unregister_interrupts();

        if (port == HW_GPIO_PORT_0) {
                hw_wkup_register_gpio_p0_interrupt(cb, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
        } else if (port == HW_GPIO_PORT_1) {
                hw_wkup_register_gpio_p1_interrupt(cb, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
        } else if (port == HW_GPIO_PORT_2) {
                hw_wkup_register_gpio_p2_interrupt(cb, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
        } else {
                return;
        }
}

/**
 * @brief Hardware Initialization
 */
static void prvSetupHardware(void)
{
        /* Init hardware */
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

        ASSERT_WARNING((THERMO3_ALERT_PORT < HW_GPIO_PORT_MAX) && (THERMO3_ALERT_PIN < HW_GPIO_PIN_MAX));

        /* WKUP controller initialization */
        hw_wkup_init(NULL);
        hw_wkup_set_trigger(THERMO3_ALERT_PORT, THERMO3_ALERT_PIN,
                (THERMO3_ALEERT_POL == TMP102_ALERT_POLARITY_ACTIVE_HIGH ? HW_WKUP_TRIG_LEVEL_HI : HW_WKUP_TRIG_LEVEL_LO));
        /* Make sure the I/O pin in which the alert signal is connect to is cleared */
        hw_wkup_clear_gpio_status(THERMO3_ALERT_PORT, 0x1 << THERMO3_ALERT_PIN);
        app_wkup_gpio_register_cb(THERMO3_ALERT_PORT, THERMO3_ALERT_PIN, app_thermo3_alert_cb);

        /*
         * When the SYSCPU master enters the default sleep state (i.e. extended sleep) its power
         * domain (i.e. PD_SYS) is turned off. This means that the core cannot react to external
         * events and thus, executing the corresponding ISR handler. This task is assigned to
         * the PDC engine which is responsible for capturing external events and notify a master.
         * To do so, the PDC comprises 16 entries that define what the system should do upon
         * a supported wakeup event. Here, we register a PDC entry that instructs the PDC to
         * enable the SYSCPU master following any toggle event in [THERMO3_ALERT_PORT].
         *
         * \note Alternatively, we can register PDC entries associated to a specific I/O line.
         *       However, that approach is not viable in cases where multiple I/O lines should
         *       be registered.
         *
         * \code{.c}
         *
         * hw_pdc_add_entry(HW_PDC_LUT_ENTRY_VAL(THERMO3_ALERT_PORT, THERMO3_ALERT_PIN,
         *                                                              HW_PDC_MASTER_CM33, 0));
         *
         * \endcode
         */
        uint32_t pdc_idx = hw_pdc_add_entry(HW_PDC_LUT_ENTRY_VAL(HW_PDC_TRIG_SELECT_PERIPHERAL,
                                                        (THERMO3_ALERT_PORT == HW_GPIO_PORT_0 ? HW_PDC_PERIPH_TRIG_ID_GPIO_P0 :
                                                        (THERMO3_ALERT_PORT == HW_GPIO_PORT_1 ? HW_PDC_PERIPH_TRIG_ID_GPIO_P1 :
                                                                HW_PDC_PERIPH_TRIG_ID_GPIO_P2)),
                                                        HW_PDC_MASTER_CM33,
                                                        (dg_configENABLE_XTAL32M_ON_WAKEUP ? HW_PDC_LUT_ENTRY_EN_XTAL : 0)));
        ASSERT_WARNING(pdc_idx != HW_PDC_INVALID_LUT_INDEX);

        hw_pdc_set_pending(pdc_idx);
        hw_pdc_acknowledge(pdc_idx);

        /* Configure the I/O pad in which the alert signal is connected to. */
        hw_gpio_configure_pin(THERMO3_ALERT_PORT, THERMO3_ALERT_PIN, HW_GPIO_MODE_INPUT_PULLDOWN,
                                                                                HW_GPIO_FUNC_GPIO, false);
        hw_gpio_pad_latch_enable(THERMO3_ALERT_PORT, THERMO3_ALERT_PIN);
        /*
         * Lock the status of the pad so its mode and functionality is retained during the sleep state.
         * If this is not the case the pad will return to its default state upon entering the sleep
         * state (typically this is input mode).
         */
        hw_gpio_pad_latch_disable(THERMO3_ALERT_PORT, THERMO3_ALERT_PIN);
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
