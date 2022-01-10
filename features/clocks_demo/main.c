/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief Clock manager test application
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
#include "hw_pmu.h"

#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"
#include "sys_watchdog.h"

#define PLL_USED_PORT                HW_GPIO_PORT_0
#define PLL_USED_PIN                 HW_GPIO_PIN_24 /* J28-pin15 */

#define BUTTON_PLL_REQ_PORT          HW_GPIO_PORT_0
#define BUTTON_PLL_REQ_PIN           HW_GPIO_PIN_23 /* J28-pin12 */

#define TIM1_PLL_REQ_PORT            HW_GPIO_PORT_0
#define TIM1_PLL_REQ_PIN             HW_GPIO_PIN_22 /* J28-pin18 */

#define TIM2_PLL_REQ_PORT            HW_GPIO_PORT_0
#define TIM2_PLL_REQ_PIN             HW_GPIO_PIN_21 /* J28-pin14 */

#define BUTTON_PORT                  HW_GPIO_PORT_1
#define BUTTON_PIN                   HW_GPIO_PIN_22 /* Button1, S3 */

#define BUTTON_NOTIF                 (1 << 0)
#define USING_PLL                    1

#define DBG_PIN_HIGH(name) hw_gpio_configure_pin(name##_PORT, name##_PIN,          \
                                   HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true);  \
                           hw_gpio_pad_latch_enable(name##_PORT, name##_PIN);      \
                           hw_gpio_pad_latch_disable(name##_PORT, name##_PIN)

#define DBG_PIN_LOW(name)  hw_gpio_configure_pin(name##_PORT, name##_PIN,          \
                                   HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false); \
                           hw_gpio_pad_latch_enable(name##_PORT, name##_PIN);      \
                           hw_gpio_pad_latch_disable(name##_PORT, name##_PIN)

#define DBG_PIN_STATE(name, state)      if ((state)) {                  \
                                                DBG_PIN_HIGH(name);     \
                                        } else {                        \
                                                DBG_PIN_LOW(name);      \
                                        }
__RETAINED static bool pll_enabled;
__RETAINED static bool rchs_enabled;

static __RETAINED OS_TASK task_1;
static __RETAINED OS_TASK task_2;
static __RETAINED OS_TASK task_button;
static OS_TASK xHandle;
static __RETAINED OS_MUTEX uart_busy;
static __RETAINED OS_MUTEX clk_check;

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
static void timer1Task( void *pvParameters );
static void timer2Task( void *pvParameters );
static void buttonTask( void *notified_value );
static void init_button( void );

/* Callback that is triggered when HW_TIMER3 expires- every 1 second */
static void trigger_task1( void )
{
        OS_TASK_NOTIFY_FROM_ISR(task_1, 1, OS_NOTIFY_VAL_WITH_OVERWRITE);
}

/* Callback that is triggered when HW_TIMER4 expires- every 90 milliseconds */
static void trigger_task2( void )
{
        OS_TASK_NOTIFY_FROM_ISR(task_2, 1, OS_NOTIFY_VAL_WITH_OVERWRITE);
}

static void check_v12_voltage(void)
{
        HW_PMU_1V2_RAIL_CONFIG rail_config;
        hw_pmu_get_1v2_active_config(&rail_config);
        if (hw_clk_get_sysclk_freq() >= dg_configRCHS_64M_FREQ) {
                ASSERT_WARNING(rail_config.voltage == HW_PMU_1V2_VOLTAGE_1V20);
        }
        else {
                ASSERT_WARNING(rail_config.voltage == HW_PMU_1V2_VOLTAGE_0V90);
        }
}

static void system_init( void *pvParameters )
{
        uint32_t pdc_entry = HW_PDC_INVALID_LUT_INDEX;

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

        /* Add a PDC entry to wakeup the system from HW_TIMER3 */
        pdc_entry = hw_pdc_add_entry(HW_PDC_LUT_ENTRY_VAL(HW_PDC_TRIG_SELECT_PERIPHERAL,
                HW_PDC_PERIPH_TRIG_ID_TIMER3, HW_PDC_MASTER_CM33, 0));
        ASSERT_WARNING(pdc_entry != HW_PDC_INVALID_LUT_INDEX);

        /* Add a PDC entry to wakeup the system from HW_TIMER4 */
        pdc_entry = hw_pdc_add_entry(HW_PDC_LUT_ENTRY_VAL(HW_PDC_TRIG_SELECT_PERIPHERAL,
                HW_PDC_PERIPH_TRIG_ID_TIMER4, HW_PDC_MASTER_CM33, 0));
        ASSERT_WARNING(pdc_entry != HW_PDC_INVALID_LUT_INDEX);

        /* Configure button functionality. */
        init_button();
        //for (volatile uint8_t i=0; i<200; i++);

        //Check all chock switch combinations and verify that voltage has been properly restored.

        cm_sys_clk_set(sysclk_RCHS_32);
        check_v12_voltage();
        cm_sys_clk_set(sysclk_RCHS_64);
        check_v12_voltage();
        cm_sys_clk_set(sysclk_RCHS_96);
        check_v12_voltage();
        cm_sys_clk_set(sysclk_PLL160);
        check_v12_voltage();
        cm_sys_clk_set(sysclk_RCHS_32);
        check_v12_voltage();
        cm_sys_clk_set(sysclk_PLL160);
        check_v12_voltage();
        cm_sys_clk_set(sysclk_RCHS_32);
        check_v12_voltage();
        cm_sys_clk_set(sysclk_RCHS_96);
        check_v12_voltage();
        cm_sys_clk_set(sysclk_PLL160);
        check_v12_voltage();
        cm_sys_clk_set(sysclk_RCHS_96);
        check_v12_voltage();
        cm_sys_clk_set(sysclk_PLL160);
        check_v12_voltage();
        cm_sys_clk_set(sysclk_RCHS_96);
        check_v12_voltage();
        cm_sys_clk_set(sysclk_XTAL32M);
        check_v12_voltage();
        cm_sys_clk_set(sysclk_RCHS_32);
        check_v12_voltage();
        cm_sys_clk_set(sysclk_RCHS_64);
        check_v12_voltage();
        cm_sys_clk_set(sysclk_RCHS_32);
        check_v12_voltage();
        cm_sys_clk_set(sysclk_RCHS_96);
        check_v12_voltage();
        cm_sys_clk_set(sysclk_XTAL32M);
        check_v12_voltage();
        cm_sys_clk_set(sysclk_PLL160);
        check_v12_voltage();
        cm_sys_clk_set(sysclk_RCHS_96);
        check_v12_voltage();
        cm_sys_clk_set(sysclk_RCHS_32);
        check_v12_voltage();

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
        OS_MUTEX_CREATE(clk_check);

        /* Create a task, which will be triggered every second by a timer */

        OS_TASK_CREATE( "TaskTimer1",                     /* The text name assigned to the task, for
                                                             debug only; not used by the kernel. */
                       timer1Task,                        /* The function that implements the task. */
                       NULL,                              /* The parameter passed to the task. */
                       1000 * OS_STACK_WORD_SIZE,         /* The number of bytes to allocate to the
                                                             stack of the task. */
                       OS_TASK_PRIORITY_NORMAL,           /* The priority assigned to the task. */
                       task_1 );                          /* The task handle */

        OS_ASSERT(task_1);

        /* Create a task, which will be triggered every 90msec by a timer */
        OS_TASK_CREATE( "TaskTimer2",                     /* The text name assigned to the task, for
                                                             debug only; not used by the kernel. */
                        timer2Task,                       /* The function that implements the task. */
                        NULL,                             /* The parameter passed to the task. */
                        1000 * OS_STACK_WORD_SIZE,        /* The number of bytes to allocate to the
                                                             stack of the task. */
                        OS_TASK_PRIORITY_NORMAL,          /* The priority assigned to the task. */
                        task_2 );                         /* The task handle */

        OS_ASSERT(task_2);

        /* Create a task, which will be triggered asynchronous by a button press (Button S3) */
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

        /* the work of the SysInit task is done */
        OS_TASK_DELETE( xHandle );
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

void check_pll_status(int task_id)
{
        /* Check PLL160 status */
        pll_enabled = (hw_clk_get_sysclk() == SYS_CLK_IS_PLL);
        DBG_PIN_STATE(PLL_USED, pll_enabled);

        /* Get the system clock frequency */
        uint32_t freq = hw_clk_get_sysclk_freq() / 1000000;

        OS_MUTEX_GET(uart_busy, OS_MUTEX_FOREVER);
        if (pll_enabled) {
                /* Switch to PLL160 succeeded */
                printf("[%03dMHz] Task%d PLL160 request: OK\n\r", freq, task_id);
        }
        else {
                /* Switch to PLL160 failed */
                printf("[%03dMHz] Task%d PLL160 request: ***Fail***\n\r", freq, task_id);
        }
        OS_MUTEX_PUT(uart_busy);
}

void check_rchs_status(int task_id, cm_sys_clk_set_status_t ret)
{
        /* Check PLL160 status */
        pll_enabled = (hw_clk_get_sysclk() == SYS_CLK_IS_PLL);
        DBG_PIN_STATE(PLL_USED, pll_enabled);

        /* Check RCHS status */
        rchs_enabled = (hw_clk_get_sysclk() == SYS_CLK_IS_RCHS);

        /* Get the system clock frequency */
        uint32_t freq = hw_clk_get_sysclk_freq() / 1000000;

        OS_MUTEX_GET(uart_busy, OS_MUTEX_FOREVER);
        if( ret == cm_sysclk_success ) {
                /* Switch to RCHS succeeded */
                ASSERT_WARNING(rchs_enabled);
                printf("[%03dMHz] Task%d RCHS request:   OK\n\r", freq, task_id);
        } else if (ret == cm_sysclk_pll_used_by_task) {
                /* Switch to RCHS failed. PLL is used by another task */
                ASSERT_WARNING(rchs_enabled == false);
                printf("[%03dMHz] Task%d RCHS request:   PLL used by other task\n\r", freq, task_id);
        } else {
                /* This is not expected to happen */
                ASSERT_WARNING(0);
        }
        OS_MUTEX_PUT(uart_busy);

}

static void timer1Task( void *pvParameters )
{
        uint32_t ulNotifiedValue;
        OS_BASE_TYPE xResult;
        cm_sys_clk_set_status_t ret;

        bool status = false;
        bool clock_changed = false;
        bool running_on_pll = false;

        /* Create Timer1(use HW_TIMER3) with frequency 1Hz to trigger Task1 - execution every 1sec */
        timer_config timer1_cfg = {
                .clk_src = HW_TIMER_CLK_SRC_INT,
                .prescaler = 0,
                .mode = HW_TIMER_MODE_TIMER,
                .timer = {
                        .direction = HW_TIMER_DIR_UP,
                        .reload_val = dg_configXTAL32K_FREQ,  // 1sec (1Hz).
                        .free_run = false,
                },
        };

        hw_timer_init(HW_TIMER3, &timer1_cfg);
        hw_timer_register_int(HW_TIMER3, trigger_task1);
        hw_timer_enable(HW_TIMER3);

        while (true) {
                xResult = OS_TASK_NOTIFY_WAIT(0x0, OS_TASK_NOTIFY_ALL_BITS, &ulNotifiedValue, OS_TASK_NOTIFY_FOREVER);
                if (xResult == OS_TASK_NOTIFY_SUCCESS) {
#if USING_PLL
                        /* Switch to PLL160MHz */
                        cm_sys_clk_set(sysclk_PLL160);
                        DBG_PIN_HIGH(TIM1_PLL_REQ);
                        check_pll_status(1);
#else
                        /* Switch to RCHS96MHz */
                        ret = cm_sys_clk_set(sysclk_RCHS_96);
                        DBG_PIN_HIGH(TIM1_PLL_REQ);
                        check_rchs_status(1, ret);
#endif

                        /* Do some heavy processing. A while loop is good enough for the demo. */
                        hw_clk_delay_usec(100000);

                        /* Wait for some time. PLL is still enabled. Task yields to allow other tasks to run. */
                        OS_DELAY_MS(200);

                        pm_sleep_mode_request(pm_mode_idle);
                        OS_DELAY_MS(200);
                        pm_sleep_mode_release(pm_mode_idle);

                        pm_sleep_mode_request(pm_mode_active);
                        OS_DELAY_MS(100);
                        pm_sleep_mode_release(pm_mode_active);

                        /* Use a mutex to make sure that the clock has not been changed by another task
                         * before checking its status
                         */
                        OS_MUTEX_GET(clk_check, OS_MUTEX_FOREVER);

                        /* Switch to RCHS@32MHz */
                        ret = cm_sys_clk_set(sysclk_RCHS_32);
                        DBG_PIN_LOW(TIM1_PLL_REQ);

                        check_rchs_status(1, ret);
                        OS_MUTEX_PUT(clk_check);
                }
        }
}

static void timer2Task( void *pvParameters )
{
        uint32_t ulNotifiedValue;
        OS_BASE_TYPE xResult;
        cm_sys_clk_set_status_t ret;

        /* Create Timer2 (use HW_TIMER4) with frequency 11Hz to trigger Task2 - execution every 90msec) */
        timer_config timer2_cfg = {
                .clk_src = HW_TIMER_CLK_SRC_INT,
                .prescaler = 0,
                .mode = HW_TIMER_MODE_TIMER,
                .timer = {
                        .direction = HW_TIMER_DIR_UP,
                        .reload_val = dg_configXTAL32K_FREQ / 11,  // 91msec (11Hz)
                        .free_run = false,
                },
        };

        hw_timer_init(HW_TIMER4, &timer2_cfg);
        hw_timer_register_int(HW_TIMER4, trigger_task2);
        hw_timer_enable(HW_TIMER4);

        while (true) {
                xResult = OS_TASK_NOTIFY_WAIT(0x0, OS_TASK_NOTIFY_ALL_BITS, &ulNotifiedValue, OS_TASK_NOTIFY_FOREVER);
                if (xResult == OS_TASK_NOTIFY_SUCCESS) {

#if USING_PLL
                        /* Switch to PLL160MHz */
                        cm_sys_clk_set(sysclk_PLL160);
                        DBG_PIN_HIGH(TIM2_PLL_REQ);

                        check_pll_status(2);

#else
                        /* Switch to RCHS96MHz */
                        ret = cm_sys_clk_set(sysclk_RCHS_96);
                        DBG_PIN_HIGH(TIM2_PLL_REQ);
                        check_rchs_status(2, ret);
#endif

                        /* Do some heavy processing. A while loop is good enough for the demo. */
                        hw_clk_delay_usec(30000);

                        /* Wait for some time. PLL is still enabled. Task yields to allow other tasks to run. */
                        OS_DELAY_MS(20);

                        /* Use a mutex to make sure that the clock has not been changed by another task
                         * before checking its status
                         */
                        OS_MUTEX_GET(clk_check, OS_MUTEX_FOREVER);

                        /* Switch to RCHS@32MHz */
                        ret = cm_sys_clk_set(sysclk_RCHS_32);
                        DBG_PIN_LOW(TIM2_PLL_REQ);

                        check_rchs_status(2, ret);
                        OS_MUTEX_PUT(clk_check);
                }
        }
}

static void buttonTask( void *notified_value )
{
        cm_sys_clk_set_status_t ret;
        while (true) {
                OS_BASE_TYPE xResult;
                uint32_t notif;

                /*
                 * Wait on any of the notification bits, then clear them all
                 */

                xResult = OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);
                if (xResult == OS_TASK_NOTIFY_SUCCESS) {

#if USING_PLL
                        /* Switch to PLL160MHz */
                        cm_sys_clk_set(sysclk_PLL160);
                        DBG_PIN_HIGH(BUTTON_PLL_REQ);

                        check_pll_status(3);
#else
                        /* Switch to RCHS96MHz */
                        ret = cm_sys_clk_set(sysclk_RCHS_96);
                        DBG_PIN_HIGH(BUTTON_PLL_REQ);
                        check_rchs_status(3, ret);
#endif
                        /* Use a mutex to make sure that the clock has not been changed by another task
                         * before checking its status
                         */
                        OS_MUTEX_GET(clk_check, OS_MUTEX_FOREVER);

                        /* Switch to RCHS@32MHz */
                        ret = cm_sys_clk_set(sysclk_RCHS_32);
                        DBG_PIN_LOW(BUTTON_PLL_REQ);

                        check_rchs_status(3, ret);
                        OS_MUTEX_PUT(clk_check);
                }
        }
}

static void wkup_handler()
{
        hw_wkup_reset_key_interrupt();
        if (task_button) {
                OS_TASK_NOTIFY_FROM_ISR(task_button, BUTTON_NOTIF, OS_NOTIFY_SET_BITS);
        }
}

static void init_button( void )
{
        hw_wkup_init(NULL);

        hw_wkup_set_trigger(BUTTON_PORT, BUTTON_PIN, HW_WKUP_TRIG_LEVEL_HI_DEB);
        hw_wkup_set_key_debounce_time(5);

        hw_wkup_register_key_interrupt(wkup_handler, 1);
        hw_wkup_enable_key_irq();
}

/**
 * @brief Initialize the peripherals domain after power-up.
 *
 */
static void periph_init(void)
{
        /* Configure Button1 S3 to trigger a task that will switch clock between PLL and RCHS@32MHz */
        hw_gpio_set_pin_function(BUTTON_PORT, BUTTON_PIN, HW_GPIO_MODE_INPUT_PULLUP,
                        HW_GPIO_FUNC_GPIO);
        hw_gpio_pad_latch_enable(BUTTON_PORT, BUTTON_PIN);
        hw_gpio_pad_latch_disable(BUTTON_PORT, BUTTON_PIN);
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
