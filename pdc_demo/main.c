/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief PDC test application
 *
 * Copyright (C) 2020-2021 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include <stdio.h>
#include "resmgmt.h"
#include "hw_gpio.h"
#include "hw_timer.h"
#include "hw_wkup.h"
#include "hw_pdc.h"
#include "hw_pd.h"

#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"
#include "sys_watchdog.h"

#define TIMER5_TRIG_PORT          HW_GPIO_PORT_0
#define TIMER5_TRIG_PIN           HW_GPIO_PIN_21 /* J28-pin14 */

#define TIMER6_TRIG_PORT          HW_GPIO_PORT_0
#define TIMER6_TRIG_PIN           HW_GPIO_PIN_23 /* J28-pin12 */

#define BUTTON_TRIG_PORT          HW_GPIO_PORT_0
#define BUTTON_TRIG_PIN           HW_GPIO_PIN_22 /* J28-pin18 */

#define BUTTON1_PORT              HW_GPIO_PORT_1
#define BUTTON1_PIN               HW_GPIO_PIN_22 /* Button1, S3 */

#define BUTTON2_PORT              HW_GPIO_PORT_1
#define BUTTON2_PIN               HW_GPIO_PIN_23 /* Button2, S4 */

#define BUTTON_NOTIF              (1 << 0)

#define DBG_PIN_TOGGLE(name)    hw_gpio_configure_pin(name##_PORT, name##_PIN,          \
                                        HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true);  \
                                hw_gpio_pad_latch_enable(name##_PORT, name##_PIN);      \
                                hw_gpio_set_inactive(name##_PORT, name##_PIN);          \
                                hw_gpio_pad_latch_disable(name##_PORT, name##_PIN)

/* Set the gpio mode and the polarity for the used Buttons */
#define GPIO_MODE                 HW_GPIO_MODE_INPUT_PULLUP
#define GPIO_POLARITY             HW_WKUP_TRIG_LEVEL_LO

#if dg_configUSE_WDOG
__RETAINED_RW int8_t idle_task_wdog_id = -1;
#endif

static OS_TASK xHandle;
static __RETAINED OS_TASK task_1;
static __RETAINED OS_TASK task_2;
static __RETAINED OS_TASK task_button;
static __RETAINED OS_MUTEX uart_busy;
/*
 * Perform any application specific hardware configuration.  The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware( void );

/*
 * Task functions .
 */
static void timer1Task( void *pvParameters );
static void timer2Task( void *pvParameters );
static void buttonTask( void *notified_value );

/* Callback that is triggered when HW_TIMER5 expires - every 1 second. */
static void timer5_handler(void)
{
        /* Toggle a GPIO to monitor the wakeup event from HW_TIMER5. */
        DBG_PIN_TOGGLE(TIMER5_TRIG);
        OS_TASK_NOTIFY_FROM_ISR(task_1, 1, OS_NOTIFY_VAL_WITH_OVERWRITE);
}

/* Callback that is triggered when HW_TIMER6 expires - every 510 milliseconds. */
static void timer6_handler(void)
{
        /* Toggle a GPIO to monitor the wakeup event from HW_TIMER6. */
        DBG_PIN_TOGGLE(TIMER6_TRIG);
        OS_TASK_NOTIFY_FROM_ISR(task_2, 1, OS_NOTIFY_VAL_WITH_OVERWRITE);
}

/* Callback that is triggered when Button1(S3) or Button2(S4) is pressed. */
static void button_handler(void)
{
        /*
         * Interrupt handler should always reset interrupt state, otherwise it will be called again.
         */
        hw_wkup_reset_key_interrupt();
        hw_wkup_clear_gpio_status(HW_GPIO_PORT_1, 0xFFFFFFFF);
        /* Toggle a GPIO to monitor the wakeup event from Button1 or Button2. */
        DBG_PIN_TOGGLE(BUTTON_TRIG);
        if (task_button) {
                OS_TASK_NOTIFY_FROM_ISR(task_button, BUTTON_NOTIF, OS_NOTIFY_SET_BITS);
        }
}

/* Function to configure and enable a HW_TIMER with its period and the corresponding callback function */
static void setup_timer(HW_TIMER_ID timer_id, uint32_t period, hw_timer_handler_cb handler)
{
        timer_config timer_cfg = {
                .clk_src = HW_TIMER_CLK_SRC_INT,
                .prescaler = 0,
                .mode = HW_TIMER_MODE_TIMER,
                .timer = {
                        .direction = HW_TIMER_DIR_UP,
                        .reload_val = period,
                        .free_run = false,
                },
        };

        hw_timer_init(timer_id, &timer_cfg);
        hw_timer_register_int(timer_id, handler);
        hw_timer_enable(timer_id);
}

void init_buttons(int port, hw_wkup_interrupt_cb cb)
{
        wkup_config buttons;

        /* Connect Button1 (S3) to GPIO P1_22, which will wakeup the system through PDC. */
        ASSERT_WARNING(BUTTON1_PORT == port);
        hw_gpio_set_pin_function(BUTTON1_PORT, BUTTON1_PIN, GPIO_MODE,
                HW_GPIO_FUNC_GPIO);
        hw_gpio_pad_latch_enable(BUTTON1_PORT, BUTTON1_PIN);
        hw_gpio_pad_latch_disable(BUTTON1_PORT, BUTTON1_PIN);

        /* Connect Button2 (S4) to GPIO P1_23, which will wakeup the system through PDC. */
        ASSERT_WARNING(BUTTON2_PORT == port);
        hw_gpio_set_pin_function(BUTTON2_PORT, BUTTON2_PIN, GPIO_MODE,
                HW_GPIO_FUNC_GPIO);
        hw_gpio_pad_latch_enable(BUTTON2_PORT, BUTTON2_PIN);
        hw_gpio_pad_latch_disable(BUTTON2_PORT, BUTTON2_PIN);

        hw_wkup_unregister_interrupts();

        hw_wkup_init(NULL);

        /* Configure Button1 and Button 2 to generate an IRQ with level sensitivity.
         * For level sensitivity : WKUP_SEL_GPIO_P[port]_REG[pin] = 1
         *                         WKUP_SEL1_GPIO_P[port]_REG[pin] = 0
         * Configure also the polarity to: LEVEL_HI -> to generate an IRQ when input goes low
         *                                 LEVEL_LO -> to generate an IRQ when input goes high
         * setting WKUP_POL_P[port]_REG[pin].
         * The above registers are configured calling hw_wkup_set_trigger().
         */
        hw_wkup_set_trigger(BUTTON1_PORT, BUTTON1_PIN, GPIO_POLARITY);
        hw_wkup_set_trigger(BUTTON2_PORT, BUTTON2_PIN, GPIO_POLARITY);

        /* Register the callback function that will be called when the buttons are pressed */
        if (port == HW_GPIO_PORT_0) {
                hw_wkup_register_gpio_p0_interrupt(cb, 1);
        } else if (port == HW_GPIO_PORT_1) {
                hw_wkup_register_gpio_p1_interrupt(cb, 1);
        } else {
                hw_wkup_register_gpio_p2_interrupt(cb, 1);
        }
}

static void system_init( void *pvParameters )
{
        uint32_t pdc_entry;
        int wakeup_port;

#if defined CONFIG_RETARGET
        extern void retarget_init(void);
#endif
        /* Set system clock to RCHS@32 MHz */
        cm_sys_clk_init(sysclk_XTAL32M);

        cm_apb_set_clock_divider(apb_div1);
        cm_ahb_set_clock_divider(ahb_div1);
        cm_lp_clk_init();

#if dg_configUSE_WDOG
        /*
         * Initialize platform watchdog
         */
        sys_watchdog_init();

        // Register the Idle task first.
        idle_task_wdog_id = sys_watchdog_register(false);
        ASSERT_WARNING(idle_task_wdog_id != -1);
        sys_watchdog_configure_idle_id(idle_task_wdog_id);
#endif

        resource_init();

        /* Prepare the hardware to run this demo. */
        prvSetupHardware();

        /* Add a PDC entry to wakeup M33 and start XTAL32M, whenever HW_TIMER5 is triggered. */
        pdc_entry = hw_pdc_add_entry(HW_PDC_LUT_ENTRY_VAL(HW_PDC_TRIG_SELECT_PERIPHERAL,
                HW_PDC_PERIPH_TRIG_ID_TIMER5, HW_PDC_MASTER_CM33, HW_PDC_LUT_ENTRY_EN_XTAL));
        ASSERT_WARNING(pdc_entry != HW_PDC_INVALID_LUT_INDEX);

        /* Add a PDC entry to wakeup M33 and enable PD_SNC, whenever HW_TIMER6 is triggered. */
        pdc_entry = hw_pdc_add_entry(HW_PDC_LUT_ENTRY_VAL(HW_PDC_TRIG_SELECT_PERIPHERAL,
                HW_PDC_PERIPH_TRIG_ID_TIMER6, HW_PDC_MASTER_CM33, HW_PDC_LUT_ENTRY_EN_SNC));
        ASSERT_WARNING(pdc_entry != HW_PDC_INVALID_LUT_INDEX);

        /* Configure a PDC entry to wakeup from a number of GPIO's in port 1 */
        wakeup_port = HW_GPIO_PORT_1;

        /* Add a PDC entry to wakeup M33, when Button1 or Button2 is pressed. */
        pdc_entry = hw_pdc_add_entry(
                HW_PDC_LUT_ENTRY_VAL(HW_PDC_TRIG_SELECT_PERIPHERAL,
                        wakeup_port == HW_GPIO_PORT_0 ?
                                                        HW_PDC_PERIPH_TRIG_ID_GPIO_P0 :
                        wakeup_port == HW_GPIO_PORT_1 ?
                                                        HW_PDC_PERIPH_TRIG_ID_GPIO_P1 :
                                                        HW_PDC_PERIPH_TRIG_ID_GPIO_P2,
                        HW_PDC_MASTER_CM33, 0));
        ASSERT_WARNING(pdc_entry != HW_PDC_INVALID_LUT_INDEX);

        /* Configure Button1 and Button2, when pressed to wakeup the system. */
        init_buttons(wakeup_port, button_handler);

#if defined CONFIG_RETARGET
        retarget_init();
#endif

        pm_set_wakeup_mode(true);

        /* Set the desired sleep mode. */
        pm_sleep_mode_set(pm_mode_extended_sleep);

        /* Set the desired wakeup mode. */
        pm_set_sys_wakeup_mode(pm_sys_wakeup_mode_normal);

#if dg_configSYSTEMVIEW
        SEGGER_SYSVIEW_Conf();
#endif

        OS_MUTEX_CREATE(uart_busy);

        /* Create a task, which will be triggered every second by HW_TIMER5 */
        OS_TASK_CREATE( "TaskTimer1",                     /* The text name assigned to the task, for
                                                            debug only; not used by the kernel. */
                      timer1Task,                        /* The function that implements the task. */
                      NULL,                              /* The parameter passed to the task. */
                      1000 * OS_STACK_WORD_SIZE,         /* The number of bytes to allocate to the
                                                            stack of the task. */
                      OS_TASK_PRIORITY_NORMAL,           /* The priority assigned to the task. */
                      task_1 );                          /* The task handle */

        OS_ASSERT(task_1);

        /* Create a task, which will be triggered every 510msec by HW_TIMER6 */
        OS_TASK_CREATE( "TaskTimer2",                     /* The text name assigned to the task, for
                                                            debug only; not used by the kernel. */
                       timer2Task,                       /* The function that implements the task. */
                       NULL,                             /* The parameter passed to the task. */
                       1000 * OS_STACK_WORD_SIZE,        /* The number of bytes to allocate to the
                                                            stack of the task. */
                       OS_TASK_PRIORITY_NORMAL,          /* The priority assigned to the task. */
                       task_2 );                         /* The task handle */

        OS_ASSERT(task_2);

        /* Create a task, which will be triggered asynchronous by Button1 or Button2 press */
        OS_TASK_CREATE( "TaskButton",                     /* The text name assigned to the task, for
                                                             debug only; not used by the kernel. */
                        buttonTask,                       /* The function that implements the task. */
                        NULL,                             /* The parameter passed to the task. */
                        2 * configMINIMAL_STACK_SIZE  * OS_STACK_WORD_SIZE,
                                                          /* The number of bytes to allocate to the
                                                             stack of the task. */
                        OS_TASK_PRIORITY_LOWEST,          /* The priority assigned to the task. */
                        task_button );                    /* The task handle */
        OS_ASSERT(task_button);

        /* the work of the SysInit task is done. */
        OS_TASK_DELETE( xHandle );
}

static void timer1Task( void *pvParameters )
{
        uint32_t ulNotifiedValue;
        OS_BASE_TYPE xResult;

        /* Setup HW_TIMER5 to be triggered every 1 second. */
        setup_timer(HW_TIMER5, dg_configXTAL32K_FREQ, timer5_handler);

        while (true) {
                xResult = OS_TASK_NOTIFY_WAIT(0x0, OS_TASK_NOTIFY_ALL_BITS, &ulNotifiedValue, OS_TASK_NOTIFY_FOREVER);

                if (xResult == OS_TASK_NOTIFY_SUCCESS) {
                        OS_MUTEX_GET(uart_busy, OS_MUTEX_FOREVER);
                        printf("Wakeup source: HW_TIMER5 switch on-> M33, XTAL32M-> ");

                        /* Check that XTAL32M is enabled. */
                        printf(hw_clk_check_xtalm_status() ? "[OK]\n\r" : "[FAIL]\n\r");
                        OS_MUTEX_PUT(uart_busy);
                }
        }
}

static void timer2Task( void *pvParameters )
{
        uint32_t ulNotifiedValue;
        OS_BASE_TYPE xResult;

        /* Setup HW_TIMER6 to be triggered every 0.51 seconds. */
        setup_timer(HW_TIMER6, dg_configXTAL32K_FREQ/1.96, timer6_handler);

        while (true) {
                xResult = OS_TASK_NOTIFY_WAIT(0x0, OS_TASK_NOTIFY_ALL_BITS, &ulNotifiedValue, OS_TASK_NOTIFY_FOREVER);

                if (xResult == OS_TASK_NOTIFY_SUCCESS) {
                        OS_MUTEX_GET(uart_busy, OS_MUTEX_FOREVER);
                        printf("Wakeup source: HW_TIMER6 switch on-> M33, PD_SNC-> ");

                        /* Check that power domain for SNC is powered up. */
                        printf(hw_pd_check_snc_status() ? " [OK]\n\r" : " [FAIL]\n\r");
                        OS_MUTEX_PUT(uart_busy);
                }
        }
}

static void buttonTask( void *notified_value )
{
        OS_BASE_TYPE xResult;
        uint32_t notif;

        while (true) {

                /*
                 * Wait on any of the notification bits, then clear them all
                 */

                xResult = OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);

                if (xResult == OS_TASK_NOTIFY_SUCCESS) {
                        OS_MUTEX_GET(uart_busy, OS_MUTEX_FOREVER);
                        printf("Wakeup source: BUTTON    switch on-> M33\n\r");
                        OS_MUTEX_PUT(uart_busy);
                }
        }
}

int main( void )
{
        OS_BASE_TYPE status;

        status = OS_TASK_CREATE("SysInit",              /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                        system_init,                    /* The System Initialization task. */
                        NULL,                           /* The parameter passed to the task. */
                        configMINIMAL_STACK_SIZE * OS_STACK_WORD_SIZE,
                                                        /* The number of bytes to allocate to the
                                                           stack of the task. */
                        OS_TASK_PRIORITY_HIGHEST,       /* The priority assigned to the task. */
                        xHandle );                      /* The task handle */
        OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);

        /* Start the tasks and timer running. */
        vTaskStartScheduler();

        /* If all is well, the scheduler will now be running, and the following
        line will never be reached.  If the following line does execute, then
        there was insufficient FreeRTOS heap memory available for the idle and/or
        timer tasks to be created.  See the memory management section on the
        FreeRTOS web site for more details. */
        for (;;);
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
        /* Init hardware */
        pm_system_init(periph_init);
}

/**
 * @brief Malloc fail hook
 */
void vApplicationMallocFailedHook( void )
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
void vApplicationIdleHook( void )
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

#if dg_configUSE_WDOG
        sys_watchdog_notify(idle_task_wdog_id);
#endif
}

/**
 * @brief Application stack overflow hook
 */
void vApplicationStackOverflowHook( OS_TASK pxTask, char *pcTaskName )
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
void vApplicationTickHook( void )
{
}
