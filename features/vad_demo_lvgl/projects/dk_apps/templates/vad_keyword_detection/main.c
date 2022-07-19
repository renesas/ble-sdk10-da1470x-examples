/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief main implementation for voice activity detector example .
 *
 * Copyright (c) 2022 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor. By using this Software
 * you agree that Dialog Semiconductor retains all intellectual property and proprietary
 * rights in and to this Software and any use, reproduction, disclosure or distribution
 * of the Software without express written permission or a license agreement from Dialog
 * Semiconductor is strictly prohibited. This Software is solely for use on or in
 * conjunction with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR AS
 * REQUIRED BY LAW, THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE PROVIDED
 * IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR BY LAW, IN NO EVENT SHALL DIALOG
 * SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE.
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
#include "sys_tcs_da1470x.h"
#include "sys_tcs.h"
#ifdef dg_configLCD_GUI
#include "ad_pmu.h"
#endif

#include "periph_setup.h"

#ifdef USE_K1
#include "hw_wkup_v2.h"
__RETAINED bool button_state = false;
#endif

#include "Helper_func.h"


/* Task priorities */
#define mainTEMPLATE_TASK_PRIORITY              ( OS_TASK_PRIORITY_NORMAL )

/* The rate at which data is template task counter is incremented. */
#define mainCOUNTER_FREQUENCY_MS                OS_MS_2_TICKS(200)

/*
 * Perform any application specific hardware configuration.  The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware( void );

static OS_TASK xHandle;
OS_TASK template_task_h;
OS_TASK audio_task_h = NULL;
#ifdef MEASURE_NFI
extern vad_stats_t vad_stats;
#endif

/*
 * Task functions.
 */
static OS_TASK_FUNCTION(prvTemplateTask, pvParameters);
void VadKeywordDetection_task(void *pvParameters);


#ifdef USE_K1
void tst_pm_button1_trigger_cb(void)
{
        OS_TASK_NOTIFY_FROM_ISR(template_task_h, BUTT_EVT_NOTIF_MASK,  OS_NOTIFY_SET_BITS);
        hw_wkup_reset_key_interrupt();
}
#endif

#ifdef dg_configLCD_GUI
INITIALISED_PRIVILEGED_DATA OS_TASK gui_task_h = NULL;
OS_TASK_FUNCTION(GUITask, pvParameters);
__RETAINED static OS_TIMER clear_lcd_timer;

static void clear_lcd_timer_cb(OS_TIMER pxTime)
{
        OS_TIMER_STOP(clear_lcd_timer, OS_TIMER_FOREVER);
        Lcd_text_api("");
}
#endif

#ifdef USE_LEDS
__RETAINED static OS_TIMER led_timer;

static void led_timer_cb(OS_TIMER pxTime)
{
        OS_TIMER_STOP(led_timer, OS_TIMER_FOREVER);
        leds_off();
        re_enable_vad();
}
#endif

#ifdef MEASURE_NFI
#define NFI_PERIOD      250
uint8_t nfi_val;
__RETAINED static OS_TIMER nfi_timer;
static void nfi_timer_cb(OS_TIMER pxTime)
{
        OS_TASK_NOTIFY_FROM_ISR(template_task_h, NFI_TIMEOUT_NOTIF,  OS_NOTIFY_SET_BITS);
}
#endif

static OS_TASK_FUNCTION(system_init, pvParameters)
{
        sys_clk_t sys_clk_prio[5] = {
                sysclk_PLL160,
                sysclk_XTAL32M,
                sysclk_RCHS_96,
                sysclk_RCHS_32,
                sysclk_RCHS_64,
        };

        cm_sys_clk_set_priority(sys_clk_prio);

//        cm_sys_clk_init(sysclk_XTAL32M);
        cm_sys_clk_init(VAD_SYSTEM_CLK);

//MUST change the LCD frequency to work with high clock settings 160MHz
        // ui/dgi/inc/e120a390qsr.h (.hw_init.write_freq = LCDC_FREQ_40MHz,)

        cm_apb_set_clock_divider(apb_div1);
        cm_ahb_set_clock_divider(ahb_div1);
        cm_lp_clk_init();

        /* Prepare the hardware to run this demo. */
        prvSetupHardware();

        /* Set the desired sleep mode. */
        pm_sleep_mode_set(pm_mode_extended_sleep);

        /* Set the desired wakeup mode. */
        pm_set_sys_wakeup_mode(pm_sys_wakeup_mode_normal);


#ifdef USE_K1
        hw_wkup_init(NULL);
        /* A wake-up trigger correlated to the KEY1 button firing event (when pressed) at low level. */
        hw_wkup_set_trigger(KEY1_PORT, KEY1_PIN, HW_WKUP_TRIG_LEVEL_LO_DEB);
        hw_wkup_set_key_debounce_time(10);
        hw_wkup_unregister_interrupts();
        hw_wkup_register_key_interrupt(tst_pm_button1_trigger_cb, 1);
        hw_wkup_enable_key_irq();
        printf("\r\nK1 init\r\n");
//        (void)test_pdc_entry;
#endif

#ifdef ENABLE_DSPOTTER_LIB
        /* Start main task here (text menu available via UART1 to control application) */
        OS_TASK_CREATE( "DSpotter",            /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                        //prvTemplateTask,                /* The function that implements the task. */
                        VadKeywordDetection_task,
                        NULL,                           /* The parameter passed to the task. */
                        //OS_MINIMAL_TASK_STACK_SIZE,
                        2048,
                                                        /* The number of bytes to allocate to the
                                                           stack of the task. */
                        _OS_TASK_PRIORITY_HIGHEST,     /* The priority assigned to the task. */
                        audio_task_h );                       /* The task handle */
        OS_ASSERT(audio_task_h);
#endif


        /* Start main task here (text menu available via UART1 to control application) */
        OS_TASK_CREATE( "Template",            /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                        prvTemplateTask,                /* The function that implements the task. */
                        NULL,                           /* The parameter passed to the task. */
                        OS_MINIMAL_TASK_STACK_SIZE*2,
                                                        /* The number of bytes to allocate to the
                                                           stack of the task. */
                        mainTEMPLATE_TASK_PRIORITY,     /* The priority assigned to the task. */
                        template_task_h );                       /* The task handle */
        OS_ASSERT(template_task_h);

#ifdef dg_configLCD_GUI
        printf("\r\nInitializing GUI task...\r\n");
        OS_TASK_CREATE("GUI Task",      /* The text name assigned to the task, for
                                           debug only; not used by the kernel. */
                       GUITask,         /* The function that implements the task. */
                       NULL,            /* The parameter passed to the task. */
                       OS_MINIMAL_TASK_STACK_SIZE*5,
                                       /* The number of bytes to allocate to the
                                          stack of the task. */
                       OS_TASK_PRIORITY_NORMAL,         /* The priority assigned to the task. */
                       gui_task_h);     /* The task handle */
        OS_ASSERT(gui_task_h);
#endif /* dg_configLCD_GUI */

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
static OS_TASK_FUNCTION(prvTemplateTask, pvParameters)
{
//        OS_TICK_TIME xNextWakeTime;
//        static uint32_t test_counter=0;

        uint32_t *chip_id_value = NULL;
        uint32_t *prod_info_value = NULL;
        uint8_t size = 0;
        sys_tcs_get_custom_values(SYS_TCS_GROUP_CHIP_ID, &chip_id_value, &size);
        sys_tcs_get_custom_values(SYS_TCS_GROUP_PROD_INFO, &prod_info_value, &size);

#ifdef VAD_PERIPH
        // Enable VAD
        vad_init();
#else
        vad_init();
        hw_vad_set_mode(HW_VAD_MODE_STANDBY);
#endif

#ifdef USE_LEDS
        test_leds();
        init_led();
        led_timer = OS_TIMER_CREATE("LedTm", OS_MS_2_TICKS(1000), OS_TIMER_FAIL, NULL, led_timer_cb);
#endif
#ifdef dg_configLCD_GUI
        clear_lcd_timer = OS_TIMER_CREATE("Clear_lcd_timer", OS_MS_2_TICKS(500), OS_TIMER_FAIL, NULL, clear_lcd_timer_cb);
#endif

#ifdef MEASURE_NFI
        nfi_timer = OS_TIMER_CREATE("NfiTm", OS_MS_2_TICKS(NFI_PERIOD), OS_TIMER_RELOAD, NULL, nfi_timer_cb);
        OS_TIMER_START(nfi_timer, OS_TIMER_FOREVER);
#endif

        /* Initialise xNextWakeTime - this only needs to be done once. */
//        xNextWakeTime = OS_GET_TICK_COUNT();

        for ( ;; ) {
                OS_BASE_TYPE ret __UNUSED;
                uint32_t notif;

                /*
                 * Wait on any of the notification bits, then clear them all
                 */
                ret = OS_TASK_NOTIFY_WAIT(OS_TASK_NOTIFY_NONE, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);
                /* Blocks forever waiting for the task notification. Therefore, the return value must
                 * always be OS_OK
                 */
                OS_ASSERT(ret == OS_OK);
#ifdef USE_K1
                if (notif & BUTT_EVT_NOTIF_MASK) {
                        printf("\r\nBTN");
#ifdef ENABLE_DSPOTTER_LIB
                        if(!button_state){
                                OS_TASK_NOTIFY(audio_task_h, AUDIO_STOP_NOTIF,  OS_NOTIFY_SET_BITS);
                                button_state = true;
                        }
                        else{
                                OS_TASK_NOTIFY(audio_task_h, AUDIO_START_NOTIF,  OS_NOTIFY_SET_BITS);
                                button_state = false;
                        }
#endif
                }
#endif

#ifdef VAD_PERIPH
                if(notif & VAD_EVT_NOTIF_MASK){
                        printf("\r\n-VAD IRQ");
                        re_enable_vad();
                }

#endif

#ifdef USE_LEDS
                if(notif & LED_TIMEOUT_NOTIF){
                        OS_TIMER_START(led_timer, OS_TIMER_FOREVER);
                }
#endif

#ifdef dg_configLCD_GUI
                if(notif & LCD_TIMEOUT_NOTIF){
                        OS_TIMER_START(clear_lcd_timer, OS_TIMER_FOREVER);
                }
#endif

#ifdef MEASURE_NFI
                if(notif & NFI_TIMEOUT_NOTIF){
                        nfi_val = VAD->VAD_STATUS_REG;
                        printf(" %i,", nfi_val);
                        vad_stats.Period +=  NFI_PERIOD;
                        if(vad_stats.Period >= 60000){
                                print_stats();
                                memset(&vad_stats, 0, sizeof(vad_stats));
                                __NOP();
                        }
                }
#endif
        }
}

/**
 * @brief Initialize the peripherals domain after power-up.
 *
 */
static void periph_init(void)
{
//#ifdef VAD_PERIPH
#ifdef ENABLE_MIC
        hw_gpio_pad_latch_enable(MIC_PWR_PIN);
        hw_gpio_configure_pin(MIC_PWR_PIN, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true);
        hw_gpio_configure_pin_power(MIC_PWR_PIN, HW_GPIO_POWER_VDD1V8P);
        hw_gpio_pad_latch_disable(MIC_PWR_PIN);
#else
        hw_gpio_pad_latch_enable(MIC_PWR_PIN);
        hw_gpio_configure_pin(MIC_PWR_PIN, HW_GPIO_MODE_INPUT_PULLDOWN, HW_GPIO_FUNC_GPIO, false);
        hw_gpio_pad_latch_disable(MIC_PWR_PIN);
#endif
//#endif

#ifdef USE_MARKER_PIN
        hw_gpio_pad_latch_enable(MARKER_PIN0);
        hw_gpio_configure_pin(MARKER_PIN0, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false);
        hw_gpio_pad_latch_disable(MARKER_PIN0);

        hw_gpio_pad_latch_enable(MARKER_PIN1);
        hw_gpio_configure_pin(MARKER_PIN1, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false);
        hw_gpio_pad_latch_disable(MARKER_PIN1);

        hw_gpio_pad_latch_enable(MARKER_PIN2);
        hw_gpio_configure_pin(MARKER_PIN2, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false);
        hw_gpio_pad_latch_disable(MARKER_PIN2);
#endif

        // GPIO pins for ADC are fixed. P1_5 and P1_6 must be used.
        hw_gpio_pad_latch_enable(HW_GPIO_PORT_1, HW_GPIO_PIN_5);
        hw_gpio_pad_latch_enable(HW_GPIO_PORT_1, HW_GPIO_PIN_6);
        hw_gpio_set_pin_function(HW_GPIO_PORT_1, HW_GPIO_PIN_5,  HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_ADC);
        hw_gpio_set_pin_function(HW_GPIO_PORT_1, HW_GPIO_PIN_6,  HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_ADC);
        hw_gpio_pad_latch_disable(HW_GPIO_PORT_1, HW_GPIO_PIN_5);
        hw_gpio_pad_latch_disable(HW_GPIO_PORT_1, HW_GPIO_PIN_6);

#ifdef USE_K1
        hw_gpio_pad_latch_enable(BUTTON_PIN);
        hw_gpio_set_pin_function(BUTTON_PIN,     HW_GPIO_MODE_INPUT_PULLUP,  HW_GPIO_FUNC_GPIO);
        hw_gpio_pad_latch_disable(BUTTON_PIN);
#endif

#if defined SUPPORT_UART_DUMP_RECORD
        hw_gpio_pad_latch_enable(AUDIO_UART_TX_PIN);
        hw_gpio_set_pin_function(AUDIO_UART_TX_PIN,  HW_GPIO_MODE_OUTPUT, AUDIO_UART_PIN_FUNC);
        hw_gpio_pad_latch_disable(AUDIO_UART_TX_PIN);
#endif

#ifndef USE_LEDS
        hw_led_pwm_off(HW_LED_MSK_LED_1);
        hw_led_pwm_off(HW_LED_MSK_LED_2);
        hw_led_pwm_off(HW_LED_MSK_LED_3);

        hw_led_off(HW_LED_MSK_LED_1);
        hw_led_off(HW_LED_MSK_LED_2);
        hw_led_off(HW_LED_MSK_LED_3);
#endif
}

/**
 * @brief Hardware Initialization
 */
static void prvSetupHardware( void )
{

        /* Init hardware */
        pm_system_init(periph_init);

#ifdef dg_configLCD_GUI
        /*
         * 1V8 rail configuration
         */
        ad_pmu_rail_config_t v18_rail_config;

        v18_rail_config.enabled_onwakeup = true;
        v18_rail_config.enabled_onsleep = true;
        v18_rail_config.rail_1v8.current_onsleep = HW_PMU_1V8_MAX_LOAD_100;
        v18_rail_config.rail_1v8.current_onwakeup = HW_PMU_1V8_MAX_LOAD_100;
        v18_rail_config.rail_1v8.voltage_common = HW_PMU_1V8_VOLTAGE_1V8;

        ad_pmu_configure_rail(PMU_RAIL_1V8, &v18_rail_config);

        /*
         * 3V0 rail configuration
         */
        ad_pmu_rail_config_t v30_rail_config;

        v30_rail_config.enabled_onwakeup = true;
        v30_rail_config.enabled_onsleep = true;
        v30_rail_config.rail_3v0.voltage_onwakeup = HW_PMU_3V0_VOLTAGE_3V0;
        v30_rail_config.rail_3v0.voltage_onsleep = HW_PMU_3V0_VOLTAGE_SLEEP_3V0;
        v30_rail_config.rail_3v0.current_onwakeup = HW_PMU_3V0_MAX_LOAD_160;
        v30_rail_config.rail_3v0.current_onsleep = HW_PMU_3V0_MAX_LOAD_160;

        ad_pmu_configure_rail(PMU_RAIL_3V0, &v30_rail_config);

#if dg_configLCDC_ADAPTER
#if dg_configUSE_LPM012M134B
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
#endif /* dg_configLCD_GUI */
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
