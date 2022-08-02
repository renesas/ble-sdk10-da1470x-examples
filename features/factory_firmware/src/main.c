/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief LCD Controller Demo application
 *
 * Copyright (C) 2021-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include "osal.h"
#include "resmgmt.h"

#include "ad_ble.h"
#include "ad_nvms.h"
#include "ad_nvparam.h"
#include "ble_mgr.h"

#include "hw_cpm.h"
#include "hw_gpio.h"
#include "hw_sys.h"
#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"
#include "sys_watchdog.h"
#include "platform_devices.h"
#include "hw_wkup.h"
#include "hw_pdc.h"
#include "ad_pmu.h"
#include "gdi.h"

#include "sys_usb.h"

#ifdef CONFIG_RTT
#include "SEGGER_RTT.h"
#endif

/**
 * \brief Button task priority
 */
#define LCDC_DEMO_TASK_PRIORITY         (OS_TASK_PRIORITY_NORMAL)
#define mainPXP_REPORTER_TASK_PRIORITY  ( OS_TASK_PRIORITY_NORMAL )

/**
 * \brief Task handler
 */

/* The configCHECK_FOR_STACK_OVERFLOW setting in FreeRTOSConifg can be used to
 check task stacks for overflows.  It does not however check the stack used by
 interrupts.  This demo has a simple addition that will also check the stack used
 by interrupts if mainCHECK_INTERRUPT_STACK is set to 1.  Note that this check is
 only performed from the tick hook function (which runs in an interrupt context).
 It is a good debugging aid - but won't catch interrupt stack problems until the
 tick interrupt next executes. */
//#define mainCHECK_INTERRUPT_STACK			1
#if mainCHECK_INTERRUPT_STACK == 1
const unsigned char ucExpectedInterruptStackValues[] = { 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC };
#endif

#if dg_configUSE_WDOG
INITIALISED_PRIVILEGED_DATA int8_t idle_task_wdog_id = -1;
#endif

void MainTask(void);

/*
 * Perform any application specific hardware configuration.  The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware(void);

OS_TASK_FUNCTION(pxp_reporter_task, pvParameters);

/*
 * System Initialization function.
 *
 * Set clocks and configure power domains. Configure hardware modules,
 * BLE manager and create the tasks running in this demo.
 *
 */
static void system_init(void *pvParameters)
{
#if defined CONFIG_RETARGET
        extern void retarget_init(void);
#endif

        OS_TASK handle;
        sys_clk_t sys_clk_prio[5] = {
                 sysclk_PLL160,
                 sysclk_XTAL32M,
                 sysclk_RCHS_96,
                 sysclk_RCHS_32,
                 sysclk_RCHS_64,
         };

        cm_sys_clk_set_priority(sys_clk_prio);
        cm_sys_clk_init(sysclk_XTAL32M);
        cm_apb_set_clock_divider(apb_div1);
        cm_ahb_set_clock_divider(ahb_div1);
        cm_lp_clk_init();

        REG_SETF(CRG_VSYS, VSYS_GEN_CTRL_REG, CURLIM_SET, 0x66);

        /* Initialize platform watchdog */
        sys_watchdog_init();

#if dg_configUSE_WDOG
        /* Register the Idle task first */
        idle_task_wdog_id = sys_watchdog_register(false);
        ASSERT_WARNING(idle_task_wdog_id != -1);
        sys_watchdog_configure_idle_id(idle_task_wdog_id);
#endif

#if dg_configSYSTEMVIEW
        SEGGER_SYSVIEW_Conf();
#endif /* dg_configSYSTEMVIEW */

#ifdef CONFIG_RTT
        SEGGER_RTT_Init();
#endif

        /* Prepare the hardware to run this demo. */
        prvSetupHardware();

        /* Set the desired sleep mode */
        pm_set_wakeup_mode(true);
        pm_sleep_mode_set(pm_mode_active);
        sys_usb_init();

        /* Initialize BLE Manager */
        ble_mgr_init();

#if defined CONFIG_RETARGET
        retarget_init();
#endif
#if (DEVICE_FAMILY == DA1469X)
        cm_sys_clk_request(sysclk_PLL96);
#elif (DEVICE_FAMILY == DA1470X)
#if DEVICE_FPGA
        cm_sys_clk_request(sysclk_XTAL32M);
#else
        cm_sys_clk_request(AD_LCDC_DEFAULT_CLK);
#endif /* DEVICE_FPGA */
#endif

        printf("Watch demo\r\nInitializing task...\r\n");

        hw_gpio_set_active(HW_GPIO_PORT_1, HW_GPIO_PIN_7);
        hw_gpio_pad_latch_enable(HW_GPIO_PORT_1, HW_GPIO_PIN_7);
        hw_gpio_pad_latch_disable(HW_GPIO_PORT_1, HW_GPIO_PIN_7);

        OS_DELAY_MS(100);

        /* Start the PXP reporter application task */
        OS_TASK_CREATE("PXP Reporter",                  /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                       pxp_reporter_task,               /* The function that implements the task. */
                       NULL,                            /* The parameter passed to the task. */
#if (dg_configDISABLE_BACKGROUND_FLASH_OPS == 1)
                       512,                             /* The number of bytes to allocate to the
                                                           stack of the task. */
#else
                       756,                             /* The number of bytes to allocate to the
                                                           stack of the task. */
#endif
                       mainPXP_REPORTER_TASK_PRIORITY,  /* The priority assigned to the task. */
                       handle);                         /* The task handle. */
        OS_ASSERT(handle);


        MainTask();
#ifdef PERFORMANCE_METRICS
        UISimulationTask();
#endif

        /* the work of the SysInit task is done */
        OS_TASK_DELETE(NULL);
}

/**
 * @brief Application Main function
 */
int main(void)
{
        OS_TASK handle;
        OS_BASE_TYPE status __UNUSED;

        /* Start SysInit task */
        status = OS_TASK_CREATE("SysInit",              /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                        system_init,                    /* The System Initialization task. */
                        ( void * ) 0,                   /* The parameter passed to the task. */
                        1024,                           /* The number of bytes to allocate to the
                                                           stack of the task. */
                        OS_TASK_PRIORITY_HIGHEST,       /* The priority assigned to the task. */
                        handle );                       /* The task handle */
        OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);

        /* Start the tasks and timer running. */
        vTaskStartScheduler();

        /* If all is well, the scheduler will now be running, and the following
         line will never be reached.  If the following line does execute, then
         there was insufficient FreeRTOS heap memory available for the idle and/or
         timer tasks	to be created.  See the memory management section on the
         FreeRTOS web site for more details. */
        for (;;);
}

#if dg_configUSE_HW_WKUP && defined(TOUCH_INT_PORT) && !dg_configUSE_TOUCH_SIMULATION
#define TOUCH_INT_PIN_MASK  (1 << TOUCH_INT_PIN)

/* WKUP P0 interrupt handler (ISR) */
static void _wkup_gpio_cb(void)
{
        uint32_t status;

        /* Clear the WKUP interrupt flag!!! (mandatory) */
        hw_wkup_reset_key_interrupt();

        /*
         * Get port status of the last wake-up event.
         *
         * \note: The status is returned as BITMASK, meaning that each bit reflects
         *        the corresponding pin state.
         */
        status = hw_wkup_get_gpio_status(TOUCH_INT_PORT);

        /*
         * Clear the interrupt latch status!!! (mandatory).
         */
        hw_wkup_clear_gpio_status(TOUCH_INT_PORT, status);

        /*
         * If interrupt is produced by the touch controller, then notify the GDI task.
         */
        if (status & TOUCH_INT_PIN_MASK) {
                gdi_touch_event();
        } else {
                /* Otherwise, notify other tasks (if needed) */
        }
}

/* PDC entry registration */
static void _wkup_event_pdc_entry_registration(void)
{
        uint32_t pdc_evt;

        /*
         * Add a PDC LUT entry so CM33 is triggered following an event on PORT0.
         * This is important because CM33 is completely turned off during sleep
         * mode.
         */
        pdc_evt = hw_pdc_add_entry(HW_PDC_LUT_ENTRY_VAL(TOUCH_INT_PORT, TOUCH_INT_PIN, HW_PDC_MASTER_CM33, 0));
        OS_ASSERT(pdc_evt != HW_PDC_ERROR_INVALID_LUT_ENTRY);

        /*
         * Manually trigger the previously defined PDC LUT entry. This guarantees that
         * PD_SYS remains active if the application invalidates all other PDC entries
         * that keep CM33 alive.
         */
        hw_pdc_set_pending(pdc_evt);

        /*
         * Acknowledge the already activated PDC entry, though not strictly needed,
         * as pending PDC entries related to CM33 are automatically acknowledged
         * before the system enters sleep.
         */
        hw_pdc_acknowledge(pdc_evt);
}

/* Setup interrupt for external sources */
static void _wkup_init(void)
{
        hw_wkup_init(NULL);

        /*
         * Triggering event polarity.
         *
         * \note The polarity is applied both to the KEY and GPIO sub-blocks of the WKUP controller
         *
         */
        hw_wkup_set_trigger(TOUCH_INT_PORT, TOUCH_INT_PIN, TOUCH_INT_POL);

        /*
         * Enable interrupts on the appropriate port and register a user-defined callback function.
         */
        if (TOUCH_INT_PORT == HW_GPIO_PORT_0) {
                hw_wkup_register_gpio_p0_interrupt(_wkup_gpio_cb, 1);
        } else if (TOUCH_INT_PORT == HW_GPIO_PORT_1) {
                hw_wkup_register_gpio_p1_interrupt(_wkup_gpio_cb, 1);
#if (DEVICE_FAMILY == DA1470X)
        } else if (TOUCH_INT_PORT == HW_GPIO_PORT_2) {
                hw_wkup_register_gpio_p2_interrupt(_wkup_gpio_cb, 1);
#endif
        }

        /* Add PDC LUT entry for events triggered on P0 */
        _wkup_event_pdc_entry_registration();

        /* Enable triggering WKUP interrupts */
        hw_wkup_enable_key_irq();
}
#endif /* dg_configUSE_HW_WKUP && defined(TOUCH_INT_PORT) && !dg_configUSE_TOUCH_SIMULATION */

/**
 * @brief Initialize the peripherals domain after power-up.
 *
 */
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
#endif /* DEVICE_FAMILY */

#if (DEVICE_FAMILY == DA1469X)
        /* USB data pin configuration */
        hw_gpio_set_pin_function(HW_GPIO_PORT_0, HW_GPIO_PIN_14, HW_GPIO_MODE_INPUT,
                                 HW_GPIO_FUNC_USB);
        hw_gpio_set_pin_function(HW_GPIO_PORT_0, HW_GPIO_PIN_15, HW_GPIO_MODE_INPUT,
                                 HW_GPIO_FUNC_USB);
#elif (DEVICE_FAMILY == DA1470X)
        /* USB data pin configuration */
        hw_gpio_set_pin_function(HW_GPIO_PORT_2, HW_GPIO_PIN_10, HW_GPIO_MODE_INPUT,
                                 HW_GPIO_FUNC_USB);
        hw_gpio_set_pin_function(HW_GPIO_PORT_2, HW_GPIO_PIN_11, HW_GPIO_MODE_INPUT,
                                 HW_GPIO_FUNC_USB);
#endif /* DEVICE_FAMILY */
#if dg_configUSE_HW_QSPI2
        /*
         * Disable the tCEM operation of QSPIC2 (no issues with the default PSRAM memory). Otherwise,
         * and given that FPGA runs @32MHz, burst transactions on the AHB bus will be split thus,
         * decreasing performance.
         */
        REG_CLR_BIT(QSPIC2, QSPIC2_MEMBLEN_REG, QSPIC_T_CEM_EN);
#endif
}

/**
 * @brief Hardware Initialization
 */
static void prvSetupHardware(void)
{
#if mainCHECK_INTERRUPT_STACK == 1
        extern unsigned long _vStackTop[], _pvHeapStart[];
        unsigned long ulInterruptStackSize;
#endif

        /* Init hardware */
        pm_system_init(periph_init);

        ad_pmu_rail_config_t v18_rail_config;

        v18_rail_config.enabled_onwakeup = true;
        v18_rail_config.enabled_onsleep = true;
        v18_rail_config.rail_1v8.current_onsleep = HW_PMU_1V8_MAX_LOAD_100;
        v18_rail_config.rail_1v8.current_onwakeup = HW_PMU_1V8_MAX_LOAD_100;
        v18_rail_config.rail_1v8.voltage_common = HW_PMU_1V8_VOLTAGE_1V8;

        ad_pmu_configure_rail(PMU_RAIL_1V8, &v18_rail_config);

        /* Make sure PD_COM is enabled before configuring any GPIO pin */
        hw_sys_pd_com_enable();


#if dg_configUSE_HW_WKUP && defined(TOUCH_INT_PORT) && !dg_configUSE_TOUCH_SIMULATION
        /* Setup interrupt for external sources */
        _wkup_init();

        hw_gpio_set_pin_function(TOUCH_INT_PORT, TOUCH_INT_PIN, TOUCH_INT_MODE, HW_GPIO_FUNC_GPIO);
        /* Latch/activate IO pin */
        hw_gpio_pad_latch_enable(TOUCH_INT_PORT, TOUCH_INT_PIN);
        /* Unlatch/lock IO pin */
        hw_gpio_pad_latch_disable(TOUCH_INT_PORT, TOUCH_INT_PIN);

        HW_GPIO_SET_PIN_FUNCTION(KEY1);
        HW_GPIO_PAD_LATCH_ENABLE(KEY1);
        HW_GPIO_PAD_LATCH_DISABLE(KEY1);

#endif /* dg_configUSE_HW_WKUP && defined(TOUCH_INT_PORT) && !dg_configUSE_TOUCH_SIMULATION */



        hw_gpio_configure_pin_power(HW_GPIO_PORT_1, HW_GPIO_PIN_7, HW_GPIO_POWER_V33);
        /* GPIO to activate the LCD LDO and power up after boot */
        //hw_gpio_configure_pin(HW_GPIO_PORT_1, HW_GPIO_PIN_7, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true);
        hw_gpio_set_pin_function(HW_GPIO_PORT_1, HW_GPIO_PIN_7, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO);

        /* Activate LCD board LDO (power)  */

//        hw_gpio_set_active(HW_GPIO_PORT_1, HW_GPIO_PIN_7);
//        hw_gpio_pad_latch_enable(HW_GPIO_PORT_1, HW_GPIO_PIN_7);
//        hw_gpio_pad_latch_disable(HW_GPIO_PORT_1, HW_GPIO_PIN_7);

        hw_sys_pd_com_disable();


#if dg_configLCDC_ADAPTER

#if dg_configUSE_HM80160A090
        ad_lcdc_io_config(&hm80160a090_io, AD_IO_CONF_OFF);
#elif dg_configUSE_E1394AA65A
        ad_lcdc_io_config(&e1394aa65a_io, AD_IO_CONF_OFF);
#elif dg_configUSE_LPM012M134B
        ad_lcdc_io_config(&lpm012m134b_io, AD_IO_CONF_OFF);
#elif dg_configUSE_NHD43480272EFASXN
        ad_lcdc_io_config(&nhd43480272efasxn_io, AD_IO_CONF_OFF);
#if dg_configUSE_FT5306
        ad_i2c_io_config(HW_I2C1, &ft5306_io, AD_IO_CONF_OFF); //! Make sure the touch controller is connected to the correct I2C driver
#endif
#elif dg_configUSE_DT280QV10CT
        ad_lcdc_io_config(&dt280qv10ct_io, AD_IO_CONF_OFF);
#elif dg_configUSE_T1D3BP006
        ad_lcdc_io_config(&t1d3bp006_io, AD_IO_CONF_OFF);
#elif dg_configUSE_T1D54BP002
        ad_lcdc_io_config(&t1d54bp002_io, AD_IO_CONF_OFF);
#elif dg_configUSE_PSP27801
        ad_lcdc_io_config(&psp27801_io, AD_IO_CONF_OFF);
#elif dg_configUSE_MCT024L6W240320PML
        ad_lcdc_io_config(&mct024l6w240320pml_io, AD_IO_CONF_OFF);
#elif dg_configUSE_LPM013M091A
        ad_lcdc_io_config(&lpm013m091a_io, AD_IO_CONF_OFF);
#elif dg_configUSE_LS013B7DH06
        ad_lcdc_io_config(&ls013b7dh06_io, AD_IO_CONF_OFF);
#elif dg_configUSE_LS013B7DH03
        ad_lcdc_io_config(&ls013b7dh03_io, AD_IO_CONF_OFF);
#elif dg_configUSE_ILI9341
        ad_lcdc_io_config(&ili9341_io,AD_IO_CONF_OFF);
#endif

#endif /* dg_configLCDC_ADAPTER */
#if dg_configUSE_FT6206
        ad_i2c_io_config(HW_I2C1, &ft6206_io, AD_IO_CONF_OFF);
#elif dg_configUSE_ZT2628
        ad_i2c_io_config(HW_I2C1, &zt2628_io, AD_IO_CONF_OFF);
#endif

#if mainCHECK_INTERRUPT_STACK == 1
        /* The size of the stack used by main and interrupts is not defined in
         the linker, but just uses whatever RAM is left.  Calculate the amount of
         RAM available for the main/interrupt/system stack, and check it against
         a reasonable number.  If this assert is hit then it is likely you don't
         have enough stack to start the kernel, or to allow interrupts to nest.
         Note - this is separate to the stacks that are used by tasks.  The stacks
         that are used by tasks are automatically checked if
         configCHECK_FOR_STACK_OVERFLOW is not 0 in FreeRTOSConfig.h - but the stack
         used by interrupts is not.  Reducing the conifgTOTAL_HEAP_SIZE setting will
         increase the stack available to main() and interrupts. */
        ulInterruptStackSize = ( ( unsigned long ) _vStackTop ) - ( ( unsigned long ) _pvHeapStart );
        OS_ASSERT( ulInterruptStackSize > 350UL );

        /* Fill the stack used by main() and interrupts to a known value, so its
         use can be manually checked. */
        memcpy( ( void * ) _pvHeapStart, ucExpectedInterruptStackValues, sizeof( ucExpectedInterruptStackValues ) );
#endif
}

/**
 * @brief Malloc fail hook
 */
void vApplicationMallocFailedHook(void)
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
        taskDISABLE_INTERRUPTS();
        for (;;);
}

/**
 * @brief Application idle task hook
 */
void vApplicationIdleHook(void)
{
        /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
         to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
         task.  It is essential that code added to this hook function never attempts
         to block in any way (for example, call OS_QUEUE_GET() with a block time
         specified, or call OS_DELAY()).  If the application makes use of the
         OS_TASK_DELETE() API function (as this demo application does) then it is also
         important that vApplicationIdleHook() is permitted to return to its calling
         function, because it is the responsibility of the idle task to clean up
         memory allocated by the kernel to any task that has since been deleted. */
#if (dg_configTRACK_OS_HEAP == 1)
        OS_BASE_TYPE i = 0;
        OS_BASE_TYPE uxMinimumEverFreeHeapSize;

        // Generate raw status information about each task.
        UBaseType_t uxNbOfTaskEntries = uxTaskGetSystemState(pxTaskStatusArray, mainMAX_NB_OF_TASKS, &ulTotalRunTime);

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
void vApplicationStackOverflowHook( OS_TASK pxTask, char *pcTaskName)
{
        (void)pcTaskName;
        (void)pxTask;

        /* Run time stack overflow checking is performed if
         configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
         function is called if a stack overflow is detected. */
        taskDISABLE_INTERRUPTS();
        for (;;);
}

/**
 * @brief Application tick hook
 */
void vApplicationTickHook(void)
{
#if mainCHECK_INTERRUPT_STACK == 1
        extern unsigned long _pvHeapStart[];

        /* This function will be called by each tick interrupt if
         configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
         added here, but the tick hook is called from an interrupt context, so
         code must not attempt to block, and only the interrupt safe FreeRTOS API
         functions can be used (those that end in FromISR()). */

        /* Manually check the last few bytes of the interrupt stack to check they
         have not been overwritten.  Note - the task stacks are automatically
         checked for overflow if configCHECK_FOR_STACK_OVERFLOW is set to 1 or 2
         in FreeRTOSConifg.h, but the interrupt stack is not. */
        OS_ASSERT( memcmp( ( void * ) _pvHeapStart, ucExpectedInterruptStackValues, sizeof( ucExpectedInterruptStackValues ) ) == 0U );
#endif /* mainCHECK_INTERRUPT_STACK */
}

