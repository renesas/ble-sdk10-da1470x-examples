/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief FreeRTOS template application with retarget
 *
 * Copyright (C) 2015-2021 Dialog Semiconductor.
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
#include "hw_watchdog.h"
#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"

#include "hw_pdc.h"
#include "hw_sys.h"
#include "hw_wkup.h"

/* Task priorities */
#define mainTEMPLATE_TASK_PRIORITY              ( OS_TASK_PRIORITY_NORMAL )

/* The rate at which data is template task counter is incremented. */
#define mainCOUNTER_FREQUENCY_MS                OS_MS_2_TICKS(200)

/******** Notification Bitmasks ********/
#define WKUP_KEY_PRESS_EVENT_NOTIF              (1 << 2)
#define WKUP_KEY_RELEASE_EVENT_NOTIF            (1 << 3)
#define WKUP_GPIO_P1_EVENT_NOTIF_LOW            (1 << 4)
#define WKUP_GPIO_P1_EVENT_NOTIF_HIGH           (1 << 5)

/*
 * Macro used for selecting the trigger (active) of the WKUP controller.
 * Valid values are:
 *
 * 0 --> The WKUP controller is triggered by the falling edge of a pulse
 *       (the pin is connected to a pull-up resistor)
 * 1 --> The WKUP controller is triggered by the rising edge  of a pulse
 *       (the pin is connected to a pull-down resistor)
 *
 **/
#define WKUP_TRIGGER_STATE                      (0)     // State for KEY interrupt

#define GPIO_TRIGGER_STATE                      (0)     // State for GPIO interrupt
/*
 * Macro used for selecting if the wake-up trigger is edge or level sensitive
 *
 * 0 -> The trigger is level sensitive
 * 1 -> The trigger is edge sensitive
 */
#define GPIO_TRIGGER_SENSITIVITY                (1)

#if (dg_configWKUP_KEY_BLOCK_ENABLE)
#define WKUP_TRIGGER_ENABLED                    (1)
#else
#define WKUP_TRIGGER_ENABLED                    (0)
#endif

#if (dg_configWKUP_GPIO_P1_BLOCK_ENABLE)
#define GPIO_TRIGGER_ENABLED                    (1)
#else
#define GPIO_TRIGGER_ENABLED                    (0)
#endif

/********************************* Custom wake up settings ************************************/
wkup_config pin_wkup_conf = {
        .debounce = 10,
        .pin_wkup_state[HW_GPIO_PORT_1] = ( WKUP_TRIGGER_ENABLED << KEY1_PIN ),
        .pin_gpio_state[HW_GPIO_PORT_1] = ( (WKUP_TRIGGER_ENABLED | GPIO_TRIGGER_ENABLED) << KEY1_PIN) | (GPIO_TRIGGER_ENABLED << KEY2_PIN ),
        .pin_trigger[HW_GPIO_PORT_1]    = ( WKUP_TRIGGER_STATE << KEY1_PIN) | (GPIO_TRIGGER_STATE << KEY2_PIN ),
        .gpio_sense[HW_GPIO_PORT_1]     = ( GPIO_TRIGGER_SENSITIVITY << KEY1_PIN) | (GPIO_TRIGGER_SENSITIVITY << KEY2_PIN ),
};
/********************************************************************************************/

/*
 * Perform any application specific hardware configuration.  The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware( void );

static OS_TASK xHandle;

/* Task handle */
__RETAINED static OS_TASK task_h;

#if (dg_configWKUP_KEY_BLOCK_ENABLE)
void wkup_deb_interrupt_cb(void)
{
        uint32_t event = 0;
        uint8_t trigger = 0;

        /* Clear the WKUP interrupt flag */
        hw_wkup_reset_key_interrupt();

        /* Mask the last bit from the enumeration, 0 stands for low 1 stands for high state */
        trigger = hw_wkup_get_trigger(KEY1_PORT, KEY1_PIN) & (1 << 0);

        event = ((trigger == WKUP_TRIGGER_STATE) ? WKUP_KEY_PRESS_EVENT_NOTIF : WKUP_KEY_RELEASE_EVENT_NOTIF);

        pin_wkup_conf.pin_trigger[KEY1_PORT] = (!trigger << KEY1_PIN) | (GPIO_TRIGGER_STATE << KEY2_PIN);

        hw_wkup_configure(&pin_wkup_conf);

        OS_TASK_NOTIFY_FROM_ISR(task_h, event, OS_NOTIFY_SET_BITS);
}
#endif

#if (dg_configWKUP_GPIO_P1_BLOCK_ENABLE)
void wkup_gpio_interrupt_cb(void)
{
        uint32_t status, event = 0;

        /* Clear the WKUP interrupt flag */
        hw_wkup_reset_key_interrupt();

        /* Get the status and polarity of the selected port on last wakeup event. */
        status = hw_wkup_get_gpio_status(HW_GPIO_PORT_1);

        if( status & (1 << KEY1_PIN) )
                event = (hw_wkup_get_trigger(KEY1_PORT, KEY1_PIN) & (1 << 0)) ? WKUP_GPIO_P1_EVENT_NOTIF_HIGH : WKUP_GPIO_P1_EVENT_NOTIF_LOW;
        else if( status & (1 << KEY2_PIN) )
                event = (hw_wkup_get_trigger(KEY2_PORT, KEY2_PIN) & (1 << 0)) ? WKUP_GPIO_P1_EVENT_NOTIF_HIGH : WKUP_GPIO_P1_EVENT_NOTIF_LOW;

        /*
         * This function MUST be called by any GPIO interrupt handler,
         * to clear the interrupt latch status.
         */
        hw_wkup_clear_gpio_status(HW_GPIO_PORT_1, status);

        OS_TASK_NOTIFY_FROM_ISR(task_h, event, OS_NOTIFY_SET_BITS);
}
#endif

/* Initialize the WKUP controller */
static void wkup_init(void)
{

        /* Initialize the WKUP controller */
        hw_wkup_init(&pin_wkup_conf);

#if (dg_configWKUP_KEY_BLOCK_ENABLE)
        /*
         * Enable interrupts produced by the KEY block of the wakeup controller (debounce
         * circuitry) and register a callback function to hit following a KEY event.
         */
        hw_wkup_register_key_interrupt(wkup_deb_interrupt_cb, 2);
#endif

#if (dg_configWKUP_GPIO_P1_BLOCK_ENABLE)
        /*
         * Enable interrupts produced by the GPIO block of the wakeup controller
         * and register a callback function to hit following a KEY event.
         */
        hw_wkup_register_gpio_p1_interrupt(wkup_gpio_interrupt_cb, 1);
#endif
        /* Enable interrupts of WKUP controller */
        hw_wkup_enable_key_irq();
}

/*
 * Task functions.
 */
static OS_TASK_FUNCTION(extWakeUpTriggerTask, pvParameters);

static OS_TASK_FUNCTION(system_init, pvParameters)
{

        cm_sys_clk_init(sysclk_XTAL32M);

        cm_apb_set_clock_divider(apb_div1);
        cm_ahb_set_clock_divider(ahb_div1);
        cm_lp_clk_init();

        /* Prepare the hardware to run this demo. */
        prvSetupHardware();

        /* Set the desired sleep mode. */
        pm_sleep_mode_set(pm_mode_extended_sleep);

        /* Set the desired wakeup mode. */
        pm_set_sys_wakeup_mode(pm_sys_wakeup_mode_normal);

        OS_TASK_CREATE( "ExternalWakeUp",            /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                        extWakeUpTriggerTask,                /* The function that implements the task. */
                        NULL,                           /* The parameter passed to the task. */
                        OS_MINIMAL_TASK_STACK_SIZE,
                                                        /* The number of bytes to allocate to the
                                                           stack of the task. */
                        mainTEMPLATE_TASK_PRIORITY,     /* The priority assigned to the task. */
                        task_h );                       /* The task handle */
        OS_ASSERT(task_h);

        /* the work of the SysInit task is done */
        OS_TASK_DELETE( xHandle );
}

/**
 * @brief Template main creates a SysInit task, which creates a Template task
 */
int main( void )
{
        OS_BASE_TYPE status;


        /* Start the two tasks as described in the comments at the top of this
        file. */
        status = OS_TASK_CREATE("SysInit",              /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                        system_init,                    /* The System Initialization task. */
                        ( void * ) 0,                   /* The parameter passed to the task. */
                        OS_MINIMAL_TASK_STACK_SIZE,
                                                        /* The number of bytes to allocate to the
                                                           stack of the task. */
                        OS_TASK_PRIORITY_HIGHEST,       /* The priority assigned to the task. */
                        xHandle );                      /* The task handle */
        OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);



        /* Start the tasks and timer running. */
        OS_TASK_SCHEDULER_RUN();

        /* If all is well, the scheduler will now be running, and the following
        line will never be reached.  If the following line does execute, then
        there was insufficient FreeRTOS heap memory available for the idle and/or
        timer tasks to be created.  See the memory management section on the
        FreeRTOS web site for more details. */
        for ( ;; );

}

/**
 * @brief Template task increases a counter every mainCOUNTER_FREQUENCY_MS ms
 */
static OS_TASK_FUNCTION(extWakeUpTriggerTask, pvParameters)
{
        uint32_t ulNotifiedValue;

        printf("Wake-up Controller Demonstration Sample Code.\n\r");

        wkup_init();

        for ( ;; ) {
                /* Wait for the external interruption notification */
                OS_TASK_NOTIFY_WAIT(0x0, OS_TASK_NOTIFY_ALL_BITS, &ulNotifiedValue, OS_TASK_NOTIFY_FOREVER);
                /* Check the notification is the expected value */

                if(ulNotifiedValue & WKUP_KEY_PRESS_EVENT_NOTIF) {
                        printf("Key press occurred\n\r");
                }

                if(ulNotifiedValue & WKUP_KEY_RELEASE_EVENT_NOTIF) {
                        printf("Key release occurred\n\r");
                }

                if(ulNotifiedValue & WKUP_GPIO_P1_EVENT_NOTIF_LOW) {
                        printf("GPIO pulse low occurred\n\r");
                }

                if(ulNotifiedValue & WKUP_GPIO_P1_EVENT_NOTIF_HIGH) {
                        printf("GPIO pulse high occurred\n\r");
                }

                fflush(stdout);
        }

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
static void prvSetupHardware( void )
{
#if (dg_configWKUP_GPIO_P1_BLOCK_ENABLE) || \
    ( (dg_configWKUP_KEY_BLOCK_ENABLE) && (!dg_configUSE_SYS_USB) && (!dg_configENABLE_DEBUGGER) )
        uint32_t pdc_wkup_combo_id;
#endif

        /*
         * The IRQ produced by the KEY sub block of the wakeup controller (debounced IO
         * IRQ) is multiplexed with other trigger sources (VBUS IRQ, SYS2CMAC IRQ, JTAG
         * present) in a single PDC peripheral trigger ID (HW_PDC_PERIPH_TRIG_ID_COMBO).
         */
#if (dg_configWKUP_KEY_BLOCK_ENABLE)
#if (!dg_configENABLE_DEBUGGER) && (!dg_configUSE_SYS_USB)

        pdc_wkup_combo_id = hw_pdc_add_entry(HW_PDC_LUT_ENTRY_VAL(
                                                HW_PDC_TRIG_SELECT_PERIPHERAL,
                                                HW_PDC_PERIPH_TRIG_ID_COMBO,
                                                HW_PDC_MASTER_CM33, 0));
        OS_ASSERT(pdc_wkup_combo_id != HW_PDC_INVALID_LUT_INDEX);

        /*  */
        hw_pdc_set_pending(pdc_wkup_combo_id);
        hw_pdc_acknowledge(pdc_wkup_combo_id);
#endif
#endif

#if (dg_configWKUP_GPIO_P1_BLOCK_ENABLE)
        pdc_wkup_combo_id = hw_pdc_add_entry(HW_PDC_LUT_ENTRY_VAL(
                                                KEY2_PORT,
                                                KEY2_PIN,
                                                HW_PDC_MASTER_CM33, 0));
        OS_ASSERT(pdc_wkup_combo_id != HW_PDC_INVALID_LUT_INDEX);

        hw_pdc_set_pending(pdc_wkup_combo_id);
        hw_pdc_acknowledge(pdc_wkup_combo_id);

        pdc_wkup_combo_id = hw_pdc_add_entry(HW_PDC_LUT_ENTRY_VAL(
                                                KEY1_PORT,
                                                KEY1_PIN,
                                                HW_PDC_MASTER_CM33, 0));
        OS_ASSERT(pdc_wkup_combo_id != HW_PDC_INVALID_LUT_INDEX);

        hw_pdc_set_pending(pdc_wkup_combo_id);
        hw_pdc_acknowledge(pdc_wkup_combo_id);
#endif

        /* Init hardware */
        pm_system_init(periph_init);

        /* Enable the COM power domain before handling any GPIO pin */
        hw_sys_pd_com_enable();

        /* Configure the KEY1 push button on Pro DevKit */
        HW_GPIO_SET_PIN_FUNCTION(KEY1);
        HW_GPIO_PAD_LATCH_ENABLE(KEY1);
        /* Lock the mode of the target GPIO pin */
        HW_GPIO_PAD_LATCH_DISABLE(KEY1);
/*
 * Wouldn't be valid to activate both key PDC entry and GPIO on another pin ?
 */
#if (dg_configWKUP_GPIO_P1_BLOCK_ENABLE)
        /* Configure the KEY2 push button on Pro DevKit */
        HW_GPIO_SET_PIN_FUNCTION(KEY2);
        HW_GPIO_PAD_LATCH_ENABLE(KEY2);
        /* Lock the mode of the target GPIO pin */
        HW_GPIO_PAD_LATCH_DISABLE(KEY2);
#endif

        /* Disable the COM power domain after handling the GPIO pins */
        hw_sys_pd_com_disable();
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
