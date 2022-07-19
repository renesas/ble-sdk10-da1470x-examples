/**
 ****************************************************************************************
 *
 * @file lv_port_disp.c
 *
 * @brief Display driver
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

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include "lv_port_gpu.h"
#include "lv_port_version.h"
#include "osal.h"
#include "gdi.h"

/*---------------------------------------------------------------------------
 * These helper macros are used to stringify a given macro */
#define STR(s)           # s
#define XSTR(s)          STR(s)

static const char lvgl_ext_versionid[] = "v" XSTR(LVGL_CONF_VERSION_MAJOR)
                                         "." XSTR(LVGL_CONF_VERSION_MINOR)
                                         "." XSTR(LVGL_CONF_VERSION_PATCH)
                                         "-" LVGL_CONF_VERSION_INFO
                                         "-" XSTR(LVGL_CONF_VERSION_EXT);

/*********************
 *      DEFINES
 *********************/
#define LAYER_OFFSET_X                  ((GDI_DISP_RESX - GDI_FB_RESX) / 2)
#define LAYER_OFFSET_Y                  ((GDI_DISP_RESY - GDI_FB_RESY) / 2)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
static void disp_rounder(struct _lv_disp_drv_t *disp_drv, lv_area_t *area);
static void disp_wait(lv_disp_drv_t *disp_drv);

#ifdef PERFORMANCE_METRICS
static void perf_monitor(lv_disp_drv_t *disp_drv, uint32_t time, uint32_t px);
#endif
/**********************
 *  STATIC VARIABLES
 **********************/
PRIVILEGED_DATA static lv_disp_draw_buf_t draw_buf_dsc;
INITIALISED_PRIVILEGED_DATA static lv_color_t *fb_addr[2] = { 0 };
PRIVILEGED_DATA static OS_EVENT flush_evt;

#if TWO_LAYERS_HORIZONTAL_SLIDING
INITIALISED_PRIVILEGED_DATA static void *flush_cb_def = NULL;
PRIVILEGED_DATA static lv_timer_t *refr_timer_def, *dummy_timer;
#endif

#ifdef PERFORMANCE_METRICS
PRIVILEGED_DATA static uint64_t flush_evt_wait;
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
        (void)(lvgl_ext_versionid);

        /*-------------------------
         * Initialize your display
         * -----------------------*/
        disp_init();

        /*-----------------------------
         * Create a buffer for drawing
         *----------------------------*/
        for (int i = 0; i < GDI_SINGLE_FB_NUM && i < ARRAY_LENGTH(fb_addr); i++) {
                fb_addr[i] = gdi_get_frame_buffer_addr(HW_LCDC_LAYER_0)
                        + i * LV_PORT_DISP_HOR_RES * LV_PORT_DISP_VER_RES * (LV_COLOR_DEPTH / 8);
        }

        /* Initialize the display buffer */
        lv_disp_draw_buf_init(&draw_buf_dsc, fb_addr[0], fb_addr[1],
                LV_PORT_DISP_HOR_RES * LV_PORT_DISP_VER_RES);

        /*-----------------------------------
         * Register the display in LVGL
         *----------------------------------*/
        static lv_disp_drv_t disp_drv;

        lv_disp_drv_init(&disp_drv);

        /* Set the resolution of the display */
        disp_drv.hor_res = LV_PORT_DISP_HOR_RES;
        disp_drv.ver_res = LV_PORT_DISP_VER_RES;

        /* Used to copy the buffer's content to the display */
        disp_drv.flush_cb = disp_flush;

        /* Set a display buffer */
        disp_drv.draw_buf = &draw_buf_dsc;

        /* Set rounder callback to modify coordinates according to LCD requirements */
        disp_drv.rounder_cb = disp_rounder;

        /* Enable task go to sleep while waiting for event */
        OS_EVENT_CREATE(flush_evt);
        disp_drv.wait_cb = disp_wait;

//        disp_drv.full_refresh = 1;

#if LV_PORT_DISP_GPU_EN
        /* Initialize GPU module */
        lv_port_gpu_init();

        /* Fill a memory array with a color with GPU */
        disp_drv.gpu_fill_cb = lv_port_gpu_fill;
        disp_drv.gpu_blit_cb = lv_port_gpu_blit;
        disp_drv.gpu_blit_with_mask_cb = lv_port_gpu_blit_with_mask;
        disp_drv.gpu_config_blit_cb = lv_port_gpu_config_blit;
        disp_drv.gpu_wait_cb = lv_port_gpu_wait;
#endif /* LV_PORT_DISP_GPU_EN */

#ifdef PERFORMANCE_METRICS
        disp_drv.monitor_cb = perf_monitor;
#endif

        /* Finally register the driver */
        lv_disp_drv_register(&disp_drv);
}

#if TWO_LAYERS_HORIZONTAL_SLIDING
void lv_port_disp_slide_init(lv_obj_t *obj, lv_dir_t dir)
{
        int dx = 0;
        lv_disp_t *disp;

        disp = lv_disp_get_default();

        if (dir == LV_DIR_RIGHT) {
                dx = -lv_disp_get_hor_res(disp);
        }
        else if (dir == LV_DIR_LEFT) {
                dx = lv_disp_get_hor_res(disp);
        }

        lv_obj_clear_state(obj, LV_STATE_SCROLLED);

        /* When partial update is enabled full redraw of watch face is required */
        lv_obj_invalidate(obj);

        /* Force a full frame update (disable partial update) */
        gdi_set_partial_update_area(LAYER_OFFSET_X + 0, LAYER_OFFSET_Y + 0,
                LAYER_OFFSET_X + lv_disp_get_hor_res(disp),
                LAYER_OFFSET_Y + lv_disp_get_ver_res(disp));

        /* Store default display driver call backs */
        flush_cb_def = *disp->driver->flush_cb;
        refr_timer_def = disp->refr_timer;

        /* Stop display update */
        disp->driver->flush_cb = NULL;

        /* Draw the whole watch face screen */
        lv_refr_now(disp);

        /* Inform the graphics library that you are ready with the flushing */
        lv_disp_flush_ready(disp->driver);

        /* Pause the refresh time to prevent any display update or frame buffer switch */
        lv_timer_pause(disp->refr_timer);

        /* Set the menu screen visible and invalidate the active screen */
        lv_obj_move_children_by(obj, dx, 0, true);
        lv_obj_invalidate(obj);

        /* Draw the menu screen in the non active frame buffer */
        lv_refr_now(disp);

        /* Inform the graphics library that you are ready with the flushing */
        lv_disp_flush_ready(disp->driver);

        /* Move back the position of the affected objects since scroll_by_raw handles it */
        lv_obj_move_children_by(obj, -dx, 0, true);

        /* Create a dummy timer to prevent drawing triggered from objects invalidation */
        dummy_timer = lv_timer_create_basic();
        disp->refr_timer = dummy_timer;
}

void lv_port_disp_slide(int posXLayer0, int posXLayer1)
{
        int posYLayer0 = 0, posYLayer1 = 0;
        lv_disp_t *disp;
        lv_disp_draw_buf_t *draw_buf;

        void *layer0_addr = NULL, *layer1_addr = NULL;
        gdi_color_fmt_t color_fmt = GDI_FORMAT_ARGB8888;

#if GDI_FB_COLOR_FORMAT == CF_NATIVE_RGB332
        color_fmt = GDI_FORMAT_RGB332;
#elif GDI_FB_COLOR_FORMAT == CF_NATIVE_RGB565
        color_fmt = GDI_FORMAT_RGB565;
#elif GDI_FB_COLOR_FORMAT == CF_NATIVE_ARGB8888
        color_fmt = GDI_FORMAT_ARGB8888;
#endif

        /* Set the position of Layer 0 in x direction */
        gdi_set_layer_start(HW_LCDC_LAYER_0, posXLayer0 + LAYER_OFFSET_X,
                posYLayer0 + LAYER_OFFSET_Y);

        /* Set the position of Layer 1 in x direction */
        gdi_set_layer_start(HW_LCDC_LAYER_1, posXLayer1 + LAYER_OFFSET_X,
                posYLayer1 + LAYER_OFFSET_Y);

        if (!gdi_get_layer_enable(HW_LCDC_LAYER_1)) {
                disp = lv_disp_get_default();
                /* Manual refresh was triggered without display update.
                 * Change the frame buffer addresses of both Layers */
                draw_buf = lv_disp_get_draw_buf(disp);
                if (draw_buf->buf1 && draw_buf->buf2) {
                        if (draw_buf->buf_act == draw_buf->buf1) {
                                layer0_addr = draw_buf->buf2;
                                layer1_addr = draw_buf->buf1;
                        }
                        else if (draw_buf->buf_act == draw_buf->buf2) {
                                layer0_addr = draw_buf->buf1;
                                layer1_addr = draw_buf->buf2;
                        }
                }

                /* Set the address of the video RAM of Layer 1 */
                gdi_set_layer_src(HW_LCDC_LAYER_1, layer1_addr, DEMO_RESX, DEMO_RESY, color_fmt);

                /* Enable the visibility of Layer 1 */
                gdi_set_layer_enable(HW_LCDC_LAYER_1, 1);

                /* Set the address of the video RAM of Layer 0 */
                gdi_set_layer_src(HW_LCDC_LAYER_0, layer0_addr, DEMO_RESX, DEMO_RESY, color_fmt);
        }

        /* Trigger an LCD display update */
        gdi_display_update_async(NULL, NULL);
}

void lv_port_disp_slide_end()
{
        lv_disp_t *disp;
        int posXLayer0 = 0, posYLayer0 = 0;
        gdi_color_fmt_t color_fmt = GDI_FORMAT_ARGB8888;

#if GDI_FB_COLOR_FORMAT == CF_NATIVE_RGB332
        color_fmt = GDI_FORMAT_RGB332;
#elif GDI_FB_COLOR_FORMAT == CF_NATIVE_RGB565
        color_fmt = GDI_FORMAT_RGB565;
#elif GDI_FB_COLOR_FORMAT == CF_NATIVE_ARGB8888
        color_fmt = GDI_FORMAT_ARGB8888;
#endif

        disp = lv_disp_get_default();

        /*Clean up invalidated areas*/
        lv_memset_00(disp->inv_areas, sizeof(disp->inv_areas));
        lv_memset_00(disp->inv_area_joined, sizeof(disp->inv_area_joined));
        disp->inv_p = 0;

        /* Re-enable display update and refresh timer callbacks */
        disp->driver->flush_cb = flush_cb_def;
        disp->refr_timer = refr_timer_def;

        lv_timer_del(dummy_timer);
        dummy_timer = NULL;

        lv_timer_resume(disp->refr_timer);

        /* Disable the visibility of Layer 1 */
        gdi_set_layer_enable(HW_LCDC_LAYER_1, 0);

        /* Reset the address of the video RAM of Layer 1 */
        gdi_set_layer_src(HW_LCDC_LAYER_1, 0, DEMO_RESX, DEMO_RESY, color_fmt);

        /* Restore the position of Layer 0 */
        gdi_set_layer_start(HW_LCDC_LAYER_0, posXLayer0 + LAYER_OFFSET_X,
                posYLayer0 + LAYER_OFFSET_Y);
}
#endif /* TWO_LAYERS_HORIZONTAL_SLIDING */

/**********************
 *   STATIC FUNCTIONS
 **********************/
/* Initialize your display and the required peripherals */
static void disp_init(void)
{
        /* Initializes the GDI instance, allocate memory and set default background color */
        gdi_init();

        /* If layer smaller than LCD, set it in the middle */
        gdi_set_layer_start(HW_LCDC_LAYER_0, LAYER_OFFSET_X, LAYER_OFFSET_Y);

        /* Make layer visible */
        gdi_set_layer_enable(HW_LCDC_LAYER_0, true);
}

static void disp_rounder(lv_disp_drv_t *disp_drv, lv_area_t *area)
{
        gdi_coord_t x0, x1, y0, y1;

        /* Transform to LCD coordinates */
        x0 = LAYER_OFFSET_X + area->x1;
        y0 = LAYER_OFFSET_Y + area->y1;
        x1 = LAYER_OFFSET_X + area->x2;
        y1 = LAYER_OFFSET_Y + area->y2;

        gdi_round_partial_update_area(&x0, &y0, &x1, &y1);

        /* Transform back to layer coordinates and ensure it fits in the layer */
        area->x1 = MAX(x0 - LAYER_OFFSET_X, 0);
        area->y1 = MAX(y0 - LAYER_OFFSET_Y, 0);
        area->x2 = MIN(x1 - LAYER_OFFSET_X, GDI_FB_RESX - 1);
        area->y2 = MIN(y1 - LAYER_OFFSET_Y, GDI_FB_RESY - 1);
}

static void flush_cb(bool underflow, void *user_data)
{
        lv_disp_drv_t *disp_drv = (lv_disp_drv_t*)user_data;

        /* Inform the graphics library that you are ready with the flushing */
        lv_disp_flush_ready(disp_drv);

        /* Trigger event in case task was blocked */
        OS_EVENT_SIGNAL(flush_evt);
}

static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
        gdi_color_fmt_t color_fmt = GDI_FORMAT_ARGB8888;

#if GDI_FB_COLOR_FORMAT == CF_NATIVE_RGB332
        color_fmt = GDI_FORMAT_RGB332;
#elif GDI_FB_COLOR_FORMAT == CF_NATIVE_RGB565
        color_fmt = GDI_FORMAT_RGB565;
#elif GDI_FB_COLOR_FORMAT == CF_NATIVE_ARGB8888
        color_fmt = GDI_FORMAT_ARGB8888;
#endif

#ifdef PERFORMANCE_METRICS
        uint64_t flush_evt_timestamp = gdi_get_sys_uptime_ticks();
#endif

#if LV_PORT_DISP_GPU_EN
        lv_port_gpu_flush();
#endif


        gdi_set_partial_update_area(LAYER_OFFSET_X + area->x1, LAYER_OFFSET_Y + area->y1,
                LAYER_OFFSET_X + area->x2, LAYER_OFFSET_Y + area->y2);

        gdi_set_layer_src(HW_LCDC_LAYER_0, color_p, lv_area_get_width(area), lv_area_get_height(area), color_fmt);
        gdi_set_layer_start(HW_LCDC_LAYER_0, LAYER_OFFSET_X + area->x1, LAYER_OFFSET_Y + area->y1);

        gdi_perf_transfer_last(lv_disp_flush_is_last(disp_drv));
        gdi_display_update_async(flush_cb, disp_drv);

#ifdef PERFORMANCE_METRICS
        flush_evt_wait += gdi_get_sys_uptime_ticks() - flush_evt_timestamp;
#endif
}

static void disp_wait(lv_disp_drv_t *disp_drv)
{
#ifdef PERFORMANCE_METRICS
        uint64_t flush_evt_timestamp = gdi_get_sys_uptime_ticks();
#endif
        OS_EVENT_WAIT(flush_evt, OS_EVENT_FOREVER);
#ifdef PERFORMANCE_METRICS
        flush_evt_wait += gdi_get_sys_uptime_ticks() - flush_evt_timestamp;
#endif
}

#ifdef PERFORMANCE_METRICS
static void perf_monitor(lv_disp_drv_t *disp_drv, uint32_t time, uint32_t px)
{
        time -= gdi_convert_ticks_to_us(flush_evt_wait) / 1000;
        flush_evt_wait = 0;
        gdi_perf_render_time(time * 1000);
}
#endif
