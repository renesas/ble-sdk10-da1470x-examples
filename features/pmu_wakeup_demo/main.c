/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief Power management and sleep test application
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
#include "hw_led.h"
#if (dg_configGPADC_ADAPTER == 1)
#include "ad_gpadc.h"
#endif

#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"
#include "sys_watchdog.h"
#if (dg_configUSE_HW_GPADC == 1)
#include "hw_gpadc.h"
#endif

#if (dg_configPMU_ADAPTER == 1)
#include "ad_pmu.h"
#else
#include "hw_pmu.h"
#endif

#define NORMAL_WAKEUP_PORT      HW_GPIO_PORT_0
#define NORMAL_WAKEUP_PIN       HW_GPIO_PIN_23 /* J28-pin12 */

#define FAST_WAKEUP_PORT        HW_GPIO_PORT_1
#define FAST_WAKEUP_PIN         HW_GPIO_PIN_0   /* J28-pin16 */

#define WKUP_BUTTON_PORT        HW_GPIO_PORT_1
#define WKUP_BUTTON_PIN         HW_GPIO_PIN_22  /* Button1, S3 */

#define HIBERNATION_BUTTON_PORT HW_GPIO_PORT_0  /* Need to connect DK Button2 to P0_29 pin */
#define HIBERNATION_BUTTON_PIN  HW_GPIO_PIN_29

#define INDICATE_ACTIVE_PORT   HW_GPIO_PORT_0
#define INDICATE_ACTIVE_PIN    HW_GPIO_PIN_30

#define INDICATE_WFI_PORT      HW_GPIO_PORT_0
#define INDICATE_WFI_PIN       HW_GPIO_PIN_31
#define DBG_PIN_HIGH(name)      hw_gpio_configure_pin(name##_PORT, name##_PIN,          \
                                        HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true);  \
                                hw_gpio_pad_latch_enable(name##_PORT, name##_PIN);      \
                                hw_gpio_pad_latch_disable(name##_PORT, name##_PIN)

#define DBG_PIN_LOW(name)       hw_gpio_configure_pin(name##_PORT, name##_PIN,          \
                                        HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false); \
                                hw_gpio_pad_latch_enable(name##_PORT, name##_PIN);      \
                                hw_gpio_pad_latch_disable(name##_PORT, name##_PIN)

#define DBG_PIN_TOGGLE(name)    hw_gpio_configure_pin(name##_PORT, name##_PIN,          \
                                        HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true);  \
                                hw_gpio_pad_latch_enable(name##_PORT, name##_PIN);      \
                                hw_gpio_set_inactive(name##_PORT, name##_PIN);          \
                                hw_gpio_pad_latch_disable(name##_PORT, name##_PIN)

__RETAINED bool fast_wakeup;

#if dg_configUSE_WDOG
__RETAINED_RW int8_t idle_task_wdog_id = -1;
#endif

/*
 * Perform any application specific hardware configuration.  The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware( void );

static void init_wakeup_buttons( void );

/*
 * Task functions.
 */
static void toggle_led_task( void *pvParameters );
static OS_TASK xHandle;
__RETAINED static OS_TASK wkup_mode_switch_task_handle;

#if (dg_configGPADC_ADAPTER == 1)

#define SAMPLE_TIME 15
#define CHOPPING    1
#define OVERSAMPLING 7

const ad_gpadc_driver_conf_t v30_level_driver = {

        .input_attenuator       = HW_GPADC_INPUT_VOLTAGE_UP_TO_3V6,
        .positive               = HW_GPADC_INP_V30,
        .input_mode             = HW_GPADC_INPUT_MODE_SINGLE_ENDED,
        .temp_sensor            = HW_GPADC_NO_TEMP_SENSOR,
        .sample_time            = SAMPLE_TIME,
        .chopping               = CHOPPING,
        .oversampling           = OVERSAMPLING,
};

const ad_gpadc_controller_conf_t V30_LEVEL = {
        HW_GPADC_1,
        NULL,
        &v30_level_driver
};
const ad_gpadc_driver_conf_t vsys_level_driver = {

        .input_attenuator       = HW_GPADC_INPUT_VOLTAGE_UP_TO_0V9,
        .positive               = HW_GPADC_INP_VSYS,
        .input_mode             = HW_GPADC_INPUT_MODE_SINGLE_ENDED,
        .temp_sensor            = HW_GPADC_NO_TEMP_SENSOR,
        .sample_time            = SAMPLE_TIME,
        .chopping               = CHOPPING,
        .oversampling           = OVERSAMPLING,
};

const ad_gpadc_controller_conf_t VSYS_LEVEL = {
        HW_GPADC_1,
        NULL,
        &vsys_level_driver
};

const ad_gpadc_driver_conf_t v18_level_driver = {

        .input_attenuator       = HW_GPADC_INPUT_VOLTAGE_UP_TO_2V7,
        .positive               = HW_GPADC_INP_V18,
        .input_mode             = HW_GPADC_INPUT_MODE_SINGLE_ENDED,
        .temp_sensor            = HW_GPADC_NO_TEMP_SENSOR,
        .sample_time            = SAMPLE_TIME,
        .chopping               = CHOPPING,
        .oversampling           = OVERSAMPLING,
};

const ad_gpadc_controller_conf_t V18_LEVEL = {
        HW_GPADC_1,
        NULL,
        &v18_level_driver
};

const ad_gpadc_driver_conf_t v18f_level_driver = {

        .input_attenuator       = HW_GPADC_INPUT_VOLTAGE_UP_TO_3V6,
        .positive               = HW_GPADC_INP_V18F,
        .input_mode             = HW_GPADC_INPUT_MODE_SINGLE_ENDED,
        .temp_sensor            = HW_GPADC_NO_TEMP_SENSOR,
        .sample_time            = SAMPLE_TIME,
        .chopping               = CHOPPING,
        .oversampling           = OVERSAMPLING,
};

const ad_gpadc_controller_conf_t V18F_LEVEL = {
        HW_GPADC_1,
        NULL,
        &v18f_level_driver
};

const ad_gpadc_driver_conf_t v18p_level_driver = {

        .input_attenuator       = HW_GPADC_INPUT_VOLTAGE_UP_TO_3V6,
        .positive               = HW_GPADC_INP_V18P,
        .input_mode             = HW_GPADC_INPUT_MODE_SINGLE_ENDED,
        .temp_sensor            = HW_GPADC_NO_TEMP_SENSOR,
        .sample_time            = SAMPLE_TIME,
        .chopping               = CHOPPING,
        .oversampling           = OVERSAMPLING,
};

const ad_gpadc_controller_conf_t V18P_LEVEL = {
        HW_GPADC_1,
        NULL,
        &v18p_level_driver
};

const ad_gpadc_driver_conf_t v12_level_driver = {

        .input_attenuator       = HW_GPADC_INPUT_VOLTAGE_UP_TO_3V6,
        .positive               = HW_GPADC_INP_V12,
        .input_mode             = HW_GPADC_INPUT_MODE_SINGLE_ENDED,
        .temp_sensor            = HW_GPADC_NO_TEMP_SENSOR,
        .sample_time            = SAMPLE_TIME,
        .chopping               = CHOPPING,
        .oversampling           = OVERSAMPLING,
};

const ad_gpadc_controller_conf_t V12_LEVEL = {
        HW_GPADC_1,
        NULL,
        &v12_level_driver
};
const ad_gpadc_driver_conf_t v14_level_driver = {

        .input_attenuator       = HW_GPADC_INPUT_VOLTAGE_UP_TO_3V6,
        .positive               = HW_GPADC_INP_V14,
        .input_mode             = HW_GPADC_INPUT_MODE_SINGLE_ENDED,
        .temp_sensor            = HW_GPADC_NO_TEMP_SENSOR,
        .sample_time            = SAMPLE_TIME,
        .chopping               = CHOPPING,
        .oversampling           = OVERSAMPLING,
};

const ad_gpadc_controller_conf_t V14_LEVEL = {
        HW_GPADC_1,
        NULL,
        &v14_level_driver
};

static void get_rails_voltage(void)
{
        uint16_t mVolt = 0;
        ad_gpadc_handle_t handle;

        handle = ad_gpadc_open(&V30_LEVEL);
        hw_gpadc_adc_measure();
        mVolt = hw_gpadc_get_voltage();
        ad_gpadc_close(handle, false);
        printf(" V30:%dmV ", mVolt);

        handle = ad_gpadc_open(&VSYS_LEVEL);
        hw_gpadc_adc_measure();
        mVolt = hw_gpadc_get_voltage();
        ad_gpadc_close(handle, false);
        printf("VSYS:%dmV ", mVolt);

        handle = ad_gpadc_open(&V18_LEVEL);
        hw_gpadc_adc_measure();
        mVolt = hw_gpadc_get_voltage();
        ad_gpadc_close(handle, false);
        printf("V18:%dmV ", mVolt);

        handle = ad_gpadc_open(&V18F_LEVEL);
        hw_gpadc_adc_measure();
        mVolt = hw_gpadc_get_voltage();
        ad_gpadc_close(handle, false);
        printf("V18F:%dmV ", mVolt);

        handle = ad_gpadc_open(&V18P_LEVEL);
        hw_gpadc_adc_measure();
        mVolt = hw_gpadc_get_voltage();
        ad_gpadc_close(handle, false);
        printf("V18P:%dmV ", mVolt);

        handle = ad_gpadc_open(&V14_LEVEL);
        hw_gpadc_adc_measure();
        mVolt = hw_gpadc_get_voltage();
        ad_gpadc_close(handle, false);
        printf("V14:%dmV ", mVolt);

        handle = ad_gpadc_open(&V12_LEVEL);
        hw_gpadc_adc_measure();
        mVolt = hw_gpadc_get_voltage();
        ad_gpadc_close(handle, false);
        printf("V12:%dmV", mVolt);
}
#endif

static void print_system_configuration(void)
{
        // Print wakeup mode
        printf("Wakeup mode:");
        if (pm_get_sys_wakeup_mode() == pm_sys_wakeup_mode_fast) {
                printf("fast  ");
        }
        else {
                printf("normal");
        }

        // Print clock configuration
        printf(" System clock:");
        switch (cm_sys_clk_get()) {
        case sysclk_RCHS_32:
                printf("RCHS@32MHz");
                break;
        case sysclk_XTAL32M:
                printf("XTAL32M   ");
                break;
        case sysclk_RCHS_64:
                printf("RCHS@64MHz");
                break;
        case sysclk_RCHS_96:
                printf("RCHS@96MHz");
                break;
        case sysclk_PLL160:
                printf("PLL160    ");
                break;
        }

        // Print the V12 rail configuration
        printf(" V12 active:");
        HW_PMU_1V2_RAIL_CONFIG rail_config;
        hw_pmu_get_1v2_onwakeup_config(&rail_config);
        if (rail_config.voltage == HW_PMU_1V2_VOLTAGE_0V90) {
                printf("0.90V");
        }
        else {
                printf("1.20V");
        }

        printf(" V12 sleep:");
        hw_pmu_get_1v2_onsleep_config(&rail_config);
        if (rail_config.voltage == HW_PMU_1V2_VOLTAGE_SLEEP_0V90) {
                printf("0.90V");
        }
        else if (rail_config.voltage == HW_PMU_1V2_VOLTAGE_SLEEP_1V20) {
                if (cm_sys_clk_get() == sysclk_PLL160) {
                        // Special case when system clock is PLL160
                        // System wakes up @ 32MHz and then switches to PLL160
                        if (pm_get_sys_wakeup_mode() == pm_sys_wakeup_mode_fast) {
                                // In fast wakeup mode the V12 sleep voltage is 0.9V
                                printf("0.90V");
                        }
                }
                else {
                        printf("1.20V");
                }
        }
#if (dg_configGPADC_ADAPTER == 1)
        printf(" GPADC:");
        get_rails_voltage();
#endif
        printf("\n\r");

}

/* This is the function of WkupSwitch task. It waits for a notification to switch between normal and fast
 * wakeup mode. This notification is triggered by pressing Button1 (S3).
 */
static void wkup_mode_switch_task()
{
        uint32_t ulNotifiedValue;
        OS_BASE_TYPE xResult;
        bool prev_fast_wakeup = false;

#if (dg_configPMU_ADAPTER == 0)
        HW_PMU_1V2_RAIL_CONFIG rail_config;
        /* Configure V12 rail to be enabled in sleep and wakeup mode. V12 must be always on, otherwise the
         * system will be turned off. The voltage on wakeup is set according to the wakeup mode (normal or fast).
         * The current in active and sleep mode is set to 150mA.
         */

        hw_pmu_1v2_set_voltage(HW_PMU_1V2_VOLTAGE_0V90);
        hw_pmu_1v2_onwakeup_enable(HW_PMU_1V2_MAX_LOAD_150);
        hw_pmu_1v2_onsleep_enable(HW_PMU_1V2_MAX_LOAD_150);
#endif

        uint8_t mode = 0;
        while (true) {
                xResult = OS_TASK_NOTIFY_WAIT(0x0, OS_TASK_NOTIFY_ALL_BITS, &ulNotifiedValue, OS_TASK_NOTIFY_FOREVER);

                if (xResult == OS_TASK_NOTIFY_SUCCESS) {
                        switch (mode) {
                        case 0:
                                // Change system clock to RCHS@64MHz
                                cm_sys_clk_set(sysclk_RCHS_64);
                                break;
                        case 1:
                                // Change system clock to RCHS@32MHz
                                cm_sys_clk_set(sysclk_RCHS_32);
                                break;
                        case 2:
                                // Change system clock to RCHS@96MHz
                                cm_sys_clk_set(sysclk_RCHS_96);
                                break;
                        case 3:
                                // Change system clock to RCHS@32MHz
                                cm_sys_clk_set(sysclk_RCHS_32);
                                break;
                        case 4:
                                // Change system clock to PLL@160MHz
                                cm_sys_clk_set(sysclk_PLL160);
                                break;
                        case 5:
                                // Change system clock to RCHS@32MHz
                                cm_sys_clk_set(sysclk_RCHS_32);
                                break;
                        case 6:
                                // Set wakeup mode to fast wakeup
                                fast_wakeup = true;
                                break;
                        case 7:
                                // Change system clock to RCHS@64MHz
                                cm_sys_clk_set(sysclk_RCHS_64);
                                break;
                        case 8:
                                // Change system clock to RCHS@32MHz
                                cm_sys_clk_set(sysclk_RCHS_32);
                                break;
                        case 9:
                                // Change system clock to RCHS@96MHz
                                cm_sys_clk_set(sysclk_RCHS_96);
                                break;
                        case 10:
                                // Change system clock to RCHS@32MHz
                                cm_sys_clk_set(sysclk_RCHS_32);
                                break;
                        case 11:
                                // Change system clock to PLL@160MHz
                                cm_sys_clk_set(sysclk_PLL160);
                                break;
                        case 12:
                                // Change system clock to RCHS@32MHz
                                cm_sys_clk_set(sysclk_RCHS_32);
                                break;
                        case 13:
                                // Set wakeup mode to normal wakeup
                                fast_wakeup = false;
                                break;
                        }
                        mode++;
                        if (mode > 13) {
                                mode = 0;
                        }

#if (dg_configPMU_ADAPTER == 0)
                        /* Get the current active voltage. This voltage changes according to the system clock. */
                        hw_pmu_get_1v2_onwakeup_config(&rail_config);
                        hw_pmu_1v2_set_voltage(rail_config.voltage);
#endif

                        if (prev_fast_wakeup != fast_wakeup) {
                                prev_fast_wakeup = fast_wakeup;
                                if (fast_wakeup) {
                                        /*
                                         * In fast wakeup mode the HW FSM lets ARM to start executing code immediately using the
                                         * RCHS clock, before V12 voltage has settled to the active level.
                                         * V12 voltage must be 0.9V when the digital logic operates at a frequency up to 50MHz.
                                         * V12 voltage must be 1.2V when the digital logic operates at a frequency higher than 50MHz.
                                         * So, the sleep voltage of V12 must be set accordingly so ARM can work as expected.
                                         * V12 sleep voltage changed automatically to proper level before changing the wakeup mode.
                                         * Responsible for that is the PMU adapter.
                                         */
#if (dg_configPMU_ADAPTER == 0)
                                        if (hw_clk_get_rchs_mode() == RCHS_32) {
                                                /* RCHS frequency is 32MHz.
                                                 * Configure V12's sleep voltage to 0.9V before switching to fast wakeup.
                                                 */
                                                hw_pmu_1v2_set_voltage(HW_PMU_1V2_VOLTAGE_SLEEP_0V90);
                                        } else {
                                                /* RCHS frequency is 64 or 96MHz.
                                                 * Configure V12's sleep voltage to 1.2V before switching to fast wakeup.
                                                 */
                                                hw_pmu_1v2_set_voltage(HW_PMU_1V2_VOLTAGE_SLEEP_1V20);
                                        }
#endif
                                        /* Set wakeup mode to fast wakeup */
                                        pm_set_sys_wakeup_mode(pm_sys_wakeup_mode_fast);
                                }
                                else {
                                        /* Set wakeup mode to normal wakeup */
                                        pm_set_sys_wakeup_mode(pm_sys_wakeup_mode_normal);

                                        /* In normal wakeup mode the HW FSM let ARM to start executing code when V12 has
                                         * settled to the active level. So it is secure to set the sleep voltage of V12
                                         * to 0.9V, to lower the power consumption during sleep.
                                         * Wakeup mode should first be set to normal and then the sleep voltage can be set to 0.9V.
                                         * The regulation of the voltage levels is done automatically by the PMU adapter
                                         * It is not possible to change the sleep voltage first, because the device is in fast mode.
                                         */

#if (dg_configPMU_ADAPTER == 0)
                hw_pmu_1v2_set_voltage(HW_PMU_1V2_VOLTAGE_SLEEP_0V90);
#endif
                                }

                        }

                        print_system_configuration();
                }
        }
}

static void system_init( void *pvParameters )
{

#if defined CONFIG_RETARGET
        extern void retarget_init(void);
#endif

        /* Set system clock to RCHS@32 MHz */
        cm_sys_clk_init(sysclk_RCHS_32);

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

        init_wakeup_buttons();

#if defined CONFIG_RETARGET
        retarget_init();
#endif
        /* Set false to wakeup faster. System is not waiting for XTAL32M to settle. */
        /* Code starts to run immediately. */
        pm_set_wakeup_mode(true);

        /* Set the desired sleep mode. */
        pm_sleep_mode_set(pm_mode_extended_sleep);

        /* This flag is used in order to switch between fast wakeup and ultra fast wakeup mode */
        fast_wakeup = false;

        /* Set the desired wakeup mode. */
        pm_set_sys_wakeup_mode(pm_sys_wakeup_mode_normal);

#if dg_configSYSTEMVIEW
        SEGGER_SYSVIEW_Conf();
#endif

        OS_TASK handle;

        /* Create a task, which switches a LED on and off every second. */
        /* When using FPGA it will only toggle a GPIO (no LEDs are available), for silicon it will switch on/off a LED. */

        OS_TASK_CREATE("ToggleLED",                      /* The text name assigned to the task, for
                                                            debug only; not used by the kernel. */
                       toggle_led_task,                  /* The function that implements the task. */
                       NULL,                             /* The parameter passed to the task. */
                       1000 * OS_STACK_WORD_SIZE,        /* The number of bytes to allocate to the
                                                            stack of the task. */
                       OS_TASK_PRIORITY_NORMAL,          /* The priority assigned to the task. */
                       handle );                         /* The task handle */

        OS_ASSERT(handle);

        /* Create a task, which will switch between normal and fast wkup mode by a button press. */

        OS_TASK_CREATE("WkupSwitch",                     /* The text name assigned to the task, for
                                                            debug only; not used by the kernel. */
                       wkup_mode_switch_task,            /* The function that implements the task. */
                       NULL,                             /* The parameter passed to the task. */
                       1000 * OS_STACK_WORD_SIZE,        /* The number of bytes to allocate to the
                                                            stack of the task. */
                       OS_TASK_PRIORITY_NORMAL,          /* The priority assigned to the task. */
                       wkup_mode_switch_task_handle );   /* The task handle */

        OS_ASSERT(wkup_mode_switch_task_handle);

        /* the work of the SysInit task is done */
        OS_TASK_DELETE( xHandle );
}

static void toggle_led_task( void *pvParameters )
{
        bool switch_on = false;
        hw_led_pwm_duty_cycle_t duty_cycle_led1 = {0, 0}; // 100%

        hw_led_pwm_set_frequency_hz(0xFFF);
        hw_led_pwm_set_duty_cycle(HW_LED_ID_LED_1, &duty_cycle_led1);
        hw_led_pwm_on(HW_LED_MSK_LED_1);
        // Set LED led to 2.5mA
        hw_led_pwm_set_load_sel(HW_LED_ID_LED_1, 0);
#if (dg_configPMU_ADAPTER == 1)
        /* Configure VLED rail with voltage 4.5V and current on sleep 300uA and on wakeup 150mA. */
        ad_pmu_rail_config_t rail_config_vled;
        rail_config_vled.rail_vled.voltage_common = HW_PMU_VLED_VOLTAGE_4V5;
        rail_config_vled.rail_vled.current_onsleep = HW_PMU_VLED_MAX_LOAD_0_300;
        rail_config_vled.rail_vled.current_onwakeup = HW_PMU_VLED_MAX_LOAD_150;
#endif
        while (true) {
                OS_DELAY_MS(1000);

                if (switch_on == false) {
#if (dg_configPMU_ADAPTER == 1)

                        /* To switch on the LED, VLED is needed. But VLED can be handled by application SW
                         * ONLY on active mode. When it is dropped to sleep mode it will duty cycle automatically
                         * with 300uA current. This not enough to switch the LED on, therefore a request to
                         * switch to active mode is needed first.
                         */
                        pm_sleep_mode_request(pm_mode_idle);

                        /* Enable VLED rail before switching on the LED. */
                        rail_config_vled.enabled_onwakeup = true;
                        rail_config_vled.enabled_onsleep = true;

                        ad_pmu_configure_rail(PMU_RAIL_VLED, &rail_config_vled);

#endif
                        switch_on = true;

                        hw_led_on(HW_LED_MSK_LED_1);
              }
              else {
                      hw_led_off(HW_LED_MSK_LED_1);

#if (dg_configPMU_ADAPTER == 1)
                        /* Disable VLED rail after switching off the LED */
                        rail_config_vled.enabled_onwakeup = false;
                        rail_config_vled.enabled_onsleep = false;

                        ad_pmu_configure_rail(PMU_RAIL_VLED, &rail_config_vled);

                        // Run in a loop for a while to measure M33 active current
                        DBG_PIN_HIGH(INDICATE_ACTIVE);
                        hw_clk_delay_usec(10000);
                        DBG_PIN_LOW(INDICATE_ACTIVE);

                        // Stay idle for a while to measure M33 WFI current
                        DBG_PIN_HIGH(INDICATE_WFI);
                        OS_DELAY_MS(10);
                        DBG_PIN_LOW(INDICATE_WFI);

                        /* After switching off the LED, VLED is not needed anymore so it can switch
                         * back to extended sleep mode.
                         */
                        pm_sleep_mode_release(pm_mode_idle);
#endif
                        switch_on = false;

              }
        }
}

int main( void )
{
        OS_BASE_TYPE status;

        status = OS_TASK_CREATE("SysInit",              /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                        system_init,                    /* The System Initialization task. */
                        ( void * ) 0,                   /* The parameter passed to the task. */
                        1000 * OS_STACK_WORD_SIZE,
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

static void set_system_to_hibernation( void )
{
       /* Configure Button3 S5 (P0_29) to wakeup system from hibernation */
       hw_wkup_configure_hibernation(HW_WKUP_HIBERN_TRIGGER_P0_29, HW_WKUP_HIBERN_PD_EN_PIN_P0_29);

       /* Put system in hibernation */
       pm_sleep_mode_set(pm_mode_hibernation);
}

static void wakeup_handler( void )
{
        bool wkup_switch_enable, hibernation_enable;

        hw_wkup_reset_key_interrupt();

        /* Check if Button1 or Button3 was pressed and call the corresponding functionality */
        wkup_switch_enable = hw_gpio_get_pin_status(WKUP_BUTTON_PORT, WKUP_BUTTON_PIN) == false;
        hibernation_enable = hw_gpio_get_pin_status(HIBERNATION_BUTTON_PORT, HIBERNATION_BUTTON_PIN) == false;
        /* If Button1 is pressed, a notification will be send to the task that switches between normal
         * and fast wakeup mode.
         */
        if (wkup_switch_enable) {
                /* Toggle a GPIO to allow the wakeup time to be measured. */
                if (fast_wakeup) {
                        DBG_PIN_TOGGLE(FAST_WAKEUP);
                } else {
                        DBG_PIN_TOGGLE(NORMAL_WAKEUP);
                }
                OS_TASK_NOTIFY_FROM_ISR(wkup_mode_switch_task_handle, 1, OS_NOTIFY_VAL_WITH_OVERWRITE);
                //
        }

        /* If Button3 is pressed the system will enter hibernation mode and remains in this state
         * until the Button is released.
         */
        else if (hibernation_enable) {
                set_system_to_hibernation();
        }
}

static void init_wakeup_buttons( void )
{
        hw_wkup_init(NULL);

        /* Configure Button1 (S3) to switch between normal and fast wakeup modes. */
        hw_wkup_set_trigger(WKUP_BUTTON_PORT, WKUP_BUTTON_PIN, HW_WKUP_TRIG_LEVEL_LO_DEB);

        /* Configure Button3 (S5) to put the system in hibernation mode. */
        hw_wkup_set_trigger(HIBERNATION_BUTTON_PORT, HIBERNATION_BUTTON_PIN, HW_WKUP_TRIG_LEVEL_LO_DEB);
        hw_wkup_set_key_debounce_time(20);

        hw_wkup_register_key_interrupt(wakeup_handler, 1);
        hw_wkup_enable_key_irq();
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
        /* Initialize hardware */
        pm_system_init(periph_init);

        /* At this point the PMU adapter initialization is done. With the code below the default configuration
         * of V18 rail will be overwritten. In this example V18 rail's voltage is set to 1.2V and will be on
         * in active mode.
         */

#if (dg_configPMU_ADAPTER == 1)
        /* In order to configure correct a rail, it is needed to define if it is enabled/disabled in sleep and
         * active mode. Then the values of the voltage and current in the enabled mode should be configured.
         */

        ad_pmu_rail_config_t rail_config_v18;


        /* Configure rail V18 to be enabled in active mode and disabled in sleep mode.
         * It's voltage will be 1.2V in active mode and the current will be 100mA. */
        rail_config_v18.enabled_onwakeup = true;
        rail_config_v18.enabled_onsleep  = false;
        rail_config_v18.rail_1v8.voltage_common   = HW_PMU_1V8_VOLTAGE_1V2;
        rail_config_v18.rail_1v8.current_onwakeup = HW_PMU_1V8_MAX_LOAD_100;

        /* Apply V18's configuration using the PMU adapter. */
        REG_SETF(CRG_TOP, POR_CTRL_REG, POR_VSYS_DISABLE, 1);
        ad_pmu_configure_rail(PMU_RAIL_1V8, &rail_config_v18);
        REG_SETF(CRG_TOP, POR_CTRL_REG, POR_VSYS_DISABLE, 0);
#else
        hw_pmu_1v8_set_voltage(HW_PMU_1V8_VOLTAGE_1V2);
        hw_pmu_1v8_onwakeup_enable(HW_PMU_1V8_MAX_LOAD_100);
#endif //dg_configPMU_ADAPTER

        /* Configure and initialize button GPIOs */
        hw_gpio_set_pin_function(WKUP_BUTTON_PORT, WKUP_BUTTON_PIN, HW_GPIO_MODE_INPUT_PULLUP,
                        HW_GPIO_FUNC_GPIO);
        hw_gpio_set_pin_function(HIBERNATION_BUTTON_PORT, HIBERNATION_BUTTON_PIN, HW_GPIO_MODE_INPUT_PULLUP,
                        HW_GPIO_FUNC_GPIO);

        hw_gpio_pad_latch_enable(WKUP_BUTTON_PORT, WKUP_BUTTON_PIN);
        hw_gpio_pad_latch_disable(WKUP_BUTTON_PORT, WKUP_BUTTON_PIN);
        hw_gpio_pad_latch_enable(HIBERNATION_BUTTON_PORT, HIBERNATION_BUTTON_PIN);
        hw_gpio_pad_latch_disable(HIBERNATION_BUTTON_PORT, HIBERNATION_BUTTON_PIN);
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
