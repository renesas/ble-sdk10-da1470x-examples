/**
 ****************************************************************************************
 *
 * @file gdi.c
 *
 * @brief Basic graphic functions implementation
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
#if dg_configLCDC_ADAPTER
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "gdi.h"
#include "interrupts.h"
#include "osal.h"
#ifdef OS_BAREMETAL
#include "osal_baremetal.h"
#endif
#include "resmgmt.h"
#include "sys_watchdog.h"
#include "sys_power_mgr.h"
#if dg_configUSE_HW_QSPI2
#include "hw_qspi.h"
#include "qspi_automode.h"
#endif
#include "hw_dma.h"
#include "hw_led.h"
#include "sys_timer.h"
#include "ad_i2c.h"
#include "ad_spi.h"

#include "touch_simulation.h"
#ifdef PERFORMANCE_METRICS
#include "metrics.h"
#endif

/**********************************************************************
 *
 *       Defines
 *
 **********************************************************************
 */
#define DEV_DRAW_TIMEOUT_MS                     (200)
#define DEV_CLOSE_TIMEOUT_MS                    (100)

#ifndef GDI_TOUCH_INTERFACE
#define GDI_TOUCH_INTERFACE                     GDI_TOUCH_INTERFACE_OTHER
#endif

#ifndef GDI_CONSOLE_LOG
#define GDI_CONSOLE_LOG                         (0)
#endif

#ifndef GDI_NO_PREALLOC
#define GDI_NO_PREALLOC                         (0)
#endif

#ifndef GDI_FB_USE_QSPI_RAM
#define GDI_FB_USE_QSPI_RAM                     (FB_USE_QSPI_RAM)
#endif

#ifndef GDI_HEAP_USE_QSPI_RAM
#define GDI_HEAP_USE_QSPI_RAM                   (0)
#endif

#if GDI_FB_USE_QSPI_RAM
#ifndef GDI_ENABLE_SYSRAM_OPT
#define GDI_ENABLE_SYSRAM_OPT                   (0)
#endif
#else /* GDI_FB_USE_QSPI_RAM */
#undef GDI_ENABLE_SYSRAM_OPT
#define GDI_ENABLE_SYSRAM_OPT                   (0)
#endif /* GDI_FB_USE_QSPI_RAM */

#if GDI_ENABLE_SYSRAM_OPT && GDI_SINGLE_FB_NUM != 1
#error "Only 1 FB can be set when SysRAM optimization is enabled."
#endif

#ifndef GDI_QSPI_RAM_OFFSET
#define GDI_QSPI_RAM_OFFSET                     (0)
#endif

#ifndef GDI_LCDC_FIFO_PREFETCH_LVL
#define GDI_LCDC_FIFO_PREFETCH_LVL              (HW_LCDC_FIFO_PREFETCH_LVL_3)
#endif

#ifndef GDI_USE_OS_TIMER
#define GDI_USE_OS_TIMER                        (0)
#endif

#ifndef GDI_PWMLED_BACKLIGHT
#define GDI_PWMLED_BACKLIGHT                    (0)
#endif

#if GDI_PWMLED_BACKLIGHT
#ifndef GDI_PWMLED_DUTY_CYCLE_1
#define GDI_PWMLED_DUTY_CYCLE_1                 (0)
#endif

#ifndef GDI_PWMLED_DUTY_CYCLE_2
#define GDI_PWMLED_DUTY_CYCLE_2                 (0)
#endif
#endif /* GDI_PWMLED_BACKLIGHT */

#ifndef GDI_DISP_OFFSETX
#define GDI_DISP_OFFSETX                        (0)
#endif

#ifndef GDI_DISP_OFFSETY
#define GDI_DISP_OFFSETY                        (0)
#endif

#ifndef GDI_USE_CONTINUOUS_MODE
#define GDI_USE_CONTINUOUS_MODE                 (0)
#endif

#ifndef GDI_MULTIPLEX_TOUCH_DISPLAY
#define GDI_MULTIPLEX_TOUCH_DISPLAY             (0)
#endif

/* Notification bit-masks */
#define DEV_DRAW_ASYNC_EVT                      (1 << 0)
#define DEV_DRAW_ASYNC_CMPL_EVT                 (1 << 1)
#define DEV_TOUCH_EVT                           (1 << 2)

#if GDI_USE_CONTINUOUS_MODE
#define DISPLAY_MUTEX_GET() OS_MUTEX_GET(gdi->block_mutex_display_status, OS_MUTEX_FOREVER)
#else
#define DISPLAY_MUTEX_GET()
#endif

#if GDI_USE_CONTINUOUS_MODE
#define DISPLAY_MUTEX_PUT() OS_MUTEX_PUT(gdi->block_mutex_display_status)
#else
#define DISPLAY_MUTEX_PUT()
#endif

#define CEILING_FUNC(quotient, divisor)         (((quotient) + ((divisor) - 1)) / (divisor))
#define FLOOR_FUNC(number, significance)        (((number) / (significance)) * (significance))

#if (dg_configUSE_HW_QSPI2 == 0) && (GDI_FB_USE_QSPI_RAM != 0 || GDI_HEAP_USE_QSPI_RAM != 0) && (GDI_NO_PREALLOC == 0)
#error "QSPI RAM must be enabled in order to place frame buffer or heap!"
#endif

typedef struct {
        uint32_t dst;
        uint32_t src;
        int dst_step;
        int src_step;
        int reps;
        resource_mask_t res;
        DMA_setup dma;
        gdi_t *gdi;
} gdi_dma_t;

#if (!GDI_FB_USE_QSPI_RAM) && (GDI_NO_PREALLOC == 0)
PRIVILEGED_DATA static uint8_t frame_buffer[GDI_FB_RESX * GDI_FB_RESY * GDI_COLOR_BYTES * GDI_SINGLE_FB_NUM];
#endif

#if (!GDI_HEAP_USE_QSPI_RAM) && (GDI_NO_PREALLOC == 0)
PRIVILEGED_DATA static uint32_t gui_heap_area[GDI_GUI_HEAP_SIZE / sizeof(uint32_t)];
#endif

PRIVILEGED_DATA static gdi_t *gdi;

#ifndef OS_BAREMETAL
PRIVILEGED_DATA static OS_TASK task_h;
PRIVILEGED_DATA OS_TIMER draw_async_timer_h;
#endif

#if GDI_CONSOLE_LOG
PRIVILEGED_DATA static uint64_t frame_render_op_start, frame_render_op_end, frame_render_start, frame_render_end, frame_transfer_start, frame_transfer_end;
PRIVILEGED_DATA static int frame_render_op_duration_us, frame_render_duration_us, frame_transfer_duration_us, frame_total_duration_us;
PRIVILEGED_DATA static bool transfer_last;
#endif

void *_gdi_get_frame_buffer_addr(void);
#ifndef OS_BAREMETAL
static void frame_update_async_cb(AD_LCDC_ERROR status, void *cb_data);
#endif

#ifdef PERFORMANCE_METRICS
static int pixel_count;
static int render_count;
#endif


#if GDI_PWMLED_BACKLIGHT
__STATIC_INLINE void hw_led_set_led1_load(uint8_t level)
{
        REG_SETF(PWMLED, PWMLED_CTRL_REG, LED1_LOAD_SEL, level);
}

__STATIC_INLINE void hw_led_set_led2_load(uint8_t level)
{
        REG_SETF(PWMLED, PWMLED_CTRL_REG, LED2_LOAD_SEL, level);
}

void pwmled_backlight_init(uint8_t duty_cycle1, uint8_t duty_cycle2)
{
        bool led1_enable = duty_cycle1 > 0 ? true : false;
        bool led2_enable = duty_cycle2 > 0 ? true : false;

        hw_led_pwm_duty_cycle_t cycle = {
                .hw_led_pwm_start = 0,
        };

        if (led1_enable || led2_enable) {
                hw_led_pwm_set_frequency(100);
        }

        if (led1_enable) {
                duty_cycle1 = MIN(duty_cycle1, 100);
                cycle.hw_led_pwm_end = duty_cycle1;
                hw_led1_pwm_set_duty_cycle(&cycle);
                hw_led_set_led1_load(7);
        }
        hw_led_enable_led1(led1_enable);

        if (led2_enable) {
                duty_cycle2 = MIN(duty_cycle2, 100);
                cycle.hw_led_pwm_end = duty_cycle2;
                hw_led2_pwm_set_duty_cycle(&cycle);
                hw_led_set_led2_load(7);
        }
        hw_led_enable_led2(led2_enable);
}

void pwmled_backlight_set(bool enable)
{
        hw_led_set_pwm_state(enable);
        if (enable) {
                pm_sleep_mode_request(pm_mode_idle);
        } else {
                pm_sleep_mode_release(pm_mode_idle);
        }
}

#endif /* GDI_PWMLED_BACKLIGHT */

uint64_t gdi_get_sys_uptime_ticks(void)
{
#ifndef OS_BAREMETAL
#if GDI_USE_OS_TIMER
        return (in_interrupt() ? OS_GET_TICK_COUNT_FROM_ISR() : OS_GET_TICK_COUNT());
#else
        return (in_interrupt() ? sys_timer_get_uptime_ticks_fromISR() : sys_timer_get_uptime_ticks());
#endif
#else
        return timer_get_uptime_ticks();
#endif
}

uint64_t gdi_convert_ticks_to_us(uint64_t ticks)
{
#if GDI_USE_OS_TIMER && !defined (OS_BAREMETAL)
        return (uint64_t)OS_TICKS_2_MS(ticks) * 1000;
#else
        return (ticks * 1000000UL) / configSYSTICK_CLOCK_HZ;
#endif
}

#if GDI_CONSOLE_LOG
static void console_log(void)
{
#ifdef PERFORMANCE_METRICS
        METRICS metrics;

        if (transfer_last) {
                metrics = get_metrics_data();

                metrics.fps = 10000000UL / frame_total_duration_us;
                metrics.frame_rendering_time = frame_render_duration_us;
                metrics.display_transfer_time = frame_transfer_duration_us;
                metrics.pixel_count = pixel_count;
                metrics_add(&metrics);

                /* Clear variables */
                frame_render_duration_us = frame_transfer_duration_us = frame_total_duration_us = 0;
        }

#if !defined(PERFORMANCE_METRICS)
        printf("%3d.%d\t%3d.%.2d\t%3d.%.2d\r\n", fps / 10, fps % 10, (frame_duration_us / 1000),
                                                                                           (frame_duration_us / 10) % 100,
                                                                                           (transfer_duration_us / 1000),
                                                                                           (transfer_duration_us / 10) % 100);
#endif /* !defined(PERFORMANCE_METRICS) */
#endif
}
#endif

void gdi_perf_render_op_start(uint8_t tag)
{
#ifdef PERFORMANCE_METRICS
        if(!frame_render_op_start) {
                metrics_set_gpu_tag(tag);
                frame_render_op_start = gdi_get_sys_uptime_ticks();
        }
#endif
}

void gdi_perf_render_op_end(void)
{
#ifdef PERFORMANCE_METRICS
        if (frame_render_op_start) {
                frame_render_op_end = gdi_get_sys_uptime_ticks();
                frame_render_op_duration_us = gdi_convert_ticks_to_us(frame_render_op_end - frame_render_op_start);

                metrics_gpu_add(frame_render_op_duration_us);
                frame_render_op_start = frame_render_op_end = 0;
        }
#endif
}

void gdi_perf_render_op_time(int time_us, uint8_t tag)
{
#ifdef PERFORMANCE_METRICS
        frame_render_op_duration_us = time_us;

        metrics_set_gpu_tag(tag);
        metrics_gpu_add(frame_render_op_duration_us);
#endif
}

void gdi_perf_render_start(void)
{
#ifdef PERFORMANCE_METRICS
        if (render_count == 0 ) {
                frame_render_start = gdi_get_sys_uptime_ticks();
        }
        render_count++;
#endif
}

void gdi_perf_render_end(void)
{
#ifdef PERFORMANCE_METRICS
        render_count--;
        if (render_count == 0) {
                frame_render_end = gdi_get_sys_uptime_ticks();

                frame_render_duration_us = gdi_convert_ticks_to_us(frame_render_end - frame_render_start);
                frame_render_start = frame_render_end = 0;
        }
#endif
}

void gdi_perf_render_time(int time_us)
{
#ifdef PERFORMANCE_METRICS
        frame_render_duration_us = time_us;
#endif
}

void gdi_perf_transfer_start(void)
{
#ifdef PERFORMANCE_METRICS
        frame_transfer_start = gdi_get_sys_uptime_ticks();
#endif
}

void gdi_perf_transfer_end(void)
{
#ifdef PERFORMANCE_METRICS
        uint64_t now = gdi_get_sys_uptime_ticks();

        if (transfer_last) {
                frame_total_duration_us = gdi_convert_ticks_to_us(now - frame_transfer_end);
                frame_transfer_end = now;
        }

        frame_transfer_duration_us += gdi_convert_ticks_to_us(now - frame_transfer_start);

        console_log();
#endif
}

void gdi_perf_transfer_time(int time_us)
{
#ifdef PERFORMANCE_METRICS
        uint64_t now = gdi_get_sys_uptime_ticks();

        frame_total_duration_us = gdi_convert_ticks_to_us(now - frame_transfer_end);

        frame_transfer_end = now;

        frame_transfer_duration_us = time_us;

        console_log();
#endif
}

void gdi_perf_transfer_last(bool last)
{
#ifdef PERFORMANCE_METRICS
        transfer_last = last;
#endif
}

static void dev_open_display(void)
{
        if (gdi->continuous_mode_enable_current) {
                /* The device must already be opened */
                OS_ASSERT(gdi->display_h != NULL);
                return;
        }

#if GDI_MULTIPLEX_TOUCH_DISPLAY
        OS_MUTEX_GET(gdi->block_mutex_multiplex_display_touch, OS_MUTEX_FOREVER);
#endif
        OS_ASSERT(gdi->config != NULL);
        ad_lcdc_handle_t dev = ad_lcdc_open(gdi->config);

#if dg_configLCDC_TEST_APP
        hw_lcdc_set_bg_color(GDI_BG_COLOR_RED_FIELD(gdi->bg_color), GDI_BG_COLOR_GREEN_FIELD(gdi->bg_color),
                                                GDI_BG_COLOR_BLUE_FIELD(gdi->bg_color), GDI_BG_COLOR_ALPHA_FIELD(gdi->bg_color));
#endif /* dg_configLCDC_TEST_APP */
        OS_ASSERT(dev != NULL);
        gdi->display_h = dev;
}

static void dev_close_display(void)
{
        /* If continuous mode is enabled just return. */
        if (gdi->continuous_mode_enable_current) {
                return;
        }

        if (gdi->display_h == NULL) {
                return;
        }

        ad_lcdc_handle_t handle = gdi->display_h;
        gdi->display_h = NULL;

        OS_TICK_TIME timeout = OS_GET_TICK_COUNT() + OS_MS_2_TICKS(DEV_CLOSE_TIMEOUT_MS);
        while(ad_lcdc_close(handle, false) == AD_LCDC_ERROR_CONTROLLER_BUSY) {
                if (OS_GET_TICK_COUNT()) {
                        if (timeout <= OS_GET_TICK_COUNT()) {
                                ad_lcdc_close(handle, true);
                                printf("LCDC close timeout!!\r\n");
                                break;
                        }
                } else {
                        if (!--timeout) {
                                ad_lcdc_close(handle, true);
                                break;
                        }
                }
                OS_DELAY(1);
        }
#if GDI_MULTIPLEX_TOUCH_DISPLAY
        OS_MUTEX_PUT(gdi->block_mutex_multiplex_display_touch);
#endif
}

#if GDI_TOUCH_ENABLE
static void dev_open_touch(void)
{
        void * dev = NULL;
#if GDI_MULTIPLEX_TOUCH_DISPLAY
        OS_MUTEX_GET(gdi->block_mutex_multiplex_display_touch, OS_MUTEX_FOREVER);
#endif
#if (GDI_TOUCH_INTERFACE == GDI_TOUCH_INTERFACE_I2C)
        dev = ad_i2c_open(GDI_TOUCH_CONFIG);
        OS_ASSERT(dev != NULL);
#elif (GDI_TOUCH_INTERFACE == GDI_TOUCH_INTERFACE_SPI)
        dev = ad_spi_open(GDI_TOUCH_CONFIG);
        OS_ASSERT(dev != NULL);
#endif
        gdi->touch_h = dev;
}

static void dev_close_touch(void)
{
#if (GDI_TOUCH_INTERFACE != GDI_TOUCH_INTERFACE_OTHER)
        OS_TICK_TIME timeout = OS_GET_TICK_COUNT() + OS_MS_2_TICKS(DEV_CLOSE_TIMEOUT_MS);
#endif

#if (GDI_TOUCH_INTERFACE == GDI_TOUCH_INTERFACE_I2C)
        while (ad_i2c_close(gdi->touch_h, false) != AD_I2C_ERROR_NONE) {
                if (timeout <= OS_GET_TICK_COUNT()) {
                        ad_i2c_close(gdi->touch_h, true);
                        break;
                }
                OS_DELAY(1);
        }
#elif (GDI_TOUCH_INTERFACE == GDI_TOUCH_INTERFACE_SPI)
        while (ad_spi_close(gdi->touch_h, false) != AD_SPI_ERROR_NONE) {
                if (timeout <= OS_GET_TICK_COUNT()) {
                        ad_spi_close(gdi->touch_h, true);
                        break;
                }
                OS_DELAY(1);
        }
#endif
#if GDI_MULTIPLEX_TOUCH_DISPLAY
        OS_MUTEX_PUT(gdi->block_mutex_multiplex_display_touch);
#endif
}
#endif /* GDI_TOUCH_ENABLE */

static void handle_error(int error)
{
        switch (error) {
        case AD_LCDC_ERROR_LLD_ERROR: {
                int code = ad_lcdc_get_lld_status(gdi->display_h);
                printf("Return: %X (%d)\r\n", code, code);
                dev_close_display();
                dev_open_display();
                break;
        }
        case AD_LCDC_ERROR_UNDERFLOW:
                printf("Underflow!!\r\n");
                break;
        }
}

static void screen_setup(void)
{
        int ret;

        dev_open_display();

        ret = ad_lcdc_execute_cmds(gdi->display_h, gdi->screen_init_cmds, gdi->screen_init_cmds_len);
        handle_error(ret);

        ret = ad_lcdc_set_display_offset(gdi->display_h, gdi->screen_offsetx, gdi->screen_offsety);
        handle_error(ret);

        dev_close_display();
}

static void screen_power_on(void)
{
        int ret;

        if (!gdi->screen_power_on_cmds_len) {
                return;
        }
        dev_open_display();

        ret = ad_lcdc_execute_cmds(gdi->display_h, gdi->screen_power_on_cmds, gdi->screen_power_on_cmds_len);
        handle_error(ret);

        dev_close_display();
}

static void screen_enable(void)
{
        int ret;

#if GDI_PWMLED_BACKLIGHT
        pwmled_backlight_set(true);
#endif /* GDI_PWMLED_BACKLIGHT */
        if (!gdi->screen_enable_cmds_len) {
                return;
        }
        dev_open_display();

        ret = ad_lcdc_execute_cmds(gdi->display_h, gdi->screen_enable_cmds, gdi->screen_enable_cmds_len);
        handle_error(ret);

        dev_close_display();
}

static void screen_disable(void)
{
        int ret;

        if (!gdi->screen_disable_cmds_len) {
                return;
        }
        dev_open_display();

        ret = ad_lcdc_execute_cmds(gdi->display_h, gdi->screen_disable_cmds, gdi->screen_disable_cmds_len);
        handle_error(ret);

        dev_close_display();
#if GDI_PWMLED_BACKLIGHT
        pwmled_backlight_set(false);
#endif /* GDI_PWMLED_BACKLIGHT */
}

static void screen_power_off(void)
{
        int ret;

        if (!gdi->screen_power_off_cmds_len) {
                return;
        }
        dev_open_display();

        ret = ad_lcdc_execute_cmds(gdi->display_h, gdi->screen_power_off_cmds, gdi->screen_power_off_cmds_len);
        handle_error(ret);

        dev_close_display();
}

static void screen_clear(void)
{
        int ret;

        if (!gdi->screen_clear_cmds_len) {
                return;
        }
        dev_open_display();

        ret = ad_lcdc_execute_cmds(gdi->display_h, gdi->screen_clear_cmds, gdi->screen_clear_cmds_len);
        handle_error(ret);

        dev_close_display();
}

static void frame_update_cb(AD_LCDC_ERROR status, void *cb_data)
{
        gdi_t *data = (gdi_t *)cb_data;

        data->underflow = (status == AD_LCDC_ERROR_UNDERFLOW);
        if (status) {
                handle_error(status);
        }

        OS_EVENT_SIGNAL_FROM_ISR(data->draw_event);
}

static void dev_continuous_mode_enable(void)
{
        DISPLAY_MUTEX_GET();
        if (!gdi->continuous_mode_enable_current) {
                dev_open_display();
                /* Set the flag so that, the display handle is no longer updated */
                gdi->continuous_mode_enable_current = true;
                ad_lcdc_continuous_update_start(gdi->display_h, frame_update_cb, gdi);
        }
        DISPLAY_MUTEX_PUT();
}

static void dev_continuous_mode_disable(void)
{
      DISPLAY_MUTEX_GET();
      if (gdi->continuous_mode_enable_current) {
              ad_lcdc_continuous_update_stop(gdi->display_h);
              /* Zero the flag so that, the device can close */
              gdi->continuous_mode_enable_current = false;
              dev_close_display();
      }
      DISPLAY_MUTEX_PUT();
}

static void dev_draw(gdi_t *gdi)
{
        if (gdi->continuous_mode_enable_current) {
                /* The new layer settings will be applied once the current frame cycle is complete */
                for (HW_LCDC_LAYER layer_no = 0; layer_no < HW_LCDC_LAYER_MAX; layer_no++) {
                                if (gdi->layer[layer_no].layer_dirty) {
                                        gdi->layer[layer_no].layer_dirty = false;
                                        ad_lcdc_setup_layer(gdi->display_h, layer_no, gdi->layer[layer_no].layer_enable, &gdi->layer[layer_no].layer);
                                }
                }

                OS_EVENT_CHECK(gdi->draw_event);

                /* Wait for the start of a new frame cycle */
                if (OS_EVENT_SIGNALED != OS_EVENT_WAIT(gdi->draw_event, OS_MS_2_TICKS(DEV_DRAW_TIMEOUT_MS))) {
#if GDI_CONSOLE_LOG
                        printf("FRAME ERROR - ");
#endif
                 }

                /* Wait for the completion of the previously tracked frame cycle */
                if (OS_EVENT_SIGNALED != OS_EVENT_WAIT(gdi->draw_event, OS_MS_2_TICKS(DEV_DRAW_TIMEOUT_MS))) {
#if GDI_CONSOLE_LOG
                        printf("FRAME ERROR - ");
#endif
                 }
        }
        else {
#ifdef GDI_DISP_PRE_DRAW
                        GDI_DISP_PRE_DRAW(gdi, gdi->display_h, &gdi->active_layer);
#endif /* GDI_DISP_PRE_DRAW */
                        for (HW_LCDC_LAYER layer_no = 0; layer_no < HW_LCDC_LAYER_MAX; layer_no++) {
                                        if (gdi->layer[layer_no].layer_dirty) {
                                                gdi->layer[layer_no].layer_dirty = false;
                                                ad_lcdc_setup_layer(gdi->display_h, layer_no, gdi->layer[layer_no].layer_enable, &gdi->layer[layer_no].layer);
                                        }
                        }
                        gdi_perf_transfer_start();

                        ad_lcdc_draw_screen_async(gdi->display_h, frame_update_cb, gdi);

                        if (OS_EVENT_SIGNALED != OS_EVENT_WAIT(gdi->draw_event, OS_MS_2_TICKS(DEV_DRAW_TIMEOUT_MS))) {
#if GDI_CONSOLE_LOG
                                printf("FRAME ERROR - ");
#endif
                        }

#ifdef GDI_DISP_POST_DRAW
                        GDI_DISP_POST_DRAW(gdi, gdi->display_h, &gdi->active_layer);
#endif /* GDI_DISP_POST_DRAW */

                        gdi_perf_transfer_end();
          }
}

static void dev_set_partial_update(void)
{
        int ret = ad_lcdc_set_partial_update(gdi->display_h, &gdi->frame);
        handle_error(ret);
}

HW_LCDC_LAYER_COLOR_MODE gdi_to_layer_color_format(gdi_color_fmt_t format)
{
        switch (format) {
        case GDI_FORMAT_RGB332:   return HW_LCDC_LCM_RGB332;
        case GDI_FORMAT_RGB565:   return HW_LCDC_LCM_RGB565;
        case GDI_FORMAT_RGBA5551: return HW_LCDC_LCM_RGBA5551;
        case GDI_FORMAT_RGB888:   return HW_LCDC_LCM_RGB888;
        case GDI_FORMAT_RGBA4444: return HW_LCDC_LCM_RGBA4444;
        case GDI_FORMAT_ARGB4444: return HW_LCDC_LCM_ARGB4444;
        case GDI_FORMAT_ARGB8888: return HW_LCDC_LCM_ARGB8888;
        case GDI_FORMAT_RGBA8888: return HW_LCDC_LCM_RGBA8888;
        case GDI_FORMAT_ABGR8888: return HW_LCDC_LCM_ABGR8888;
        case GDI_FORMAT_BGRA8888: return HW_LCDC_LCM_BGRA8888;
        default:                  ASSERT_ERROR(0);
        }
        // Should never get here...
        return 0;
}

gdi_color_fmt_t gdi_from_layer_color_format(HW_LCDC_LAYER_COLOR_MODE format)
{
        switch (format) {
        case HW_LCDC_LCM_RGB332:   return GDI_FORMAT_RGB332;
        case HW_LCDC_LCM_RGB565:   return GDI_FORMAT_RGB565;
        case HW_LCDC_LCM_RGBA5551: return GDI_FORMAT_RGBA5551;
        case HW_LCDC_LCM_RGB888:   return GDI_FORMAT_RGB888;
        case HW_LCDC_LCM_RGBA4444: return GDI_FORMAT_RGBA4444;
        case HW_LCDC_LCM_ARGB4444: return GDI_FORMAT_ARGB4444;
        case HW_LCDC_LCM_ARGB8888: return GDI_FORMAT_ARGB8888;
        case HW_LCDC_LCM_RGBA8888: return GDI_FORMAT_RGBA8888;
        case HW_LCDC_LCM_ABGR8888: return GDI_FORMAT_ABGR8888;
        case HW_LCDC_LCM_BGRA8888: return GDI_FORMAT_BGRA8888;
        default:                  ASSERT_ERROR(0);
        }
        // Should never get here...
        return 0;
}


static void dev_draw_async_signal(void)
{
#if !DEVICE_FPGA && !defined(OS_BAREMETAL)
        OS_EVENT_SIGNAL(gdi->draw_smphr);
#endif

        if (gdi->draw_cb) {
                draw_callback cb = gdi->draw_cb;
                gdi->draw_cb = NULL;

                cb(gdi->underflow, gdi->user_data);
        }
}

#ifndef OS_BAREMETAL
static void draw_async_cmpl(void)
#else
static void draw_async_cmpl(AD_LCDC_ERROR status, void *user_data)
#endif
{
#ifdef OS_BAREMETAL
        gdi->underflow = (status == AD_LCDC_ERROR_UNDERFLOW);

        if (status) {
                handle_error(status);
        }
#endif
#ifdef GDI_DISP_POST_DRAW
        GDI_DISP_POST_DRAW(gdi, gdi->display_h, &gdi->active_layer);
#endif /* GDI_DISP_POST_DRAW */
        gdi_perf_transfer_end();

        dev_close_display();

        DISPLAY_MUTEX_PUT();
        dev_draw_async_signal();
}

void dev_draw_async_evt(void)
{
        /* Display state must not change while draw operations are in progress */
        DISPLAY_MUTEX_GET();

        if (!gdi->continuous_mode_enable_current) {
                dev_open_display();
#ifdef GDI_DISP_PRE_DRAW
                GDI_DISP_PRE_DRAW(gdi, gdi->display_h, &gdi->active_layer);
#endif /* GDI_DISP_PRE_DRAW */

                for (HW_LCDC_LAYER layer_no = 0; layer_no < HW_LCDC_LAYER_MAX; layer_no++) {
                                if (gdi->layer[layer_no].layer_dirty) {
                                        gdi->layer[layer_no].layer_dirty = false;
                                        ad_lcdc_setup_layer(gdi->display_h, layer_no, gdi->layer[layer_no].layer_enable, &gdi->layer[layer_no].layer);
                                }
                }

                gdi_perf_transfer_start();

#ifndef OS_BAREMETAL
                ad_lcdc_draw_screen_async(gdi->display_h, frame_update_async_cb, draw_async_timer_h);
#else
                ad_lcdc_draw_screen_async(gdi->display_h, draw_async_cmpl, NULL);
#endif
                /*
                 * Protect the LCD controller from hanging. There might be the case in which the
                 * TE signal is enabled and the display is turned off while a frame update is in
                 * progress. The result would be for LCDC to infinitely wait for the TE signal.
                 */
                OS_TIMER_RESET(draw_async_timer_h, OS_TIMER_FOREVER);
        }
        else {
                /* The new layer settings will be applied once the current frame cycle is complete */
                for (HW_LCDC_LAYER layer_no = 0; layer_no < HW_LCDC_LAYER_MAX; layer_no++) {
                                if (gdi->layer[layer_no].layer_dirty) {
                                        gdi->layer[layer_no].layer_dirty = false;
                                        ad_lcdc_setup_layer(gdi->display_h, layer_no, gdi->layer[layer_no].layer_enable, &gdi->layer[layer_no].layer);
                                }
                }

                OS_EVENT_CHECK(gdi->draw_event);

                /* Wait for the start of a new frame cycle */
                if (OS_EVENT_SIGNALED != OS_EVENT_WAIT(gdi->draw_event, OS_MS_2_TICKS(DEV_DRAW_TIMEOUT_MS))) {
#if GDI_CONSOLE_LOG
                        printf("FRAME ERROR - ");
#endif
                 }

                /* Wait for the completion of the previously tracked frame cycle */
                if (OS_EVENT_SIGNALED != OS_EVENT_WAIT(gdi->draw_event, OS_MS_2_TICKS(DEV_DRAW_TIMEOUT_MS))) {
#if GDI_CONSOLE_LOG
                        printf("FRAME ERROR - ");
#endif
                }

                DISPLAY_MUTEX_PUT();
                dev_draw_async_signal();
        }
}

static void dev_draw_async(draw_callback cb, void *user_data)
{
#if !DEVICE_FPGA && !defined(OS_BAREMETAL)
        OS_EVENT_WAIT(gdi->draw_smphr, OS_EVENT_FOREVER);
#endif

        gdi->draw_cb = cb;
        gdi->user_data = user_data;

#ifndef OS_BAREMETAL
        OS_TASK_NOTIFY(task_h, DEV_DRAW_ASYNC_EVT, OS_NOTIFY_SET_BITS);
#else
        dev_draw_async_evt();
#endif
}

static uint32_t calc_stride(gdi_color_fmt_t format, gdi_coord_t width)
{
        return hw_lcdc_stride_size(gdi_to_layer_color_format(format), width);
}

#if GDI_TOUCH_ENABLE
static void dev_notify_touch(void)
{
        /* Make sure that GDI is already up and running */
        if (task_h) {
                in_interrupt() ?
                                OS_TASK_NOTIFY_FROM_ISR(task_h, DEV_TOUCH_EVT, OS_NOTIFY_SET_BITS) :
                                OS_TASK_NOTIFY(task_h, DEV_TOUCH_EVT, OS_NOTIFY_SET_BITS);
        }
}

static void dev_init_touch(void)
{
        dev_open_touch();
#ifdef GDI_TOUCH_INIT
        /* Initialize the target touch controller */
        GDI_TOUCH_INIT(gdi->touch_h);
#endif
        dev_close_touch();
}

static void dev_read_data_touch(void)
{
        gdi_touch_data_t touch_data;

        dev_open_touch();
#ifdef GDI_TOUCH_READ_EVENT
        GDI_TOUCH_READ_EVENT(gdi->touch_h, &touch_data);
#endif
        dev_close_touch();

        /* Push touch events into the graphics buffers */
        if (gdi->store_touch_cb) {
                gdi->store_touch_cb(&touch_data);
        } else {
                /* Set a callback to store the touch events */
                OS_ASSERT(0);
        }
}
#endif /* GDI_TOUCH_ENABLE */

#ifndef OS_BAREMETAL
static void frame_update_async_cb(AD_LCDC_ERROR status, void *cb_data)
{
        OS_TIMER timer = (OS_TIMER)cb_data;
        OS_TIMER_STOP_FROM_ISR(timer);

        gdi->underflow = (status == AD_LCDC_ERROR_UNDERFLOW);
        if (status) {
                handle_error(status);
        }

        OS_TASK_NOTIFY_FROM_ISR(task_h, DEV_DRAW_ASYNC_CMPL_EVT, OS_NOTIFY_SET_BITS);
}

static void draw_async_timer_cb(OS_TIMER timer)
{
        OS_TASK task = (OS_TASK)OS_TIMER_GET_TIMER_ID(timer);
        OS_TASK_NOTIFY(task, DEV_DRAW_ASYNC_CMPL_EVT, OS_NOTIFY_SET_BITS);
}

static void gdi_task(void *pvParameters)
{
        int8_t wdog_id;
        bool draw_async_cmpl_en;

        draw_async_timer_h = OS_TIMER_CREATE("DRAW_TIMEOUT", OS_MS_2_TICKS(DEV_DRAW_TIMEOUT_MS), OS_TIMER_FAIL,
                                                                                 (void *)OS_GET_CURRENT_TASK(),
                                                                                          draw_async_timer_cb);
        OS_ASSERT(draw_async_timer_h);


        /* Register Wearable Sleep Quality task to be monitored by watchdog */
        wdog_id = sys_watchdog_register(false);
        draw_async_cmpl_en = 0;

        for (;;) {
                OS_BASE_TYPE ret;
                uint32_t notif;

                /* Notify watchdog on each loop */
                sys_watchdog_notify(wdog_id);

                /* Suspend watchdog while blocking on OS_TASK_NOTIFY_WAIT() */
                sys_watchdog_suspend(wdog_id);

                ret = OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif, portMAX_DELAY);
                OS_ASSERT(ret == OS_TASK_NOTIFY_SUCCESS);

                /* Resume watchdog */
                sys_watchdog_notify_and_resume(wdog_id);

                if (notif & DEV_DRAW_ASYNC_EVT) {
                        draw_async_cmpl_en = 1;
                        dev_draw_async_evt();
                }

                if (notif & DEV_DRAW_ASYNC_CMPL_EVT) {
                        /*
                         * Protect the following code block from being executed twice.
                         * There might be the case in which both the timer and the
                         * drawing operation are complete simultaneously.
                         */
                        if (draw_async_cmpl_en) {
                                draw_async_cmpl_en = 0;
                                draw_async_cmpl();
                        }
                }

#if GDI_TOUCH_ENABLE
                if (notif & DEV_TOUCH_EVT) {
                        /* Read touch events */
                        dev_read_data_touch();
                }
#endif /* GDI_TOUCH_ENABLE */
        }
}
#endif

static int gdi_setup_screen(const ad_lcdc_controller_conf_t *cfg)
{
        gdi->config = cfg;
        gdi->width = GDI_DISP_RESX;
        gdi->height = GDI_DISP_RESY;
        gdi->screen_offsetx = GDI_DISP_OFFSETX;
        gdi->screen_offsety = GDI_DISP_OFFSETY;

        gdi->frame.startx = 0;
        gdi->frame.starty = 0;
        gdi->frame.endx = gdi->width - 1;
        gdi->frame.endy = gdi->height - 1;

        gdi->screen_init_cmds = screen_init_cmds;
        gdi->screen_power_on_cmds = screen_power_on_cmds;
        gdi->screen_enable_cmds = screen_enable_cmds;
        gdi->screen_disable_cmds = screen_disable_cmds;
        gdi->screen_power_off_cmds = screen_power_off_cmds;
        gdi->screen_clear_cmds = screen_clear_cmds;
        gdi->screen_color_modes = screen_color_modes;

        gdi->screen_set_partial_update_area = screen_set_partial_update_area;
        gdi->continuous_mode_enable_config = GDI_USE_CONTINUOUS_MODE;
        gdi->screen_set_color_mode = screen_set_color_mode;

        gdi->screen_init_cmds_len = sizeof(screen_init_cmds);
        gdi->screen_power_on_cmds_len = sizeof(screen_power_on_cmds);
        gdi->screen_enable_cmds_len = sizeof(screen_enable_cmds);
        gdi->screen_disable_cmds_len = sizeof(screen_disable_cmds);
        gdi->screen_power_off_cmds_len = sizeof(screen_power_off_cmds);
        gdi->screen_clear_cmds_len = sizeof(screen_clear_cmds);
        gdi->screen_color_modes_len = sizeof(screen_color_modes);

        gdi->display_powered = false;
        gdi->display_enabled = false;
#if dg_configLCDC_TEST_APP
        gdi->bg_color = 0x000000FF;
#endif
        gdi_display_power_on();
        screen_setup();
        gdi_display_enable();
        gdi_set_partial_update_area(0, 0, gdi->width - 1, gdi->height - 1);
        return 0;
}

void gdi_set_layer_src(HW_LCDC_LAYER layer_no, void *address, gdi_coord_t resx, gdi_coord_t resy, gdi_color_fmt_t format)
{
        gdi->layer[layer_no].layer.baseaddr = (uint32_t)address;
        gdi->layer[layer_no].layer.format = gdi_to_layer_color_format(format);
        gdi->layer[layer_no].layer.resx = resx;
        gdi->layer[layer_no].layer.resy = resy;
        gdi->layer[layer_no].layer.stride = calc_stride(format, resx);
        gdi->layer[layer_no].layer_dirty = true;
}

void gdi_set_layer_start(HW_LCDC_LAYER layer_no, int startx, int starty)
{
        gdi->layer[layer_no].layer.startx = startx;
        gdi->layer[layer_no].layer.starty = starty;
        gdi->layer[layer_no].layer_dirty = true;
}

void gdi_set_layer_blending(HW_LCDC_LAYER layer_no, HW_LCDC_BLEND_MODE blendmode, uint8_t alpha)
{
        gdi->layer[layer_no].layer.blendmode = blendmode;
        gdi->layer[layer_no].layer.alpha = alpha;
        gdi->layer[layer_no].layer_dirty = true;
}

void gdi_set_layer_dirty(HW_LCDC_LAYER layer_no, bool dirty)
{
        gdi->layer[layer_no].layer_dirty = dirty;
}

#if dg_configLCDC_TEST_APP
void gdi_set_bg_color(uint32_t bg_color)
{
        gdi->bg_color = bg_color;
}
#endif /* dg_configLCDC_TEST_APP */

void gdi_set_layer_enable(HW_LCDC_LAYER layer_no, bool enable)
{
        if (gdi->layer[layer_no].layer_enable != enable) {
                gdi->layer[layer_no].layer_enable = enable;
                gdi->layer[layer_no].layer_dirty = true;
        }
}

bool gdi_get_layer_enable(HW_LCDC_LAYER layer_no)
{
        return gdi->layer[layer_no].layer_enable;
}

uint32_t * gdi_set_palette_lut(uint32_t *palette_lut)
{
        ad_lcdc_driver_conf_t *cfg = (ad_lcdc_driver_conf_t *)gdi->config->drv;
        uint32_t *palette_lut_old = cfg->palette_lut;

        cfg->palette_lut = palette_lut;
        return palette_lut_old;
}

size_t gdi_setup_layer(HW_LCDC_LAYER layer_no, void *address, gdi_coord_t resx, gdi_coord_t resy, gdi_color_fmt_t format, int buffers)
{
        size_t i = 0;

        buffers = MIN(GDI_SINGLE_FB_NUM, buffers);

        gdi->layer[layer_no].single_buff_sz = calc_stride(format, resx) * resy;
        gdi->layer[layer_no].bufs_num = buffers;
        gdi->layer[layer_no].active_buf = 0;
        gdi->layer[layer_no].buffer[i] = (uint8_t *)address;
        for (i = 1; i < buffers; i++) {
                gdi->layer[layer_no].buffer[i] = (uint8_t *)(gdi->layer[layer_no].single_buff_sz + (size_t)gdi->layer[layer_no].buffer[i - 1]);
        }

        gdi->layer[layer_no].layer.dma_prefetch_lvl = GDI_LCDC_FIFO_PREFETCH_LVL;
        gdi_set_layer_src(layer_no, gdi->layer[layer_no].buffer[0], resx, resy, format);
        gdi_set_layer_start(layer_no, 0, 0);
        gdi_set_layer_blending(layer_no, HW_LCDC_BL_SRC, 0xFF);

        gdi_set_layer_dirty(layer_no, true); //! Mark layer as 'dirty' so that setting can be applied

        return gdi->layer[layer_no].single_buff_sz * gdi->layer[layer_no].bufs_num;
}

gdi_t *gdi_init(void)
{
        gdi_color_fmt_t format = GDI_FORMAT_ARGB8888;
        size_t layers_size = 0;

        (void)format;
        (void)layers_size;

        if (gdi) {
                return gdi;
        }

        gdi = OS_MALLOC(sizeof(gdi_t));
        OS_ASSERT(gdi != NULL);
        memset(gdi, 0, sizeof(gdi_t));

        for (HW_LCDC_LAYER layer_no = 0; layer_no < HW_LCDC_LAYER_MAX; layer_no++) {
                gdi->layer[layer_no].buffer = OS_MALLOC(sizeof(uint8_t *) * GDI_SINGLE_FB_NUM);
                OS_ASSERT(gdi->layer[layer_no].buffer != NULL);
                memset(gdi->layer[layer_no].buffer, 0, sizeof(sizeof(uint8_t *) * GDI_SINGLE_FB_NUM));
        }

        gdi_setup_screen(GDI_LCDC_CONFIG);

#if GDI_FB_COLOR_FORMAT == CF_NATIVE_RGB332
        format = GDI_FORMAT_RGB332;
#elif GDI_FB_COLOR_FORMAT == CF_NATIVE_RGB565
        format = GDI_FORMAT_RGB565;
#elif GDI_FB_COLOR_FORMAT == CF_NATIVE_RGBA8888
        format = GDI_FORMAT_RGBA8888;
#elif GDI_FB_COLOR_FORMAT == CF_NATIVE_ARGB8888
        format = GDI_FORMAT_ARGB8888;
#endif

        OS_EVENT_CREATE(gdi->draw_event);
        OS_EVENT_CREATE(gdi->dma_event);
#if GDI_MULTIPLEX_TOUCH_DISPLAY
        OS_MUTEX_CREATE(gdi->block_mutex_multiplex_display_touch);
#endif
#if GDI_USE_CONTINUOUS_MODE
        OS_MUTEX_CREATE(gdi->block_mutex_display_status);
#endif

#if !DEVICE_FPGA && !defined(OS_BAREMETAL)
        gdi->draw_smphr = xSemaphoreCreateCounting(1, 1);
#endif

#if GDI_TOUCH_ENABLE
        dev_init_touch();
#endif

#if !GDI_NO_PREALLOC
        int layer_size, address = (int)_gdi_get_frame_buffer_addr();

        for (HW_LCDC_LAYER layer_no = HW_LCDC_LAYER_0; layer_no < HW_LCDC_LAYER_MAX; layer_no++) {
                layer_size = gdi_setup_layer(layer_no, (void *)address, GDI_FB_RESX, GDI_FB_RESY, format, GDI_SINGLE_FB_NUM);
                address += layer_size;
                layers_size += layer_size;
        }
#if dg_configUSE_HW_QSPI2
        size_t qspi_area = GDI_QSPI_RAM_OFFSET;
#if GDI_FB_USE_QSPI_RAM
        if (qspi_is_valid_addr((uint32_t)_gdi_get_frame_buffer_addr())) {
                qspi_area += layers_size;
        }
#endif /* GDI_FB_USE_QSPI_RAM */
#if GDI_HEAP_USE_QSPI_RAM
        if (qspi_is_valid_addr((uint32_t)gdi_get_gui_heap_addr())) {
                qspi_area += GDI_GUI_HEAP_SIZE;
        }
#endif /* GDI_HEAP_USE_QSPI_RAM */
        OS_ASSERT(qspi_get_device_size(HW_QSPIC2) >= qspi_area);
#endif /* dg_configUSE_HW_QSPI2 */
#endif /* GDI_NO_PREALLOC */

#if GDI_PWMLED_BACKLIGHT
        pwmled_backlight_init(GDI_PWMLED_DUTY_CYCLE_1, GDI_PWMLED_DUTY_CYCLE_2);
#endif /* GDI_PWMLED_BACKLIGHT */

        OS_BASE_TYPE res = OS_TASK_CREATE( "GDI_task",  /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                        gdi_task,                       /* The function that implements the task. */
                        NULL,                           /* The parameter passed to the task. */
                        2048,                           /* The number of bytes to allocate to the
                                                           stack of the task. */
                        OS_TASK_PRIORITY_NORMAL + 1,        /* The priority assigned to the task. */
                        task_h );                       /* The task handle */
        OS_ASSERT(res == OS_TASK_CREATE_SUCCESS);

        return gdi;
}

void gdi_set_callback_store_touch(store_touch_callback cb)
{
        if (cb) {
                gdi->store_touch_cb = cb;
        }
}

void gdi_touch_event(void)
{
#if GDI_TOUCH_ENABLE
        dev_notify_touch();
#endif
}

void gdi_display_power_on(void)
{
        if (!gdi->display_powered) {
                screen_power_on();
        }
        gdi->display_powered = true;
}

void gdi_display_power_off(void)
{
        if (gdi->display_powered) {
                screen_power_off();
        }
        gdi->display_powered = false;
}

bool gdi_display_is_powered(void)
{
        return gdi->display_powered;
}

void gdi_display_enable(void)
{
        if (!gdi->display_enabled) {
                if (gdi->continuous_mode_enable_config) {
                        dev_continuous_mode_enable();
                }

                screen_enable();
        }

        gdi->display_enabled = true;
}

void gdi_display_disable(void)
{
        if (gdi->display_enabled) {
                screen_disable();

                if (gdi->continuous_mode_enable_config) {
                        dev_continuous_mode_disable();
                }
        }

        gdi->display_enabled = false;
}

bool gdi_display_is_enabled(void)
{
        return gdi->display_enabled;
}

void gdi_display_update(void)
{
        dev_open_display();
        dev_draw(gdi);
        dev_close_display();
}

void gdi_display_update_async(draw_callback cb, void *user_data)
{
        dev_draw_async(cb, user_data);
}

/* Routine to update the output color mode */
bool gdi_set_color_mode(HW_LCDC_OUTPUT_COLOR_MODE mode)
{
        bool status = false;
        if (gdi->screen_set_color_mode != NULL) {
                dev_open_display();
                status = gdi->screen_set_color_mode(mode);
                dev_close_display();

                /* Update controller's settings */
                if (status) {
                        hw_lcdc_config_t *cfg = (hw_lcdc_config_t *)&gdi->config->drv->hw_init;
                        cfg->format = mode;
                }
        }
        return status;
}

/* Routine to get the GDI configuration structure */
gdi_t * gdi_get_gdi_structure(void)
{
        return gdi;
}

void gdi_round_partial_update_area(gdi_coord_t *x0, gdi_coord_t *y0, gdi_coord_t *x1, gdi_coord_t *y1)
{
        hw_lcdc_frame_t frame;

        frame.startx = MAX(*x0, 0);
        frame.starty = MAX(*y0, 0);
        frame.endx = MIN(*x1, gdi->width - 1);
        frame.endy = MIN(*y1, gdi->height - 1);

        if (gdi->screen_set_partial_update_area) {
                gdi->screen_set_partial_update_area(&frame);
        }


        *x0 = frame.startx;
        *y0 = frame.starty;
        *x1 = frame.endx;
        *y1 = frame.endy;
}

void gdi_set_partial_update_area(gdi_coord_t x0, gdi_coord_t y0, gdi_coord_t x1, gdi_coord_t y1)
{
        hw_lcdc_frame_t frame;

        if (x0 > x1 || y0 > y1) {
                return;
        }

        frame.startx = x0;
        frame.starty = y0;
        frame.endx = x1;
        frame.endy = y1;

        gdi_round_partial_update_area(&frame.startx, &frame.starty, &frame.endx, &frame.endy);

        if (!memcmp(&gdi->frame, &frame, sizeof(frame))) {
                return;
        }
        memcpy(&gdi->frame, &frame, sizeof(frame));

#ifdef PERFORMANCE_METRICS
        pixel_count = (frame.endx - frame.startx + 1) * (frame.endy - frame.starty + 1);
#endif

        dev_open_display();
        dev_set_partial_update();
        dev_close_display();
}

void gdi_display_clear(void)
{
        screen_clear();
}

uint8_t gdi_set_next_frame_buffer(HW_LCDC_LAYER layer_no)
{
        if (++gdi->layer[layer_no].active_buf >= gdi->layer[layer_no].bufs_num) {
                gdi->layer[layer_no].active_buf = 0;
        }
        return gdi->layer[layer_no].active_buf;
}

uint8_t gdi_get_current_frame_buffer(HW_LCDC_LAYER layer_no)
{
        return gdi->layer[layer_no].active_buf;
}

void gdi_set_frame_buffer(HW_LCDC_LAYER layer_no, uint8_t frame)
{
        gdi->layer[layer_no].active_buf = frame >= gdi->layer[layer_no].bufs_num ? 0 : frame;
        gdi->layer[layer_no].layer.baseaddr = (uint32_t)gdi->layer[layer_no].buffer[gdi->layer[layer_no].active_buf];
        gdi->layer[layer_no].layer_dirty = true;
}


void *_gdi_get_frame_buffer_addr(void)
{
#if GDI_NO_PREALLOC
        return NULL;
#else
#if GDI_FB_USE_QSPI_RAM
        return (void *)((uint32_t)qspi_automode_addr(QSPI_MEM2_VIRTUAL_BASE_ADDR)
                                                                   + GDI_QSPI_RAM_OFFSET);
#else /* GDI_FB_USE_QSPI_RAM */
        return (void *)(frame_buffer);
#endif /* GDI_FB_USE_QSPI_RAM */
#endif
}

void *gdi_get_frame_buffer_addr(HW_LCDC_LAYER layer_no)
{
        void *addr = gdi->layer[layer_no].buffer[0];
        return addr;
}
void *gdi_get_gui_heap_addr(void)
{
#if GDI_NO_PREALLOC
        return NULL;
#else /* GDI_NO_PREALLOC */
#if GDI_HEAP_USE_QSPI_RAM
#if GDI_FB_USE_QSPI_RAM
        return (void *)(gdi_get_frame_buffer_addr(GUI_NUM_LAYERS == 2 ? 1 : 0)
                + GDI_FB_RESX * GDI_FB_RESY * GDI_COLOR_BYTES * GDI_SINGLE_FB_NUM);
#else /* GDI_FB_USE_QSPI_RAM */
        return (void *)((uint32_t)qspi_automode_addr(QSPI_MEM2_VIRTUAL_BASE_ADDR)
                                                                   + GDI_QSPI_RAM_OFFSET);
#endif /* GDI_FB_USE_QSPI_RAM */
#else /* GDI_HEAP_USE_QSPI_RAM */
        return (void *)(gui_heap_area);
#endif /* GDI_HEAP_USE_QSPI_RAM */
#endif /* GDI_NO_PREALLOC */
}


#endif /* dg_configLCDC_ADAPTER */
/**
 * \}
 * \}
 */
