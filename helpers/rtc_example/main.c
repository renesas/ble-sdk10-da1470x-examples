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
#include "hw_rtc.h"
#include "hw_pdc.h"
#include "qspi_automode.h"
#include "sys_power_mgr.h"
#include "hw_pmu.h"
#include "cmsis_gcc.h"

/*
 * Notification bits reservation
 *
 * Bit #0 is always assigned to BLE event queue notification.
 */
#define RTC_ALARM_NOTIF                 (1 << 1)
#define RTC_EVENT_NOTIF                 (1 << 2)


/* Interrupt counter */
__RETAINED uint32_t rtc_irq_occured;

__RETAINED uint8_t rtc_event_irq, rtc_alarm_irq;

/* RTC event flag */
__RETAINED uint8_t rtc_event;

/* Task priorities */
#define mainTEMPLATE_TASK_PRIORITY              ( OS_TASK_PRIORITY_NORMAL )

/* The rate at which data is template task counter is incremented. */
#define mainCOUNTER_FREQUENCY_MS                OS_MS_2_TICKS(200)
/*
 * Perform any application specific hardware configuration.  The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware( void );
/*
 * Task functions .
 */
static void prvTemplateTask( void *pvParameters );

static OS_TASK xHandle;
__RETAINED static OS_TASK task_h;

static void system_init( void *pvParameters )
{
        task_h = NULL;

#if defined CONFIG_RETARGET
        extern void retarget_init(void);
#endif

        cm_sys_clk_init(sysclk_XTAL32M);

        cm_apb_set_clock_divider(apb_div1);
        cm_ahb_set_clock_divider(ahb_div1);
        cm_lp_clk_init();

        /* Prepare the hardware to run this demo. */
        prvSetupHardware();

#if defined CONFIG_RETARGET
        retarget_init();
#endif

//        pm_set_wakeup_mode(true);
        /* Set the desired sleep mode. */
        pm_sleep_mode_set(pm_mode_extended_sleep);


        /* Start main task here (text menu available via UART1 to control application) */
        OS_TASK_CREATE( "Template",            /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                        prvTemplateTask,                /* The function that implements the task. */
                        NULL,                           /* The parameter passed to the task. */
                        200 * OS_STACK_WORD_SIZE,
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
        for ( ;; );
}

void RTC_Event_Handler(void)
{
        rtc_event_irq++;
        hw_rtc_pdc_event_clear();
        OS_TASK_NOTIFY_FROM_ISR(task_h, RTC_EVENT_NOTIF, OS_NOTIFY_SET_BITS);
}

static void rtc_alarm_wakeup_hdlr(uint8_t event)
{
        rtc_alarm_irq++;
        hw_rtc_pdc_event_clear();
        OS_TASK_NOTIFY_FROM_ISR(task_h, RTC_ALARM_NOTIF, OS_NOTIFY_SET_BITS);
}

static void rtc_time_rollover_interrupt_test_hdlr(uint8_t event)
{
        rtc_irq_occured++;
        if (((event & HW_RTC_EVENT_SEC) == HW_RTC_EVENT_SEC) &&
            ((event & HW_RTC_EVENT_HSEC) == HW_RTC_EVENT_HSEC) &&
            ((event & HW_RTC_EVENT_MIN) == HW_RTC_EVENT_MIN) &&
            ((event & HW_RTC_EVENT_HOUR) == HW_RTC_EVENT_HOUR)) {
                rtc_event = event;
                hw_rtc_stop();
        }
}

static void rtc_date_rollover_interrupt_test_hdlr(uint8_t event)
{
        rtc_irq_occured++;
        if (((event & HW_RTC_EVENT_MDAY) == HW_RTC_EVENT_MDAY) &&
            ((event & HW_RTC_EVENT_MONTH) == HW_RTC_EVENT_MONTH)) {
                rtc_event = event;
                hw_rtc_stop();
        }
}

static void rtc_time_alarm_interrupt_test_hdlr(uint8_t event)
{
        rtc_irq_occured++;
        if((event & HW_RTC_EVENT_ALRM) == HW_RTC_EVENT_ALRM) {
                rtc_event = event;
                hw_rtc_stop();
        }
}

static void rtc_test_print_time_date(const hw_rtc_time_t *time, const hw_rtc_calendar_t *clndr)
{
        printf("\tRTC IRQ %ld evnt 0x%x\r\n", rtc_irq_occured, rtc_event);
        if (clndr) {
                printf("\tDate %d:%d:%d\r\n", clndr->year, clndr->month, clndr->mday);
        }
        if (time) {
                printf("\tTime %d:%d:%d:%d\r\n", time->hour, time->minute, time->sec, time->hsec);
        }
        fflush(stdout);
}

static void rtc_test_print_time(const hw_rtc_time_t *time)
{
        if (time) {
                printf("\tTime %d:%d:%d:%d\r\n", time->hour, time->minute, time->sec, time->hsec);
        }
        fflush(stdout);
}

/**
 * @brief Template task increases a counter every mainCOUNTER_FREQUENCY_MS ms
 */
static void prvTemplateTask( void *pvParameters )
{
//        OS_TICK_TIME xNextWakeTime;
//        static uint32_t test_counter=0;

        hw_rtc_time_t rd_time, set_time, alarm_time;
        hw_rtc_calendar_t rd_clndr;
        hw_rtc_alarm_calendar_t alarm_clndr;

        hw_rtc_time_t time = {
                .hour_mode = RTC_24H_CLK,
                .pm_flag = 0,
                .hour = 11,
                .minute = 59,
                .sec = 59,
                .hsec = 98,
        };

        hw_rtc_calendar_t clndr = {
                .year = 2020,
                .month = 1,
                .mday = 31,
                .wday = 5,
        };

        rtc_irq_occured = 0;
        rtc_event = 0;

        printf("\r\n\t#### Delay RTC tests for 10 seconds for the XTAL to settle #####\r\n");
        // Get RTC value and print it
        hw_rtc_get_time_clndr(&rd_time, &rd_clndr);
        rtc_test_print_time_date(&rd_time, &rd_clndr);

        // Enable the RTC peripheral clock
        hw_rtc_clock_enable();

        OS_DELAY_MS(1000 * 10); // Delay 10 seconds for before you run the RTC tests. The XTAL needs 8seconds to settle

        hw_rtc_stop();
        // Set hour clock mode, time and data, register interrupt handler callback function and enable seconds interrupt
        hw_rtc_set_hour_clk_mode(RTC_24H_CLK);

        hw_rtc_set_time_clndr(&time, &clndr);
        // Get RTC value and print it
        hw_rtc_get_time_clndr(&rd_time, &rd_clndr);
        printf("\r\n\t#### Time roll over interrupt test #####\r\n");
        rtc_test_print_time_date(&rd_time, &rd_clndr);
        hw_rtc_get_event_flags(); // clear pending interrupts
        hw_rtc_register_intr(rtc_time_rollover_interrupt_test_hdlr,
                (HW_RTC_EVENT_HSEC | HW_RTC_EVENT_SEC | HW_RTC_EVENT_MIN | HW_RTC_EVENT_HOUR));

        // Start the RTC
        hw_rtc_start();
        OS_DELAY_MS(40);
        hw_rtc_interrupt_disable((HW_RTC_EVENT_HSEC | HW_RTC_EVENT_SEC | HW_RTC_EVENT_MIN | HW_RTC_EVENT_HOUR));  // Disable event interrupts

        // Get RTC value and print it
        hw_rtc_get_time_clndr(&rd_time, &rd_clndr);
        rtc_test_print_time_date(&rd_time, &rd_clndr);

        //------------------------------------------------------------------------------

        rtc_irq_occured = 0;
        rtc_event = 0;
        set_time = time;
        set_time.hour = 23;

        // Set hour clock mode, time and data, register interrupt handler callback function and enable seconds interrupt
        hw_rtc_set_hour_clk_mode(RTC_24H_CLK);

        hw_rtc_set_time_clndr(&set_time, &clndr);
        // Get RTC value and print it
        hw_rtc_get_time_clndr(&rd_time, &rd_clndr);
        printf("\r\n\t#### Date roll over interrupt test #####\r\n");
        rtc_test_print_time_date(&rd_time, &rd_clndr);
        hw_rtc_get_event_flags(); // clear pending interrupts
        hw_rtc_register_intr(rtc_date_rollover_interrupt_test_hdlr, (HW_RTC_EVENT_MDAY | HW_RTC_EVENT_MONTH));

        // Start the RTC
        hw_rtc_start();
        OS_DELAY_MS(40);
        hw_rtc_interrupt_disable((HW_RTC_EVENT_MDAY | HW_RTC_EVENT_MONTH));  // Disable event interrupts

        // Get RTC value and print it
        hw_rtc_get_time_clndr(&rd_time, &rd_clndr);
        rtc_test_print_time_date(&rd_time, &rd_clndr);

        //------------------------------------------------------------------------------

        rtc_irq_occured = 0;
        rtc_event = 0;
        set_time = time;
        set_time.hsec = 1;
        alarm_time = time;
        alarm_time.hour = 12;
        alarm_time.minute = 0;
        alarm_time.sec = 0; // alarm should fire in 1 secs
        alarm_time.hsec = 0;

        // Set hour clock mode, time and data, register interrupt handler callback function and enable seconds interrupt
        hw_rtc_set_hour_clk_mode(RTC_24H_CLK);

        hw_rtc_set_time_clndr(&set_time, &clndr);
        // Get RTC value and print it
        hw_rtc_get_time_clndr(&rd_time, &rd_clndr);
        printf("\r\n\t#### Time alarm interrupt test #####\r\n");
        rtc_test_print_time_date(&rd_time, &rd_clndr);
        hw_rtc_get_event_flags(); // clear pending interrupts
        hw_rtc_set_alarm(&alarm_time, NULL, (HW_RTC_ALARM_HSEC | HW_RTC_ALARM_SEC | HW_RTC_ALARM_MIN | HW_RTC_ALARM_HOUR));
        hw_rtc_register_intr(rtc_time_alarm_interrupt_test_hdlr, HW_RTC_INT_ALRM);

        // Start the RTC
        hw_rtc_start();
//        hw_clk_delay_usec(1000 * 1500); // 1.5 sec
        OS_DELAY_MS(1500);
        hw_rtc_interrupt_disable((HW_RTC_ALARM_HSEC | HW_RTC_ALARM_SEC | HW_RTC_ALARM_MIN | HW_RTC_ALARM_HOUR));  // Disable event interrupts

        // Get RTC value and print it
        hw_rtc_get_time_clndr(&rd_time, &rd_clndr);
        rtc_test_print_time_date(&rd_time, &rd_clndr);

        //------------------------------------------------------------------------------

        rtc_irq_occured = 0;
        rtc_event = 0;
        set_time = time;
        set_time.hour = 23;
        set_time.hsec = 1;
        alarm_clndr.mday = 1;
        alarm_clndr.month = 2;

        // Set hour clock mode, time and data, register interrupt handler callback function and enable seconds interrupt
        hw_rtc_set_hour_clk_mode(RTC_24H_CLK);

        hw_rtc_set_time_clndr(&set_time, &clndr);
        // Get RTC value and print it
        hw_rtc_get_time_clndr(&rd_time, &rd_clndr);
        printf("\r\n\t#### Date alarm interrupt test #####\r\n");
        rtc_test_print_time_date(&rd_time, &rd_clndr);
        hw_rtc_get_event_flags(); // clear pending interrupts
        hw_rtc_set_alarm(&alarm_time, &alarm_clndr, (HW_RTC_ALARM_MDAY | HW_RTC_ALARM_MONTH));
        hw_rtc_register_intr(rtc_time_alarm_interrupt_test_hdlr, HW_RTC_INT_ALRM);

        // Start the RTC
        hw_rtc_start();
//        hw_clk_delay_usec(1000 * 1500); // 1.5 sec
        OS_DELAY_MS(1500);
        hw_rtc_interrupt_disable((HW_RTC_ALARM_MDAY | HW_RTC_ALARM_MONTH));  // Disable event interrupts

        // Get RTC value and print it
        hw_rtc_get_time_clndr(&rd_time, &rd_clndr);
        rtc_test_print_time_date(&rd_time, &rd_clndr);

        //------------------------------------------------------------------------------
        rtc_irq_occured = 0;
        rtc_event = 0;
        rtc_event_irq = 0;

        alarm_time.hour = 12;
        alarm_time.minute = 0;
        alarm_time.sec = 0; // alarm should fire in 1 secs
        alarm_time.hsec = 0;

        // Set hour clock mode, time and data, register interrupt handler callback function and enable seconds interrupt
        hw_rtc_set_hour_clk_mode(RTC_24H_CLK);

        hw_rtc_set_time_clndr(&set_time, &clndr);
        // Get RTC value and print it
        hw_rtc_get_time_clndr(&rd_time, &rd_clndr);
        printf("\r\n\t####  Wake up M33 from RTC event test #####\r\n");
        rtc_test_print_time_date(&rd_time, &rd_clndr);
        hw_rtc_get_event_flags(); // clear pending interrupts
        NVIC_EnableIRQ(RTC_EVENT_IRQn);
        hw_rtc_interrupt_disable(0xFF);
        // Start the RTC
        hw_rtc_start();
        hw_rtc_set_pdc_event_period(199); // 2 secs

        //-- Set RTC to be enable to wake up PDC
        uint32_t idx2 = hw_pdc_add_entry( HW_PDC_LUT_ENTRY_VAL(HW_PDC_TRIG_SELECT_PERIPHERAL,
                                                             HW_PDC_PERIPH_TRIG_ID_RTC_TIMER,
                                                             HW_PDC_MASTER_CM33,
                                                             (dg_configENABLE_XTAL32M_ON_WAKEUP ? HW_PDC_LUT_ENTRY_EN_XTAL : 0)));

        // Activate PDC entry
        hw_pdc_set_pending(idx2);
        hw_pdc_acknowledge(idx2);

        hw_rtc_set_alarm(&alarm_time, NULL, (HW_RTC_ALARM_HSEC));
        hw_rtc_register_intr(rtc_alarm_wakeup_hdlr, HW_RTC_INT_ALRM);


        //-- Set PDC entry to wake up from RTC alarm event
        uint32_t idx1 = hw_pdc_add_entry( HW_PDC_LUT_ENTRY_VAL(HW_PDC_TRIG_SELECT_PERIPHERAL,
                                                              HW_PDC_PERIPH_TRIG_ID_RTC_ALARM,
                                                              HW_PDC_MASTER_CM33,
                                                              (dg_configENABLE_XTAL32M_ON_WAKEUP ? HW_PDC_LUT_ENTRY_EN_XTAL : 0)));

        // Activate PDC entry
        hw_pdc_set_pending(idx1);
        hw_pdc_acknowledge(idx1);

        hw_rtc_pdc_event_enable();

        for ( ;; ) {
                OS_BASE_TYPE ret __UNUSED;
                uint32_t notif;

                /*
                 * Wait on any of the notification bits, then clear them all
                 */
                ret = OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);
                /* Blocks forever waiting for the task notification. Therefore, the return value must
                 * always be OS_OK
                 */
                OS_ASSERT(ret == OS_OK);

                hw_rtc_get_time_clndr(&rd_time, &rd_clndr);
                rtc_test_print_time(&rd_time);
                if (notif & RTC_EVENT_NOTIF) {
                        printf("\r\n\t #### RTC EVENT triggered %d ####\r\n", rtc_event_irq);
                        fflush(stdout);
                }

                if (notif & RTC_ALARM_NOTIF) {
                        printf("\r\n\t #### RTC ALARM triggered %d ####\r\n", rtc_alarm_irq);
                        fflush(stdout);
                }
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


