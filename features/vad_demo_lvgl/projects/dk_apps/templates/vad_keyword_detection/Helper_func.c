/**
 ****************************************************************************************
 *
 * @file Helper_func.c
 *
 * @brief Helper functions source file
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
#include "Helper_func.h"

extern OS_TASK template_task_h;
extern OS_TASK audio_task_h;
extern vad_stats_t vad_stats;
__RETAINED uint32_t vad_entry;
__RETAINED bool led_state = false;

const hw_led_config test_led_config = {
        .leds_pwm_start_cycle = {0},
        .leds_pwm_duty_cycle = {10000, 10000, 10000},
        .leds_pwm_frequency = 1000
};

#define CUST_DELAY      (100000) //1.000.000 = 1sec

static void vad_cb(void)
{
        vad_stats.WakeUp++;

        // SET STAND BY
        REG_SETF(VAD, VAD_CTRL3_REG, VAD_SB, 1);        // The WhisperTrigger is deactivated (stand-by mode), avoid another IRQ
        REG_SETF(VAD, VAD_CTRL4_REG, VAD_IRQ_FLAG, 1);  // Writing 1 resets the flag and the IRQ to 0.

        // SET TO SLEEP
        REG_SETF(VAD, VAD_CTRL3_REG, VAD_SLEEP, 1);     // 1: If SB = 0, The WhisperTrigger is in sleep mode
        REG_SETF(VAD, VAD_CTRL3_REG, VAD_SB, 0);        // The WhisperTrigger is activated (sleep or listening mode)

        NVIC_ClearPendingIRQ(VAD_IRQn);
        hw_pdc_acknowledge(vad_entry);

#ifdef ENABLE_DSPOTTER_LIB
        OS_TASK_NOTIFY_FROM_ISR(audio_task_h, AUDIO_START_NOTIF,  OS_NOTIFY_SET_BITS);          // Start the audio
#else
        OS_TASK_NOTIFY_FROM_ISR(template_task_h, VAD_EVT_NOTIF_MASK,  OS_NOTIFY_SET_BITS);      // Enable again the VAD
#endif
}

void re_enable_vad(void)
{
        hw_vad_set_mode(HW_VAD_MODE_ALWAYS_LISTENING);
        while(hw_vad_get_mode() != HW_VAD_MODE_ALWAYS_LISTENING) {__NOP();}
//        printf("\r\n\r\nVAD Ready");
#ifdef ENABLE_MIC
        // After this the system should go to sleep, POWER the mic
        hw_gpio_pad_latch_enable(MIC_PWR_PIN);
        hw_gpio_configure_pin(MIC_PWR_PIN, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true);
        hw_gpio_pad_latch_disable(MIC_PWR_PIN);
#endif
}

void vad_init(void)
{
        hw_vad_set_mode(HW_VAD_MODE_STANDBY);           // make sure settings are not changed during listening mode
        hw_vad_config_t vcfg;

        // Custom VAD settings
        vcfg.mclk = HW_VAD_MCLK_XTAL32K;
        vcfg.mclk_div = HW_VAD_MCLK_DIV_1;
        vcfg.irq_mode = HW_VAD_IRQ_MODE_HIGH;
        vcfg.voice_sens = HW_VAD_VOICE_SENS_DEFAULT;    // Low value may confuse vocal to noise
        vcfg.noise_sens = HW_VAD_NOISE_SENS_FAST_2;    // set the speed of the system adaptation to the ambient noise. Higher value triggers IRQ less frequently
        vcfg.power_sens = HW_VAD_PWR_LVL_SENS_10_DB;    // Not much affects
        vcfg.min_delay = HW_VAD_MIN_DELAY_1536_CYCLES;  // Default, set the minimum time before a detection when switching to Always listening mode. Lower value = more frequent IRQ on vocal or street noise
        vcfg.min_event = HW_VAD_MIN_EVENT_128_CYCLES;   // HW_VAD_MIN_EVENT_32_CYCLES; up to 1024, responds to higher sounds? Time to detect actual vocal and not noise.
        vcfg.nfi_threshold = 0x27;                      // 0x27; default. If set low triggers every time as a sound detector. **************************, value 19 is the trigger


        // Based on VAD MIWOK Benchmark Report
//        vcfg.mclk = HW_VAD_MCLK_XTAL32K;
//        vcfg.mclk_div = HW_VAD_MCLK_DIV_1;
//        vcfg.irq_mode = HW_VAD_IRQ_MODE_HIGH;
//        vcfg.voice_sens = HW_VAD_VOICE_SENS_DEFAULT;    // Low value may confuse vocal to noise
//        vcfg.noise_sens = HW_VAD_NOISE_SENS_DEFAULT;    // set the speed of the system adaptation to the ambient noise. Higher value triggers IRQ less frequently
//        vcfg.power_sens = HW_VAD_PWR_LVL_SENS_3_DB;     // Not much affects
//        vcfg.min_delay = HW_VAD_MIN_DELAY_768_CYCLES;   // Default, set the minimum time before a detection when switching to Always listening mode. Lower value = more frequent IRQ on vocal or street noise
//        vcfg.min_event = HW_VAD_MIN_EVENT_32_CYCLES;   // HW_VAD_MIN_EVENT_32_CYCLES; up to 1024, responds to higher sounds? Time to detect actual vocal and not noise.
//        vcfg.nfi_threshold = 0x27;                      // 0x27; default. If set low triggers every time as a sound detector. **************************, value 19 is the trigger

        hw_vad_configure(&vcfg);
        hw_vad_get_config(&vcfg);

        // Setup the VAD block
        REG_SETF(CRG_TOP, CLK_CTRL_REG, VAD_CLK_SEL, 1);        // Selects the clock for the VAD.
        REG_SETF(VAD, VAD_CTRL3_REG, VAD_SB, 0);                // The VAD block is activated
        REG_SETF(VAD, VAD_CTRL3_REG, VAD_SLEEP, 0);             // If SB = 0, The VAD block is set in LISTENING mode
        REG_SETF(VAD, VAD_CTRL4_REG, VAD_IRQ_FLAG, 1);          // Writing 1 resets the flag and the IRQ to 0.
        NVIC_ClearPendingIRQ(VAD_IRQn);
        NVIC_EnableIRQ(VAD_IRQn);

        vad_entry = hw_pdc_add_entry(HW_PDC_LUT_ENTRY_VAL(
                                HW_PDC_TRIG_SELECT_PERIPHERAL,
                                HW_PDC_PERIPH_TRIG_ID_VAD,
                                HW_PDC_MASTER_CM33,
                                0));

        hw_vad_register_interrupt(vad_cb);

#ifdef ENABLE_MIC        // POWER the mic???
        hw_gpio_pad_latch_enable(MIC_PWR_PIN);
        hw_gpio_configure_pin(MIC_PWR_PIN, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true);
        hw_gpio_pad_latch_disable(MIC_PWR_PIN);
#endif

        printf("\r\nVAD init1\r\n");
}


void vad_power_down(void)
{
        REG_SETF(CRG_TOP, CLK_CTRL_REG, VAD_CLK_SEL, 1);
        REG_SETF(VAD, VAD_CTRL3_REG, VAD_SB, 1); //1: The WhisperTrigger is deactivated (stand-by mode)
}


#ifdef USE_LEDS
void leds_off(void)
{
        hw_led_pwm_on(HW_LED_MSK_LED_1);
        hw_led_pwm_on(HW_LED_MSK_LED_2);
        hw_led_pwm_on(HW_LED_MSK_LED_3);

        hw_led_off(HW_LED_MSK_LED_1);
        hw_led_off(HW_LED_MSK_LED_2);
        hw_led_off(HW_LED_MSK_LED_3);
}

void init_led(void)
{
        hw_led_pwm_duty_cycle_t duty_cycle_led = {0, 100}; // 100%

        hw_led_pwm_off(HW_LED_MSK_LED_1);
        hw_led_pwm_off(HW_LED_MSK_LED_2);
        hw_led_pwm_off(HW_LED_MSK_LED_3);

        hw_led_pwm_set_frequency_hz(1000);
        hw_led_pwm_set_duty_cycle(HW_LED_ID_LED_1, &duty_cycle_led);
        hw_led_pwm_set_duty_cycle(HW_LED_ID_LED_2, &duty_cycle_led);
        hw_led_pwm_set_duty_cycle(HW_LED_ID_LED_3, &duty_cycle_led);

        leds_off();
}

void toggle_db_led(HW_LED_ID led, bool state)
{
        if(state){
                hw_led_on(led);
                hw_led_pwm_on(led);
        }
        else{
                hw_led_off(led);
                hw_led_pwm_off(led);
        }
}

void led_operation (int nRecordFrameCount, int nActiveGroupIndex)
{
        if((nRecordFrameCount % 10 == 0)&&(nActiveGroupIndex == 0)){

                if(led_state){
                        toggle_db_led(HW_LED_MSK_LED_1,true);
                        led_state = false;
                }
                else{
                        toggle_db_led(HW_LED_MSK_LED_1,false);
                        led_state = true;
                }
        }

        if((nRecordFrameCount % 2 == 0)&&(nActiveGroupIndex == 1)){

                if(led_state){
                        toggle_db_led(HW_LED_MSK_LED_1,true);
                        led_state = false;
                }
                else{
                        toggle_db_led(HW_LED_MSK_LED_1,false);
                        led_state = true;
                }
        }
}

void test_leds(void){

        hw_led_init(&test_led_config);

        /* LED1 */

        hw_led_on(HW_LED_MSK_LED_1);
        hw_led_pwm_on(HW_LED_MSK_LED_1);
        hw_clk_delay_usec(CUST_DELAY);
        assert(hw_led_get_states() == 0x1);
        hw_led_off(HW_LED_MSK_LED_1);
        hw_clk_delay_usec(CUST_DELAY);
        assert(hw_led_get_states() == 0x0);

        /* LED2 */

        hw_led_on(HW_LED_MSK_LED_2);
        hw_led_pwm_on(HW_LED_MSK_LED_2);
        hw_clk_delay_usec(CUST_DELAY);
        assert(hw_led_get_states() == 0x2);
        hw_led_off(HW_LED_MSK_LED_2);
        hw_clk_delay_usec(CUST_DELAY);
        assert(hw_led_get_states() == 0x0);

        /* LED3 */

        hw_led_on(HW_LED_MSK_LED_3);
        hw_led_pwm_on(HW_LED_MSK_LED_3);
        hw_clk_delay_usec(CUST_DELAY);
        assert(hw_led_get_states() == 0x4);
        hw_led_off(HW_LED_MSK_LED_3);
        hw_clk_delay_usec(CUST_DELAY);
        assert(hw_led_get_states() == 0x0);

//        /* LED1, LED2, and LED3 */
//
//        hw_led_on(HW_LED_ALL_LED_MASK);
//        hw_led_pwm_on(HW_LED_ALL_LED_MASK);
//        hw_clk_delay_usec(CUST_DELAY/2);
//        assert(hw_led_get_states() == 0x7);
//        hw_led_off(HW_LED_ALL_LED_MASK);
//        hw_clk_delay_usec(CUST_DELAY/2);
//        assert(hw_led_get_states() == 0x0);

}
#endif

void print_stats(void)
{
        printf("\r\n\r\n------------------------------");
        printf("\r\nWake_up: %i", vad_stats.WakeUp);
        printf("\r\nNVD: %i", vad_stats.NDV);
        printf("\r\nVDV: %i", vad_stats.VDV);
#ifdef MEASURE_NFI
        printf("\r\nPeriod: %lu ms", (unsigned long)vad_stats.Period);
#endif
        printf("\r\n------------------------------\r\n");
}

#ifdef dg_configLCD_GUI
extern INITIALISED_PRIVILEGED_DATA OS_TASK gui_task_h;
extern char *ui_txt;
extern bool lights_on;

void Lcd_light_api(bool state)
{
        OS_TASK_NOTIFY(gui_task_h, UI_CHANGE_IMG_EVT, OS_NOTIFY_SET_BITS);
        OS_ENTER_CRITICAL_SECTION();
        lights_on = state;
        OS_LEAVE_CRITICAL_SECTION();
}

void Lcd_text_api(char *text)
{
        OS_TASK_NOTIFY(gui_task_h, UI_CHANGE_TEXT_EVT, OS_NOTIFY_SET_BITS);
        OS_ENTER_CRITICAL_SECTION();
        ui_txt = text;
        OS_LEAVE_CRITICAL_SECTION();
}
#endif //dg_configLCD_GUI



