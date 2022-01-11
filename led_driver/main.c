/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief FreeRTOS template application with retarget
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
#include "hw_led.h"
#include "hw_watchdog.h"
#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"


/* The rate at which data is template task counter is incremented. */
#define mainCOUNTER_FREQUENCY_MS                OS_MS_2_TICKS(20)

#define PWM_SPEED 100
#define PWM_INTENSITY (PWM_SPEED+1)/10

/* Task priorities */
#define mainTEMPLATE_TASK_PRIORITY              ( OS_TASK_PRIORITY_NORMAL )

/*
 * Perform any application specific hardware configuration.  The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware( void );
/*
 * Task functions .
 */
static void rgb_rotation_task( void *pvParameters );

static OS_TASK xHandle;

static void system_init( void *pvParameters )
{
        OS_TASK task_h = NULL;

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

        /* Set the desired sleep mode. */
        pm_sleep_mode_set(pm_mode_active);

        /* Set the desired wakeup mode. */
        // pm_set_sys_wakeup_mode(pm_sys_wakeup_mode_normal);

        /* Start main task here (text menu available via UART1 to control application) */
        OS_TASK_CREATE( "RGBTASK",                              /* The text name assigned to the task, for
                                                                   debug only; not used by the kernel. */
                        rgb_rotation_task,                      /* The function that implements the task. */
                        NULL,                                   /* The parameter passed to the task. */
                        configMINIMAL_STACK_SIZE * OS_STACK_WORD_SIZE,
                                                                /* The number of bytes to allocate to the
                                                                   stack of the task. */
                        mainTEMPLATE_TASK_PRIORITY,             /* The priority assigned to the task. */
                        task_h );                               /* The task handle */
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


/**
 * @brief this task rotates between the Red, Green and Blue colours of the RBG LED.
 *        it affects only one LED at any time
 */

void rgb_rotation_task( void *pvParameters )
{

        OS_TICK_TIME xNextWakeTime;
        // LED 1: Green, LED2: Red, LED3: Blue
        hw_led_config leds_config = {
                .leds_pwm_duty_cycle = {0, 0, 0},
                .leds_pwm_start_cycle = {0, 0, 0},
                .leds_pwm_frequency = PWM_SPEED
        };

        hw_led_pwm_duty_cycle_t current_led_duty = {0, 0};
        HW_LED_MSK current_led_msk;
        HW_LED_ID current_led_id;

        uint8_t current_color = 0;
        bool fade_in = true;

        // initialize all led duty cycles to 0
        hw_led_init(&leds_config);

        // enable LED drivers
        hw_led_on(HW_LED_ALL_LED_MASK);

        // start PWM
        hw_led_pwm_on(HW_LED_ALL_LED_MASK);

        // set LED density
        hw_led_pwm_set_load_sel(HW_LED_ID_LED_1, 0);
        hw_led_pwm_set_load_sel(HW_LED_ID_LED_2, 0);
        hw_led_pwm_set_load_sel(HW_LED_ID_LED_3, 0);

        // turn everything off to start rotation sequence
        hw_led_off(HW_LED_ALL_LED_MASK);

        /* Initialize xNextWakeTime - this only needs to be done once. */
        xNextWakeTime = OS_GET_TICK_COUNT();

        for( ;; ) {
                /* Place this task in the blocked state until it is time to run again.
                   The block time is specified in ticks, the constant used converts ticks
                   to ms.  While in the Blocked state this task will not consume any CPU
                   time. */
                vTaskDelayUntil( &xNextWakeTime, mainCOUNTER_FREQUENCY_MS );

                // choose LED to handle
                if (current_color == 0) {
                        current_led_id  = HW_LED_ID_LED_1;
                        current_led_msk = HW_LED_MSK_LED_1;
                } else if (current_color == 1) {
                        current_led_id  = HW_LED_ID_LED_2;
                        current_led_msk = HW_LED_MSK_LED_2;
                } else {
                        current_led_id  = HW_LED_ID_LED_3;
                        current_led_msk = HW_LED_MSK_LED_3;
                }

                // compute next PWM step
                if (fade_in) {
                        if (current_led_duty.hw_led_pwm_end == current_led_duty.hw_led_pwm_start
                                && current_led_duty.hw_led_pwm_start == 0) {
                                // start of sequence
                                hw_led_on(current_led_msk);
                        }
                        current_led_duty.hw_led_pwm_end += PWM_INTENSITY;
                } else {
                        current_led_duty.hw_led_pwm_end -= PWM_INTENSITY;
                }

                if(current_led_duty.hw_led_pwm_end >= PWM_SPEED) {
                        fade_in = false;
                        current_led_duty.hw_led_pwm_end = PWM_SPEED;
                } else if (current_led_duty.hw_led_pwm_end <= 0 ) {
                        hw_led_off(current_led_msk);
                        current_led_duty.hw_led_pwm_end = 0;
                        fade_in = true;
                        current_color = (current_color + 1) % 3;
                }

                // set LED DCs
                hw_led_pwm_set_duty_cycle(current_led_id, &current_led_duty);
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


