/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief GPADC Adapter Application
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
#include "hw_watchdog.h"
#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"

#include "hw_pdc.h"
#include "ad_gpadc.h"
#include "hw_wkup.h"
#include "hw_sys.h"
#include "platform_devices.h"

/* Task priorities */
#define  mainGPADC_TASK_PRIORITY    ( OS_TASK_PRIORITY_NORMAL )

/* Enable/disable asynchronous SPI operations */
#define POT_ASYNC_EN                (1)

/* Retained symbols */
__RETAINED static OS_EVENT signal_pot;
__RETAINED static OS_EVENT signal_pot_async;

/* GPADC Task handle */
__RETAINED static OS_TASK prvGPADCTask_h;

uint32_t pdc_wkup_combo_id  __attribute__((unused));

/*
 * Perform any application specific hardware configuration.  The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware( void );

static OS_TASK xHandle;

/*
 * Task functions
 */
static void prvGPADCTask_POT(void *pvParameters);
/*static OS_TASK_FUNCTION(prvTemplateTask, pvParameters);*/

static void system_init(void *pvParameters)
{
        OS_BASE_TYPE status;

        REG_SETF(GPREG, DEBUG_REG, SYS_CPU_FREEZE_EN, 0);

#if defined CONFIG_RETARGET
        extern void retarget_init(void);
#endif /* CONFIG_RETARGET */

        /*
         * Prepare clocks. Note: cm_cpu_clk_set() and cm_sys_clk_set() can be called only
         * from a task since they will suspend the task until the XTAL32M has settled and,
         * maybe, the PLL is locked.
         */
        cm_sys_clk_init(sysclk_XTAL32M);
        cm_apb_set_clock_divider(apb_div1);
        cm_ahb_set_clock_divider(ahb_div1);
        cm_lp_clk_init();

        /* Prepare the hardware to run this demo */
        prvSetupHardware();

#if defined CONFIG_RETARGET
        retarget_init();
#endif /* CONFIG_RETARGET */


        OS_EVENT_CREATE(signal_pot);
        OS_EVENT_CREATE(signal_pot_async);

        /*
         * Upon a wakeup cycle, wait for the XTAL32M crystal to settle.
         * BLE, USB and UART blocks require the XTAL32M to be up and
         * running to work properly.
         */
        pm_set_wakeup_mode(true);


        /* Set the desired sleep mode. */
        pm_sleep_mode_set(pm_mode_extended_sleep);

        /*
         * Set the desired wakeup mode.
         *
         * \warning When set is Ultra-Fast wakeup mode, sleep voltage should be 0.9V
         *          and not less than that.
         *
         **/
        pm_set_sys_wakeup_mode(pm_sys_wakeup_mode_fast);

        /* GPADC task  */
        status = OS_TASK_CREATE("GPADC",     /* The text name assigned to the task, for
                                             debug only; not used by the kernel. */
                        prvGPADCTask_POT,    /* The function that implements the task. */
                        NULL,               /* The parameter passed to the task. */
                        1024 * OS_STACK_WORD_SIZE,  /* Stack size allocated for the task
                                                    in bytes. */
                        mainGPADC_TASK_PRIORITY, /* The priority assigned to the task. */
                        prvGPADCTask_h );       /* The task handle. */
        OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);

        /* The work of the SysInit task is done */
        OS_TASK_DELETE(xHandle);
}

#if 0
/*
 * Function for converting a raw ADC value to mV
 *
 * \param [in]  src     The GPADC instance
 * \param [in]  value   The raw ADC value
 *
 * \return The converted raw ADC value in millivolt
 *
 */
int convert_raw_to_mv(gpadc_device src, int value)
{
        uint32_t mv_src_max;
        gpadc_device cfg = src;

        const uint16 adc_src_max = ad_gpadc_get_source_max(cfg->drv);

        switch (cfg->drv->input_attenuator) {
        case HW_GPADC_INPUT_VOLTAGE_UP_TO_0V9:
                mv_src_max = 900;
                break;
        case HW_GPADC_INPUT_VOLTAGE_UP_TO_1V8:
                mv_src_max = 1800;
                break;
        case HW_GPADC_INPUT_VOLTAGE_UP_TO_2V7:
                mv_src_max = 2700;
                break;
        case HW_GPADC_INPUT_VOLTAGE_UP_TO_3V6:
                mv_src_max = 3600;
                break;
        default:
                mv_src_max = 3600;
                /* Invalid input attenuator */
                OS_ASSERT(0);
        }

        int ret = 0;

        switch (cfg->drv->input_mode) {
        case HW_GPADC_INPUT_MODE_SINGLE_ENDED:
                if (cfg->drv->positive == HW_GPADC_INP_VBAT) {
                        mv_src_max = 5000;
                }
                ret =  (mv_src_max * value) / adc_src_max;
                break;
        case HW_GPADC_INPUT_MODE_DIFFERENTIAL:
                ret = ((int)mv_src_max * (value - (adc_src_max >> 1))) /
                        (adc_src_max >> 1);
                break;
        default:
                /* Invalid input mode */
                OS_ASSERT(0);
        }

        return ret;
}
#endif


#if POT_ASYNC_EN == 1
/*
 * Callback function for GPADC asynchronous operations:
 *
 * \param [in]  user_data  Data that can be passed and used within the function
 * \param [out] value      The raw ADC value
 *
 */
void pot_gpadc_cb(void *user_data, int value)
{
        /* Signal the [prvGPADCTask_POT] task that time for resuming has elapsed. */
        OS_EVENT_SIGNAL_FROM_ISR(signal_pot_async);
}
#endif


/* Perform a GPADC read operation */
static void pot_gpadc_reader(gpadc_device dev)
{
        int error_code;
        uint16_t raw_val, mv_val;

        /* Open the GPADC device */
        ad_gpadc_handle_t pot_dev = ad_gpadc_open(dev);

#if POT_ASYNC_EN == 0
        /*
         * Perform a synchronous GPADC read operation, that is, the task
         * is blocking waiting for the operation to finish.
         */
        error_code = ad_gpadc_read_nof_conv(pot_dev, 1, &raw_val);
#else
        /*
         * Perform an asynchronous GPADC read operation, that is, the task does not
         * block waiting for the transaction to finish. Upon operation completion
         * callback function is triggered indicating the completion of the GPADC operation
         */
        error_code = ad_gpadc_read_nof_conv_async(pot_dev, 1, &raw_val, pot_gpadc_cb, NULL);

        /*
         * In the meantime and while GPADC operations are performed in the background,
         * application task can proceed to other operations/calculation. It is essential
         * that the new operations do not involve GPADC operations on the already
         * occupied block!!!
         */

        /*
         * Make sure that the current GPADC operation has finished,
         * blocking here forever.
         */
        OS_EVENT_WAIT(signal_pot_async, OS_EVENT_FOREVER);
#endif

        /* Convert the raw ADC value to mV */
        mv_val = ad_gpadc_conv_to_mvolt(POT_DEVICE->drv, raw_val);

        /* Close the GPADC device */
        ad_gpadc_close(pot_dev, true);

        /* Print on the serial console the status of the GPADC operation */
        if (error_code == AD_GPADC_ERROR_NONE) {
                printf("\n\rPOT value (raw): %d\n\rPOT value (analog): %d mV\n\r",
                        raw_val, mv_val);
        } else {
                printf("\n\rUnsuccessful GPADC write operation with error code: %d\n\r",
                        error_code);
        }
        fflush(stdout);
}

/**
 * @brief GPADC task
 */
static void prvGPADCTask_POT(void *pvParameters)
{

        printf("\n\r***GPADC Demonstration Example***\n\r\n");

        /*
         * GPADC adapter initialization should be done once at the beginning.
         * Alternatively, this function could be called during system
         * initialization in system_init().
         */
        /*ad_gpadc_init();*/

        for (;;) {
                /*
                 * Suspend task execution - As soon as WKUP callback function
                 * is triggered, the task resumes its execution.
                 */
                OS_EVENT_WAIT(signal_pot, OS_EVENT_FOREVER);

                /* Perform a GPADC read operation */
                pot_gpadc_reader(POT_DEVICE);
        }
}

/* WKUP KEY interrupt handler */
static void wkup_cb(void)
{

        /* Clear the WKUP interrupt flag!!! */
        hw_wkup_reset_key_interrupt();


        /*
         * Avoid using printf() within ISR context!!! It may crash your code.
         * Instead, use notifications to notify a task to perform an action!!
         */

        /*
         * Notify [prvGPADCTask_POT] task that time for performing GPADC operations
         * has elapsed.
         */
        OS_EVENT_SIGNAL_FROM_ISR(signal_pot);

}


/* Initialize the WKUP controller */
static void wkup_init(void)
{

        /* Initialize the WKUP controller */
        hw_wkup_init(NULL);

        /*
         * Set debounce time expressed in ms. Maximum allowable value is 63 ms.
         * A value set to 0 disables the debounce functionality.
         */
        hw_wkup_set_key_debounce_time(10);

        /*
         * Enable interrupts produced by the KEY block of the wakeup controller (debounce
         * circuitry) and register a callback function to hit following a KEY event.
         */
        hw_wkup_register_key_interrupt(wkup_cb, 1);

        /*
         * Set the polarity (rising/falling edge) that triggers the WKUP controller.
         *
         * \note The polarity is applied both to KEY and GPIO blocks of the controller
         *
         */
        hw_wkup_set_trigger(KEY1_PORT, KEY1_PIN, HW_WKUP_TRIG_LEVEL_LO_DEB);

        /* Enable interrupts of WKUP controller */
        hw_wkup_enable_key_irq();
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

        printf("\r\nError: insufficient heap memory\r\n"); // DEBUG
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
#if (DEVICE_FAMILY == DA1470X)
        hw_gpio_configure_pin(HW_GPIO_PORT_0, HW_GPIO_PIN_5, HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_ADC, false);
        hw_gpio_configure_pin(HW_GPIO_PORT_0, HW_GPIO_PIN_6, HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_ADC, false);
        hw_gpio_configure_pin(HW_GPIO_PORT_0, HW_GPIO_PIN_27, HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_ADC, false);
        hw_gpio_configure_pin(HW_GPIO_PORT_0, HW_GPIO_PIN_30, HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_ADC, false);
#endif
}

/**
 * @brief Hardware Initialization
 */
static void prvSetupHardware(void)
{
        /*
         * The IRQ produced by the KEY sub block of the wakeup controller
         * (debounced IO IRQ) is multiplexed with other trigger sources
         * (VBUS IRQ, SYS2CMAC IRQ, JTAG present) in a single PDC peripheral
         * trigger ID (HW_PDC_PERIPH_TRIG_ID_COMBO).
         */
#if !defined(CONFIG_USE_BLE) && (!dg_configENABLE_DEBUGGER) && (!dg_configUSE_SYS_CHARGER)

        pdc_wkup_combo_id = hw_pdc_add_entry(HW_PDC_LUT_ENTRY_VAL(
                                                         HW_PDC_TRIG_SELECT_PERIPHERAL,
                                                         HW_PDC_PERIPH_TRIG_ID_COMBO,
                                                         HW_PDC_MASTER_CM33, 0));
        OS_ASSERT(pdc_wkup_combo_id != HW_PDC_INVALID_LUT_INDEX);

        /*  */
        hw_pdc_set_pending(pdc_wkup_combo_id);
        hw_pdc_acknowledge(pdc_wkup_combo_id);
#endif

        wkup_init();

        /* Init hardware */
        pm_system_init(periph_init);


        /* Enable the COM power domain before handling any GPIO pin */
        hw_sys_pd_com_enable();

        ad_gpadc_io_config(POT_DEVICE->id, POT_DEVICE->io, AD_IO_CONF_OFF);

        /* Configure the KEY1 push button on Pro DevKit */
        HW_GPIO_SET_PIN_FUNCTION(KEY1);
        HW_GPIO_PAD_LATCH_ENABLE(KEY1);

        /* Lock the mode of the target GPIO pin */
        HW_GPIO_PAD_LATCH_DISABLE(KEY1);

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
