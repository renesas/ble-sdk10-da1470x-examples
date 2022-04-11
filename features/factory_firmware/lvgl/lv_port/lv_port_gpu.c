/**
 ****************************************************************************************
 *
 * @file lv_port_gpu.c
 *
 * @brief GPU driver
 *
 * Copyright (C) 2021-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

/*********************
 *      INCLUDES
 *********************/

#include <math.h>
#include "lvgl.h"
#include "lv_port_gpu.h"
#include "lv_port_disp.h"
#include "dave_driver.h"
#ifdef PERFORMANCE_METRICS
#include "metrics.h"
#endif

#if LV_PORT_DISP_GPU_EN
/*********************
 *      DEFINES
 *********************/
#ifndef LV_PORT_DISP_GPU_SUB_BYTE_SWAP
#define LV_PORT_DISP_GPU_SUB_BYTE_SWAP          (DLG_LVGL_CF_SUB_BYTE_SWAP)
#endif

#if GDI_FB_COLOR_FORMAT != CF_NATIVE_RGB565 && GDI_FB_COLOR_FORMAT != CF_NATIVE_ARGB8888
#error "Selected format not supported by GPU"
#endif

#if LV_COLOR_16_SWAP
#error "Selected format not supported by GPU"
#endif

#define LOG_ERRORS

#ifdef LOG_ERRORS
#define STRINGIFY(x)                            #x
#define TOSTRING(x)                             STRINGIFY(x)

#define ERROR_LIST_SIZE                         (4)
#define D2_EXEC(a)                              lv_port_gpu_log_error(a, __func__,  __LINE__)
#else
/* here is error logging not enabled */
#define D2_EXEC(a)                              a;
#endif
#define CEILING_FUNC(quotient, divisor)         (((quotient) + ((divisor) - 1)) / (divisor))

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
        d2_s32 error;
        const char *func;
        int line;
} log_error_entry;

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if LV_PORT_DISP_GPU_EN
static void lv_port_gpu_rotate_point(int *x, int *y, float cos_angle, float sin_angle, int pivot_x, int pivot_y);
static void lv_port_gpu_config_blit_clear(void);
static void lv_port_gpu_blit_internal(lv_disp_drv_t *disp_drv, const lv_area_t * dst_area, const lv_color_t *src,
        const lv_area_t * src_area, d2_u32 flags);
static void lv_port_gpu_get_recolor_consts(d2_color *cl, d2_color *ch);
static int lv_port_gpu_handle_indexed_color(const lv_color_t **src, const d2_color **clut, d2_s32 cf);
#if LV_PORT_DISP_GPU_SUB_BYTE_SWAP
static const lv_color_t *lv_port_gpu_fix_order(const lv_color_t *src, const lv_area_t * src_area, d2_s32 cf);
#endif
static int lv_port_gpu_hw_init(void);
static void lv_port_gpu_hw_deinit(void);
static d2_s32 lv_port_gpu_cf_get_default(void);
static bool lv_port_gpu_cf_is_alpha(d2_s32 cf);
static int lv_port_gpu_cf_bpp(d2_s32 cf);
static bool lv_port_gpu_cf_has_alpha(d2_s32 cf);
static d2_s32 lv_port_gpu_cf_lv_to_d2(lv_img_cf_t cf);
static bool lv_port_gpu_cf_fb_valid(d2_s32 cf);
static d2_color lv_port_gpu_color_lv_to_d2(lv_color_t color);
static void lv_port_gpu_start_render(void);
static void lv_port_gpu_execute_render(void);
static void lv_port_gpu_complete_render(void);
#ifdef LOG_ERRORS
static void lv_port_gpu_log_error(d2_s32 status, const char *func, int line);
#endif
#endif /* LV_PORT_DISP_GPU_EN */

/**********************
 *  STATIC VARIABLES
 **********************/
PRIVILEGED_DATA static bool color_key_enabled, alpha_enabled, blend_enabled, colorize_enabled;
PRIVILEGED_DATA static d2_s32 src_cf_val, dst_cf_val;
PRIVILEGED_DATA static lv_draw_img_dsc_t img_dsc;
#if LV_PORT_DISP_GPU_SUB_BYTE_SWAP
PRIVILEGED_DATA static const lv_color_t *buf;
#endif

INITIALISED_PRIVILEGED_DATA static bool d2_enabled = SCENARIO != ORIGINAL ? 1 : 0;
PRIVILEGED_DATA static d1_device *d1_handle;
PRIVILEGED_DATA static d2_device *d2_handle;
PRIVILEGED_DATA static d2_renderbuffer *renderbuffer;
#ifdef LOG_ERRORS
PRIVILEGED_DATA static log_error_entry log_error_list[ERROR_LIST_SIZE];
PRIVILEGED_DATA static int error_list_index;
PRIVILEGED_DATA static int error_count;
#endif

#ifdef PERFORMANCE_METRICS
PRIVILEGED_DATA static uint8_t metrics_tag;
#endif
#if LV_PORT_DISP_GPU_SUB_BYTE_SWAP
static const d2_color mirror_1BLUT[] =
{
        0x00000000, 0x80000000, 0x40000000, 0xc0000000, 0x20000000, 0xa0000000, 0x60000000, 0xe0000000,
        0x10000000, 0x90000000, 0x50000000, 0xd0000000, 0x30000000, 0xb0000000, 0x70000000, 0xf0000000,
        0x08000000, 0x88000000, 0x48000000, 0xc8000000, 0x28000000, 0xa8000000, 0x68000000, 0xe8000000,
        0x18000000, 0x98000000, 0x58000000, 0xd8000000, 0x38000000, 0xb8000000, 0x78000000, 0xf8000000,
        0x04000000, 0x84000000, 0x44000000, 0xc4000000, 0x24000000, 0xa4000000, 0x64000000, 0xe4000000,
        0x14000000, 0x94000000, 0x54000000, 0xd4000000, 0x34000000, 0xb4000000, 0x74000000, 0xf4000000,
        0x0c000000, 0x8c000000, 0x4c000000, 0xcc000000, 0x2c000000, 0xac000000, 0x6c000000, 0xec000000,
        0x1c000000, 0x9c000000, 0x5c000000, 0xdc000000, 0x3c000000, 0xbc000000, 0x7c000000, 0xfc000000,
        0x02000000, 0x82000000, 0x42000000, 0xc2000000, 0x22000000, 0xa2000000, 0x62000000, 0xe2000000,
        0x12000000, 0x92000000, 0x52000000, 0xd2000000, 0x32000000, 0xb2000000, 0x72000000, 0xf2000000,
        0x0a000000, 0x8a000000, 0x4a000000, 0xca000000, 0x2a000000, 0xaa000000, 0x6a000000, 0xea000000,
        0x1a000000, 0x9a000000, 0x5a000000, 0xda000000, 0x3a000000, 0xba000000, 0x7a000000, 0xfa000000,
        0x06000000, 0x86000000, 0x46000000, 0xc6000000, 0x26000000, 0xa6000000, 0x66000000, 0xe6000000,
        0x16000000, 0x96000000, 0x56000000, 0xd6000000, 0x36000000, 0xb6000000, 0x76000000, 0xf6000000,
        0x0e000000, 0x8e000000, 0x4e000000, 0xce000000, 0x2e000000, 0xae000000, 0x6e000000, 0xee000000,
        0x1e000000, 0x9e000000, 0x5e000000, 0xde000000, 0x3e000000, 0xbe000000, 0x7e000000, 0xfe000000,
        0x01000000, 0x81000000, 0x41000000, 0xc1000000, 0x21000000, 0xa1000000, 0x61000000, 0xe1000000,
        0x11000000, 0x91000000, 0x51000000, 0xd1000000, 0x31000000, 0xb1000000, 0x71000000, 0xf1000000,
        0x09000000, 0x89000000, 0x49000000, 0xc9000000, 0x29000000, 0xa9000000, 0x69000000, 0xe9000000,
        0x19000000, 0x99000000, 0x59000000, 0xd9000000, 0x39000000, 0xb9000000, 0x79000000, 0xf9000000,
        0x05000000, 0x85000000, 0x45000000, 0xc5000000, 0x25000000, 0xa5000000, 0x65000000, 0xe5000000,
        0x15000000, 0x95000000, 0x55000000, 0xd5000000, 0x35000000, 0xb5000000, 0x75000000, 0xf5000000,
        0x0d000000, 0x8d000000, 0x4d000000, 0xcd000000, 0x2d000000, 0xad000000, 0x6d000000, 0xed000000,
        0x1d000000, 0x9d000000, 0x5d000000, 0xdd000000, 0x3d000000, 0xbd000000, 0x7d000000, 0xfd000000,
        0x03000000, 0x83000000, 0x43000000, 0xc3000000, 0x23000000, 0xa3000000, 0x63000000, 0xe3000000,
        0x13000000, 0x93000000, 0x53000000, 0xd3000000, 0x33000000, 0xb3000000, 0x73000000, 0xf3000000,
        0x0b000000, 0x8b000000, 0x4b000000, 0xcb000000, 0x2b000000, 0xab000000, 0x6b000000, 0xeb000000,
        0x1b000000, 0x9b000000, 0x5b000000, 0xdb000000, 0x3b000000, 0xbb000000, 0x7b000000, 0xfb000000,
        0x07000000, 0x87000000, 0x47000000, 0xc7000000, 0x27000000, 0xa7000000, 0x67000000, 0xe7000000,
        0x17000000, 0x97000000, 0x57000000, 0xd7000000, 0x37000000, 0xb7000000, 0x77000000, 0xf7000000,
        0x0f000000, 0x8f000000, 0x4f000000, 0xcf000000, 0x2f000000, 0xaf000000, 0x6f000000, 0xef000000,
        0x1f000000, 0x9f000000, 0x5f000000, 0xdf000000, 0x3f000000, 0xbf000000, 0x7f000000, 0xff000000,
};

static const d2_color mirror_2BLUT[] =
{
        0x00000000, 0x40000000, 0x80000000, 0xC0000000, 0x10000000, 0x50000000, 0x90000000, 0xD0000000,
        0x20000000, 0x60000000, 0xA0000000, 0xE0000000, 0x30000000, 0x70000000, 0xB0000000, 0xF0000000,
        0x04000000, 0x44000000, 0x84000000, 0xC4000000, 0x14000000, 0x54000000, 0x94000000, 0xD4000000,
        0x24000000, 0x64000000, 0xA4000000, 0xE4000000, 0x34000000, 0x74000000, 0xB4000000, 0xF4000000,
        0x08000000, 0x48000000, 0x88000000, 0xC8000000, 0x18000000, 0x58000000, 0x98000000, 0xD8000000,
        0x28000000, 0x68000000, 0xA8000000, 0xE8000000, 0x38000000, 0x78000000, 0xB8000000, 0xF8000000,
        0x0C000000, 0x4C000000, 0x8C000000, 0xCC000000, 0x1C000000, 0x5C000000, 0x9C000000, 0xDC000000,
        0x2C000000, 0x6C000000, 0xAC000000, 0xEC000000, 0x3C000000, 0x7C000000, 0xBC000000, 0xFC000000,
        0x01000000, 0x41000000, 0x81000000, 0xC1000000, 0x11000000, 0x51000000, 0x91000000, 0xD1000000,
        0x21000000, 0x61000000, 0xA1000000, 0xE1000000, 0x31000000, 0x71000000, 0xB1000000, 0xF1000000,
        0x05000000, 0x45000000, 0x85000000, 0xC5000000, 0x15000000, 0x55000000, 0x95000000, 0xD5000000,
        0x25000000, 0x65000000, 0xA5000000, 0xE5000000, 0x35000000, 0x75000000, 0xB5000000, 0xF5000000,
        0x09000000, 0x49000000, 0x89000000, 0xC9000000, 0x19000000, 0x59000000, 0x99000000, 0xD9000000,
        0x29000000, 0x69000000, 0xA9000000, 0xE9000000, 0x39000000, 0x79000000, 0xB9000000, 0xF9000000,
        0x0D000000, 0x4D000000, 0x8D000000, 0xCD000000, 0x1D000000, 0x5D000000, 0x9D000000, 0xDD000000,
        0x2D000000, 0x6D000000, 0xAD000000, 0xED000000, 0x3D000000, 0x7D000000, 0xBD000000, 0xFD000000,
        0x02000000, 0x42000000, 0x82000000, 0xC2000000, 0x12000000, 0x52000000, 0x92000000, 0xD2000000,
        0x22000000, 0x62000000, 0xA2000000, 0xE2000000, 0x32000000, 0x72000000, 0xB2000000, 0xF2000000,
        0x06000000, 0x46000000, 0x86000000, 0xC6000000, 0x16000000, 0x56000000, 0x96000000, 0xD6000000,
        0x26000000, 0x66000000, 0xA6000000, 0xE6000000, 0x36000000, 0x76000000, 0xB6000000, 0xF6000000,
        0x0A000000, 0x4A000000, 0x8A000000, 0xCA000000, 0x1A000000, 0x5A000000, 0x9A000000, 0xDA000000,
        0x2A000000, 0x6A000000, 0xAA000000, 0xEA000000, 0x3A000000, 0x7A000000, 0xBA000000, 0xFA000000,
        0x0E000000, 0x4E000000, 0x8E000000, 0xCE000000, 0x1E000000, 0x5E000000, 0x9E000000, 0xDE000000,
        0x2E000000, 0x6E000000, 0xAE000000, 0xEE000000, 0x3E000000, 0x7E000000, 0xBE000000, 0xFE000000,
        0x03000000, 0x43000000, 0x83000000, 0xC3000000, 0x13000000, 0x53000000, 0x93000000, 0xD3000000,
        0x23000000, 0x63000000, 0xA3000000, 0xE3000000, 0x33000000, 0x73000000, 0xB3000000, 0xF3000000,
        0x07000000, 0x47000000, 0x87000000, 0xC7000000, 0x17000000, 0x57000000, 0x97000000, 0xD7000000,
        0x27000000, 0x67000000, 0xA7000000, 0xE7000000, 0x37000000, 0x77000000, 0xB7000000, 0xF7000000,
        0x0B000000, 0x4B000000, 0x8B000000, 0xCB000000, 0x1B000000, 0x5B000000, 0x9B000000, 0xDB000000,
        0x2B000000, 0x6B000000, 0xAB000000, 0xEB000000, 0x3B000000, 0x7B000000, 0xBB000000, 0xFB000000,
        0x0F000000, 0x4F000000, 0x8F000000, 0xCF000000, 0x1F000000, 0x5F000000, 0x9F000000, 0xDF000000,
        0x2F000000, 0x6F000000, 0xAF000000, 0xEF000000, 0x3F000000, 0x7F000000, 0xBF000000, 0xFF000000,
};

static const d2_color mirror_4BLUT[] =
{
        0x00000000, 0x10000000, 0x20000000, 0x30000000, 0x40000000, 0x50000000, 0x60000000, 0x70000000,
        0x80000000, 0x90000000, 0xA0000000, 0xB0000000, 0xC0000000, 0xD0000000, 0xE0000000, 0xF0000000,
        0x01000000, 0x11000000, 0x21000000, 0x31000000, 0x41000000, 0x51000000, 0x61000000, 0x71000000,
        0x81000000, 0x91000000, 0xA1000000, 0xB1000000, 0xC1000000, 0xD1000000, 0xE1000000, 0xF1000000,
        0x02000000, 0x12000000, 0x22000000, 0x32000000, 0x42000000, 0x52000000, 0x62000000, 0x72000000,
        0x82000000, 0x92000000, 0xA2000000, 0xB2000000, 0xC2000000, 0xD2000000, 0xE2000000, 0xF2000000,
        0x03000000, 0x13000000, 0x23000000, 0x33000000, 0x43000000, 0x53000000, 0x63000000, 0x73000000,
        0x83000000, 0x93000000, 0xA3000000, 0xB3000000, 0xC3000000, 0xD3000000, 0xE3000000, 0xF3000000,
        0x04000000, 0x14000000, 0x24000000, 0x34000000, 0x44000000, 0x54000000, 0x64000000, 0x74000000,
        0x84000000, 0x94000000, 0xA4000000, 0xB4000000, 0xC4000000, 0xD4000000, 0xE4000000, 0xF4000000,
        0x05000000, 0x15000000, 0x25000000, 0x35000000, 0x45000000, 0x55000000, 0x65000000, 0x75000000,
        0x85000000, 0x95000000, 0xA5000000, 0xB5000000, 0xC5000000, 0xD5000000, 0xE5000000, 0xF5000000,
        0x06000000, 0x16000000, 0x26000000, 0x36000000, 0x46000000, 0x56000000, 0x66000000, 0x76000000,
        0x86000000, 0x96000000, 0xA6000000, 0xB6000000, 0xC6000000, 0xD6000000, 0xE6000000, 0xF6000000,
        0x07000000, 0x17000000, 0x27000000, 0x37000000, 0x47000000, 0x57000000, 0x67000000, 0x77000000,
        0x87000000, 0x97000000, 0xA7000000, 0xB7000000, 0xC7000000, 0xD7000000, 0xE7000000, 0xF7000000,
        0x08000000, 0x18000000, 0x28000000, 0x38000000, 0x48000000, 0x58000000, 0x68000000, 0x78000000,
        0x88000000, 0x98000000, 0xA8000000, 0xB8000000, 0xC8000000, 0xD8000000, 0xE8000000, 0xF8000000,
        0x09000000, 0x19000000, 0x29000000, 0x39000000, 0x49000000, 0x59000000, 0x69000000, 0x79000000,
        0x89000000, 0x99000000, 0xA9000000, 0xB9000000, 0xC9000000, 0xD9000000, 0xE9000000, 0xF9000000,
        0x0A000000, 0x1A000000, 0x2A000000, 0x3A000000, 0x4A000000, 0x5A000000, 0x6A000000, 0x7A000000,
        0x8A000000, 0x9A000000, 0xAA000000, 0xBA000000, 0xCA000000, 0xDA000000, 0xEA000000, 0xFA000000,
        0x0B000000, 0x1B000000, 0x2B000000, 0x3B000000, 0x4B000000, 0x5B000000, 0x6B000000, 0x7B000000,
        0x8B000000, 0x9B000000, 0xAB000000, 0xBB000000, 0xCB000000, 0xDB000000, 0xEB000000, 0xFB000000,
        0x0C000000, 0x1C000000, 0x2C000000, 0x3C000000, 0x4C000000, 0x5C000000, 0x6C000000, 0x7C000000,
        0x8C000000, 0x9C000000, 0xAC000000, 0xBC000000, 0xCC000000, 0xDC000000, 0xEC000000, 0xFC000000,
        0x0D000000, 0x1D000000, 0x2D000000, 0x3D000000, 0x4D000000, 0x5D000000, 0x6D000000, 0x7D000000,
        0x8D000000, 0x9D000000, 0xAD000000, 0xBD000000, 0xCD000000, 0xDD000000, 0xED000000, 0xFD000000,
        0x0E000000, 0x1E000000, 0x2E000000, 0x3E000000, 0x4E000000, 0x5E000000, 0x6E000000, 0x7E000000,
        0x8E000000, 0x9E000000, 0xAE000000, 0xBE000000, 0xCE000000, 0xDE000000, 0xEE000000, 0xFE000000,
        0x0F000000, 0x1F000000, 0x2F000000, 0x3F000000, 0x4F000000, 0x5F000000, 0x6F000000, 0x7F000000,
        0x8F000000, 0x9F000000, 0xAF000000, 0xBF000000, 0xCF000000, 0xDF000000, 0xEF000000, 0xFF000000,
};
#endif /* LV_PORT_DISP_GPU_SUB_BYTE_SWAP */

void lv_port_gpu_init(void)
{
        lv_port_gpu_config_blit_clear();
}

void lv_port_gpu_fill(lv_disp_drv_t *disp_drv, lv_color_t *dst, lv_coord_t dst_width,
        const lv_area_t *fill_area, lv_color_t color, lv_opa_t opa)
{
        lv_port_gpu_start_render();

        D2_EXEC(d2_framebuffer(d2_handle, d1_maptovidmem(d1_handle, dst), MAX(dst_width, 2), MAX(dst_width, 2),
                MAX(fill_area->y2 + 1, 2), lv_port_gpu_cf_get_default()));
        D2_EXEC(d2_cliprect(d2_handle, 0, 0, dst_width - 1, fill_area->y2));

        D2_EXEC(d2_setalpha(d2_handle, opa > LV_OPA_MAX ? 0xFF : opa));
        D2_EXEC(d2_setcolor(d2_handle, 0, lv_port_gpu_color_lv_to_d2(color)));
        D2_EXEC(d2_renderbox(d2_handle, D2_FIX4(fill_area->x1), D2_FIX4(fill_area->y1),
                D2_FIX4(lv_area_get_width(fill_area)), D2_FIX4(lv_area_get_height(fill_area))));

#ifdef PERFORMANCE_METRICS
        metrics_tag = GPU_METRICS_FILL;
#endif
        lv_port_gpu_execute_render();
}

static void lv_port_gpu_rotate_point(int *x, int *y, float cos_angle, float sin_angle, int pivot_x, int pivot_y)
{
        float fx, fy;

        *x -= pivot_x;
        *y -= pivot_y;

        fx = ((float)*x) / 16.0f;
        fy = ((float)*y) / 16.0f;

        *x = (int)(((fx * cos_angle) - (fy * sin_angle)) * 16.0f);
        *y = (int)(((fx * sin_angle) + (fy * cos_angle)) * 16.0f);

        *x += pivot_x;
        *y += pivot_y;
}

static void lv_port_gpu_get_recolor_consts(d2_color *cl, d2_color *ch)
{
        d2_color c = lv_port_gpu_color_lv_to_d2(img_dsc.recolor);
        d2_alpha r, g, b, opa = img_dsc.recolor_opa > LV_OPA_MAX ? LV_OPA_COVER : img_dsc.recolor_opa;

        r = ((uint32_t)((uint8_t)(c >> GDI_F_COLOR_POS(ARGB8888, RED)  )) * opa) / 255;
        g = ((uint32_t)((uint8_t)(c >> GDI_F_COLOR_POS(ARGB8888, GREEN))) * opa) / 255;
        b = ((uint32_t)((uint8_t)(c >> GDI_F_COLOR_POS(ARGB8888, BLUE) )) * opa) / 255;
        *cl = r << GDI_F_COLOR_POS(ARGB8888, RED) | g << GDI_F_COLOR_POS(ARGB8888, GREEN) | b << GDI_F_COLOR_POS(ARGB8888, BLUE);

        r += 255 - opa;
        g += 255 - opa;
        b += 255 - opa;
        *ch = r << GDI_F_COLOR_POS(ARGB8888, RED) | g << GDI_F_COLOR_POS(ARGB8888, GREEN) | b << GDI_F_COLOR_POS(ARGB8888, BLUE);
}

static int lv_port_gpu_handle_indexed_color(const lv_color_t **src, const d2_color **clut, d2_s32 cf)
{
        int clut_len = 0;

        if (cf & d2_mode_clut) {
                /* Calculate CLUT length in entries */
                switch (cf & ~(d2_mode_clut | d2_mode_rle)) {
                case d2_mode_i1:   clut_len = 2;   break;
                case d2_mode_i2:   clut_len = 4;   break;
                case d2_mode_i4:   clut_len = 16;  break;
                case d2_mode_i8:   clut_len = 256; break;
                case d2_mode_ai44: clut_len = 16;  break;
                default:         return 0;
                }

                *clut = (const d2_color *)*src;
                *src = (const lv_color_t *)((const uint32_t *)*src + clut_len);
        }
        return clut_len;
}

static void lv_port_gpu_blit_internal(lv_disp_drv_t *disp_drv, const lv_area_t * dst_area, const lv_color_t *src,
        const lv_area_t * src_area, d2_u32 flags)
{
        const lv_area_t *img_area = src_area;
        lv_area_t img_area_scaled;
        lv_coord_t w, h, img_w, img_h;
        d2_s32 pitch;
        int bpp = lv_port_gpu_cf_bpp(src_cf_val);

        D2_EXEC(d2_cliprect(d2_handle, dst_area->x1, dst_area->y1, dst_area->x2, dst_area->y2));

        pitch = w = lv_area_get_width(src_area);
        h = lv_area_get_height(src_area);

        if (img_dsc.zoom != LV_IMG_ZOOM_NONE) {
                img_area_scaled.x1 = src_area->x1 + ((((int32_t)0 - img_dsc.pivot.x) * img_dsc.zoom) >> 8) + img_dsc.pivot.x;
                img_area_scaled.x2 = src_area->x1 + ((((int32_t)w - img_dsc.pivot.x) * img_dsc.zoom) >> 8) + img_dsc.pivot.x;
                img_area_scaled.y1 = src_area->y1 + ((((int32_t)0 - img_dsc.pivot.y) * img_dsc.zoom) >> 8) + img_dsc.pivot.y;
                img_area_scaled.y2 = src_area->y1 + ((((int32_t)h - img_dsc.pivot.y) * img_dsc.zoom) >> 8) + img_dsc.pivot.y;
                img_area = &img_area_scaled;
        }

        img_w = lv_area_get_width(img_area);
        img_h = lv_area_get_height(img_area);


        if (0 < bpp && bpp < 8) {
                pitch = (w + (8 - bpp)) & (~(8 - bpp));
        }

        if (img_dsc.angle == 0) {
                D2_EXEC(d2_setblitsrc(d2_handle, d1_maptovidmem(d1_handle, (void *) src), pitch, w, h, src_cf_val));

                D2_EXEC(d2_blitcopy(d2_handle, w, h, 0, 0,
                        D2_FIX4(img_w), D2_FIX4(img_h), D2_FIX4(img_area->x1), D2_FIX4(img_area->y1), flags));

#ifdef PERFORMANCE_METRICS
                metrics_tag = GPU_METRICS_BLITBITMAP;
#endif
        } else {
                int x, y, x1, y1, x2, y2, x3, y3, x4, y4, dxu, dxv, dyu, dyv, xx, xy, yx, yy;
                int pivot_scaled_x, pivot_scaled_y;
                int tex_offset = (flags & d2_bf_filter) ? -32767 : 0;
                d2_u8 amode, cmode = d2_to_copy;
                float angle = ((float)img_dsc.angle / 10) * M_PI / 180;
                float cos_angle = cosf(angle);
                float sin_angle = sinf(angle);
                d2_u8 fillmode_backup;

                /* setup texture params */
                fillmode_backup = d2_getfillmode(d2_handle);
                D2_EXEC(d2_setfillmode(d2_handle, d2_fm_texture));
                D2_EXEC(d2_settexture(d2_handle, d1_maptovidmem(d1_handle, (void *) src), pitch, w, h, src_cf_val));
                D2_EXEC(d2_settexturemode(d2_handle, flags & (d2_bf_filter | d2_bf_wrap)));
                amode = flags & d2_bf_usealpha ? d2_to_copy : d2_to_one;
                cmode = flags & d2_bf_colorize2 ? d2_to_blend : d2_to_copy;
                D2_EXEC(d2_settextureoperation(d2_handle, amode, cmode, cmode, cmode));
                if (flags & d2_bf_colorize2) {
                        d2_color cl = d2_getcolor(d2_handle, 0), ch = d2_getcolor(d2_handle, 1);
                        D2_EXEC(d2_settexopparam(d2_handle, d2_cc_red,   (uint8_t)(cl >> GDI_F_COLOR_POS(ARGB8888, RED)),
                                (uint8_t)(ch >> GDI_F_COLOR_POS(ARGB8888, RED))));
                        D2_EXEC(d2_settexopparam(d2_handle, d2_cc_green, (uint8_t)(cl >> GDI_F_COLOR_POS(ARGB8888, GREEN)),
                                (uint8_t)(ch >> GDI_F_COLOR_POS(ARGB8888, GREEN))));
                        D2_EXEC(d2_settexopparam(d2_handle, d2_cc_blue,  (uint8_t)(cl >> GDI_F_COLOR_POS(ARGB8888, BLUE)),
                                (uint8_t)(ch >> GDI_F_COLOR_POS(ARGB8888, BLUE))));
                }

                x = D2_FIX4(img_area->x1); y = D2_FIX4(img_area->y1);

                /* define quad points */
                x1 = D2_FIX4(0);     y1 = D2_FIX4(0);
                x2 = D2_FIX4(img_w); y2 = D2_FIX4(0);
                x3 = D2_FIX4(img_w); y3 = D2_FIX4(img_h);
                x4 = D2_FIX4(0);     y4 = D2_FIX4(img_h);

                /* rotate points for quad */
                pivot_scaled_x = (img_dsc.pivot.x * img_dsc.zoom) >> 4;
                pivot_scaled_y = (img_dsc.pivot.y * img_dsc.zoom) >> 4;

                lv_port_gpu_rotate_point(&x1, &y1, cos_angle, sin_angle, pivot_scaled_x, pivot_scaled_y);
                lv_port_gpu_rotate_point(&x2, &y2, cos_angle, sin_angle, pivot_scaled_x, pivot_scaled_y);
                lv_port_gpu_rotate_point(&x3, &y3, cos_angle, sin_angle, pivot_scaled_x, pivot_scaled_y);
                lv_port_gpu_rotate_point(&x4, &y4, cos_angle, sin_angle, pivot_scaled_x, pivot_scaled_y);

                /* compute texture increments */
                xx = (int)(cos_angle * 65536.0f);
                xy = (int)(sin_angle * 65536.0f);
                yx = (int)(-sin_angle * 65536.0f);
                yy = (int)(cos_angle * 65536.0f);
                dxu = ((D2_FIX16(w) / D2_FIX4(img_w)) * xx) >> 12;
                dxv = ((D2_FIX16(w) / D2_FIX4(img_w)) * xy) >> 12;
                dyu = ((D2_FIX16(h) / D2_FIX4(img_h)) * yx) >> 12;
                dyv = ((D2_FIX16(h) / D2_FIX4(img_h)) * yy) >> 12;

                /* map texture exactly to rotated quad, so texel center is always (0/0) top-left */
                D2_EXEC(d2_settexelcenter(d2_handle, 0, 0));
                D2_EXEC(d2_settexturemapping(d2_handle, (d2_point )(x + x1), (d2_point )(y + y1),
                        tex_offset, tex_offset, dxu, dxv, dyu, dyv));

                int minx = MAX(dst_area->x1, D2_INT4(x + MIN(x1, MIN(x2, MIN(x3, x4)))));
                int maxx = MIN(dst_area->x2, D2_INT4(x + MAX(x1, MAX(x2, MAX(x3, x4)))));
                int slice = (flags & d2_bf_filter) ? 6 : 8;

                /* Perform render operation in slices to acheive better performance */
                for (int posx = minx; posx < maxx; posx += slice) {
                        D2_EXEC(d2_cliprect(d2_handle, posx, dst_area->y1, MIN(posx + slice - 1, maxx), dst_area->y2));
                        D2_EXEC(d2_renderquad(d2_handle, (d2_point )(x + x1), (d2_point )(y + y1),
                                                         (d2_point )(x + x2), (d2_point )(y + y2),
                                                         (d2_point )(x + x3), (d2_point )(y + y3),
                                                         (d2_point )(x + x4), (d2_point )(y + y4), 0));
                }
                D2_EXEC(d2_setfillmode(d2_handle, fillmode_backup));

#ifdef PERFORMANCE_METRICS
                metrics_tag = GPU_METRICS_ROTATEIMAGE;
#endif
        }
}

void lv_port_gpu_blit(lv_disp_drv_t *disp_drv, lv_color_t *dst, const lv_area_t * dst_area, lv_coord_t dst_pitch,
        const lv_color_t *src, const lv_area_t * src_area, lv_opa_t opa)
{
        d2_u32 flags = 0;
        const d2_color *clut = NULL;
        int clut_len = 0;

        clut_len = lv_port_gpu_handle_indexed_color(&src, &clut, src_cf_val);

#if LV_PORT_DISP_GPU_SUB_BYTE_SWAP
        src = lv_port_gpu_fix_order(src, src_area, src_cf_val);
#endif

        lv_port_gpu_start_render();

        D2_EXEC(d2_framebuffer(d2_handle, d1_maptovidmem(d1_handle, dst), MAX(dst_pitch, 2),
                MAX(dst_area->x2 + 1, 2), MAX(dst_area->y2 + 1, 2), dst_cf_val));

        flags |= alpha_enabled ? d2_bf_usealpha : 0;

        D2_EXEC(d2_setalpha(d2_handle, opa > LV_OPA_MAX ? LV_OPA_COVER : opa));

        if (clut) {
                D2_EXEC(d2_writetexclut_direct(d2_handle, clut, 0, clut_len));
        }

        flags |= color_key_enabled ? d2_bf_usealpha : 0;
        D2_EXEC(d2_setcolorkey(d2_handle, color_key_enabled, lv_port_gpu_color_lv_to_d2(disp_drv->color_chroma_key)));

        flags |= (colorize_enabled || img_dsc.recolor_opa != LV_OPA_TRANSP) ? d2_bf_colorize2 : 0;
        if (colorize_enabled) {
                D2_EXEC(d2_setcolor(d2_handle, 0, lv_port_gpu_color_lv_to_d2(img_dsc.recolor)));
                D2_EXEC(d2_setcolor(d2_handle, 1, lv_port_gpu_color_lv_to_d2(img_dsc.recolor)));
        } else if(img_dsc.recolor_opa != LV_OPA_TRANSP) {
                d2_color cl = 0, ch = 0;
                lv_port_gpu_get_recolor_consts(&cl, &ch);
                D2_EXEC(d2_setcolor(d2_handle, 0, cl));
                D2_EXEC(d2_setcolor(d2_handle, 1, ch));
        }

        flags |= ((img_dsc.angle || img_dsc.zoom != LV_IMG_ZOOM_NONE) && img_dsc.antialias) ? d2_bf_filter : 0;

        if (blend_enabled) {
                D2_EXEC(d2_setblendmode(d2_handle, d2_bm_alpha,
                        img_dsc.blend_mode != LV_BLEND_MODE_NORMAL ? d2_bm_one : d2_bm_one_minus_alpha));
                D2_EXEC(d2_setalphablendmode(d2_handle, d2_bm_one, d2_bm_one_minus_alpha));
        } else {
                D2_EXEC(d2_setblendmode(d2_handle, d2_bm_one, d2_bm_zero));
                D2_EXEC(d2_setalphablendmode(d2_handle, d2_bm_one, d2_bm_zero));
        }

        lv_port_gpu_blit_internal(disp_drv, dst_area, src, src_area, flags);

        lv_port_gpu_execute_render();
}

void lv_port_gpu_blit_with_mask(lv_disp_drv_t *disp_drv, lv_color_t *dst, const lv_area_t * dst_area, lv_coord_t dst_pitch,
        const lv_color_t *src, const lv_area_t * src_area, const lv_opa_t *msk, uint32_t *work_buf, lv_opa_t opa)
{
        d2_u32 flags = 0;
        lv_area_t buf_area, src_area_rel;
        const d2_color *clut = NULL;
        int clut_len = 0;
        d2_s32 cf = src_cf_val;
        uint16_t angle = img_dsc.angle;
        uint16_t zoom = img_dsc.zoom;

        clut_len = lv_port_gpu_handle_indexed_color(&src, &clut, src_cf_val);

#if LV_PORT_DISP_GPU_SUB_BYTE_SWAP
        src = lv_port_gpu_fix_order(src, src_area, src_cf_val);
#endif

        lv_port_gpu_start_render();

        /*
         * First step: BLIT image in temporary buffer
         */
        /* Move areas to the origin */
        buf_area.x1 = 0;
        buf_area.y1 = 0;
        buf_area.x2 = dst_area->x2 - dst_area->x1;
        buf_area.y2 = dst_area->y2 - dst_area->y1;

        lv_area_copy(&src_area_rel, src_area);
        lv_area_move(&src_area_rel, -dst_area->x1, -dst_area->y1);

        D2_EXEC(d2_framebuffer(d2_handle, d1_maptovidmem(d1_handle, work_buf), MAX(lv_area_get_width(&buf_area), 2),
                MAX(lv_area_get_width(&buf_area), 2), MAX(lv_area_get_height(&buf_area), 2), d2_mode_argb8888));

        flags |= alpha_enabled ? d2_bf_usealpha : 0;

        D2_EXEC(d2_setalpha(d2_handle, opa > LV_OPA_MAX ? LV_OPA_COVER : opa));

        if (clut) {
                D2_EXEC(d2_writetexclut_direct(d2_handle, clut, 0, clut_len));
        }

        flags |= color_key_enabled ? d2_bf_usealpha : 0;
        D2_EXEC(d2_setcolorkey(d2_handle, color_key_enabled, lv_port_gpu_color_lv_to_d2(disp_drv->color_chroma_key)));

        flags |= (colorize_enabled || img_dsc.recolor_opa != LV_OPA_TRANSP) ? d2_bf_colorize2 : 0;
        if (colorize_enabled) {
                D2_EXEC(d2_setcolor(d2_handle, 0, lv_port_gpu_color_lv_to_d2(img_dsc.recolor)));
                D2_EXEC(d2_setcolor(d2_handle, 1, lv_port_gpu_color_lv_to_d2(img_dsc.recolor)));
        } else if(img_dsc.recolor_opa != LV_OPA_TRANSP) {
                d2_color cl = 0, ch = 0;
                lv_port_gpu_get_recolor_consts(&cl, &ch);
                D2_EXEC(d2_setcolor(d2_handle, 0, cl));
                D2_EXEC(d2_setcolor(d2_handle, 1, ch));
        }

        flags |= ((img_dsc.angle || img_dsc.zoom != LV_IMG_ZOOM_NONE) && img_dsc.antialias) ? d2_bf_filter : 0;

        /* Do not blend, simple overlay */
        D2_EXEC(d2_setblendmode(d2_handle, d2_bm_one, d2_bm_zero));
        D2_EXEC(d2_setalphablendmode(d2_handle, d2_bm_one, d2_bm_zero));

        lv_port_gpu_blit_internal(disp_drv, &buf_area, src, &src_area_rel, flags);

        /*
         * Second step: Mix alpha in temporary buffer with mask
         */
        flags = d2_bf_usealpha;

        img_dsc.angle = 0;
        img_dsc.zoom = LV_IMG_ZOOM_NONE;
        src_cf_val = d2_mode_alpha8;

        D2_EXEC(d2_setalpha(d2_handle, 0xFF));

        D2_EXEC(d2_setblendmode(d2_handle, d2_bm_zero, d2_bm_one));
        D2_EXEC(d2_setalphablendmode(d2_handle, d2_bm_zero, d2_bm_alpha));

        lv_port_gpu_blit_internal(disp_drv, &buf_area, (const lv_color_t *)msk, &buf_area, flags);

        lv_port_gpu_execute_render();
        lv_port_gpu_complete_render();
        lv_port_gpu_start_render();

        /*
         * Third step: BLIT temporary buffer as image
         */
        flags = d2_bf_usealpha;
        src_cf_val = d2_mode_argb8888;

        D2_EXEC(d2_framebuffer(d2_handle, d1_maptovidmem(d1_handle, dst), MAX(dst_pitch, 2),
                MAX(dst_area->x2 + 1, 2), MAX(dst_area->y2 + 1, 2), dst_cf_val));

        if (blend_enabled) {
                D2_EXEC(d2_setblendmode(d2_handle, d2_bm_alpha,
                        img_dsc.blend_mode != LV_BLEND_MODE_NORMAL ? d2_bm_one : d2_bm_one_minus_alpha));
                D2_EXEC(d2_setalphablendmode(d2_handle, d2_bm_one, d2_bm_one_minus_alpha));
        } else {
                D2_EXEC(d2_setblendmode(d2_handle, d2_bm_one, d2_bm_zero));
                D2_EXEC(d2_setalphablendmode(d2_handle, d2_bm_one, d2_bm_zero));
        }
        lv_port_gpu_blit_internal(disp_drv, dst_area, (const lv_color_t *)work_buf, dst_area, flags);

        /* Restore modified image settings */
        src_cf_val = cf;
        img_dsc.angle = angle;
        img_dsc.zoom = zoom;

        lv_port_gpu_execute_render();
}

bool lv_port_gpu_config_blit(lv_disp_drv_t *disp_drv, const lv_draw_img_dsc_t *draw_dsc,  lv_img_cf_t dst_cf,
        lv_img_cf_t src_cf, bool alpha_en, bool color_key_en, bool blend_en, bool colorize_en)
{
        d2_s32 d2_src_cf, d2_dst_cf;

#if !DLG_LVGL_CF
        if (alpha_en && LV_COLOR_DEPTH != 32 && src_cf == LV_IMG_CF_TRUE_COLOR) {
                return false;
        }
#endif /* !DLG_LVGL_CF */

        if (blend_en && draw_dsc->blend_mode != LV_BLEND_MODE_NORMAL
                && draw_dsc->blend_mode != LV_BLEND_MODE_ADDITIVE)
        {
                return false;
        }

        d2_src_cf = lv_port_gpu_cf_lv_to_d2(src_cf);
        d2_dst_cf = lv_port_gpu_cf_lv_to_d2(dst_cf);
        if (d2_src_cf < 0 || !lv_port_gpu_cf_fb_valid(d2_dst_cf)) {
                return false;
        }
        src_cf_val = d2_src_cf;
        dst_cf_val = d2_dst_cf;

        img_dsc = *draw_dsc;

        /* Disable alpha if alpha channel does not exist */
        alpha_enabled = lv_port_gpu_cf_has_alpha(src_cf_val) ? alpha_en : 0;
        color_key_enabled = color_key_en;
        blend_enabled = blend_en;
        colorize_enabled = colorize_en | lv_port_gpu_cf_is_alpha(src_cf_val);

        return true;
}

static void lv_port_gpu_config_blit_clear(void)
{
        alpha_enabled = false;
        color_key_enabled = false;
        blend_enabled = true;
        colorize_enabled = false;

        lv_draw_img_dsc_init(&img_dsc);

        src_cf_val = lv_port_gpu_cf_get_default();
        dst_cf_val = lv_port_gpu_cf_get_default();
}

void lv_port_gpu_render_box(lv_disp_drv_t *disp_drv,  lv_color_t *dst, lv_coord_t dst_pitch,
        lv_coord_t x, lv_coord_t y, lv_coord_t w, lv_coord_t h, lv_color_t color)
{
        lv_port_gpu_start_render();

        D2_EXEC(d2_framebuffer(d2_handle, d1_maptovidmem(d1_handle, dst), MAX(dst_pitch, 2),
                MAX(x + w, 2), MAX(y + h, 2), lv_port_gpu_cf_get_default()));
        D2_EXEC(d2_cliprect(d2_handle, 0, 0, x + w - 1, y + h - 1));

        D2_EXEC(d2_setcolor(d2_handle, 0, lv_port_gpu_color_lv_to_d2(color)));

        x = D2_FIX4(x);
        y = D2_FIX4(y);
        w = D2_FIX4(w - 1);
        h = D2_FIX4(h - 1);

        D2_EXEC(d2_renderline(d2_handle, x,     y,     x + w, y,     D2_FIX4(1), 0));
        D2_EXEC(d2_renderline(d2_handle, x,     y,     x,     y + h, D2_FIX4(1), 0));
        D2_EXEC(d2_renderline(d2_handle, x + w, y,     x + w, y + h, D2_FIX4(1), 0));
        D2_EXEC(d2_renderline(d2_handle, x,     y + h, x + w, y + h, D2_FIX4(1), 0));

        lv_port_gpu_execute_render();
}

#if LV_PORT_DISP_GPU_SUB_BYTE_SWAP
static const lv_color_t *lv_port_gpu_fix_order(const lv_color_t *src, const lv_area_t * src_area, d2_s32 cf)
{
        lv_coord_t w, h;
        const d2_color *clut;
        uint8_t bpp;

        switch (cf & ~d2_mode_clut) {
        case d2_mode_alpha1:
        case d2_mode_i1:
                clut = mirror_1BLUT;
                bpp = 1;
                break;
        case d2_mode_alpha2:
        case d2_mode_i2:
                clut = mirror_2BLUT;
                bpp = 2;
                break;
        case d2_mode_alpha4:
        case d2_mode_i4:
                clut = mirror_4BLUT;
                bpp = 4;
                break;
        default:
                return src;
        }

        if (buf != NULL) {
                return buf;
        }

        w = CEILING_FUNC((lv_area_get_width(src_area) * bpp), 8);
        h = lv_area_get_height(src_area);
        buf = lv_mem_buf_get(w * h);

        if (buf == NULL) {
                return src;
        }

        lv_port_gpu_start_render();

        D2_EXEC(d2_framebuffer(d2_handle, d1_maptovidmem(d1_handle, (void *)buf), MAX(w, 2), MAX(w, 2), MAX(h, 2), d2_mode_alpha8));
        D2_EXEC(d2_cliprect(d2_handle, 0, 0, w - 1, h - 1));

        D2_EXEC(d2_settexclut(d2_handle, (d2_color *)clut));

        D2_EXEC(d2_setalphablendmode(d2_handle, d2_bm_one, d2_bm_zero));

        D2_EXEC(d2_setblitsrc(d2_handle, d1_maptovidmem(d1_handle, (void *)src), w, w, h, d2_mode_i8 | d2_mode_clut));
        D2_EXEC(d2_blitcopy(d2_handle, w, h, 0, 0, D2_FIX4(w), D2_FIX4(h), D2_FIX4(0), D2_FIX4(0), d2_bf_usealpha));

        lv_port_gpu_execute_render();
        lv_port_gpu_complete_render();

        return buf;
}
#endif /* LV_PORT_DISP_GPU_SUB_BYTE_SWAP */

void lv_port_gpu_wait(lv_disp_drv_t * disp_drv)
{
        lv_port_gpu_complete_render();

#if LV_PORT_DISP_GPU_SUB_BYTE_SWAP
        if (buf != NULL) {
                lv_mem_buf_release((void *)buf);
                buf = NULL;
        }
#endif
}

void lv_port_gpu_flush(void)
{
        lv_port_gpu_hw_deinit();
}

static int lv_port_gpu_hw_init(void)
{
        if (d2_handle != NULL) {
                return 0;
        }

        d2_handle = d2_opendevice(0);

        if (d2_handle == NULL)
                return 0;

        /* Set the maximum burst length of the master bus interfaces to 8 beats*/
        d2_inithwburstlengthlimit(d2_handle, d2_bbl_8, d2_bbl_8, d2_bbl_8, d2_bbl_8);

        /* set blocksize for default displaylist */
        if (d2_setdlistblocksize(d2_handle, 25) != D2_OK) {
                printf("Could NOT d2_setdlistblocksize\n");
                d2_closedevice(d2_handle);

                return 0;
        }

        /* bind the hardware */
        if (d2_inithw(d2_handle, 0) != D2_OK) {
                printf("Could NOT d2_inithw\n");
                d2_closedevice(d2_handle);

                return 0;
        }

        d1_handle = d2_level1interface(d2_handle);
        if (!d1_handle) {
                printf("NO d1_handle\n");
                d2_closedevice(d2_handle);

                return 0;
        }

        renderbuffer = d2_newrenderbuffer(d2_handle, 20, 20);
        if (!renderbuffer) {
                printf("NO renderbuffer\n");
                d2_closedevice(d2_handle);

                return 0;
        }

#ifdef PERFORMANCE_METRICS
        d2_setperfcountevent(d2_handle, 0, d2_pc_davecycles);
#endif

        return 1;
}

static void lv_port_gpu_hw_deinit(void)
{
        if (d2_handle == NULL)
                return;

        D2_EXEC(d2_freerenderbuffer(d2_handle, renderbuffer));

        D2_EXEC(d2_closedevice(d2_handle));

        renderbuffer = NULL;
        d2_handle = NULL;
        d1_handle = NULL;
}

static d2_s32 lv_port_gpu_cf_get_default(void)
{
#if GDI_FB_COLOR_FORMAT == CF_NATIVE_RGB565
        return d2_mode_rgb565;
#elif GDI_FB_COLOR_FORMAT == CF_NATIVE_ARGB8888
        return d2_mode_argb8888;
#endif
}

static d2_color lv_port_gpu_color_lv_to_d2(lv_color_t color)
{
        uint8_t alpha, red, green, blue;
#if GDI_FB_COLOR_FORMAT == CF_NATIVE_RGB565
        alpha = 0xFF;
        red = color.ch.red << 3 | color.ch.red >> 2;
        green = color.ch.green << 2 | color.ch.green >> 4;
        blue = color.ch.blue << 3 | color.ch.blue >> 2;
#elif GDI_FB_COLOR_FORMAT == CF_NATIVE_ARGB8888
        alpha = color.ch.alpha;
        red = color.ch.red;
        green = color.ch.green;
        blue = color.ch.blue;
#endif
        return (alpha) << GDI_F_COLOR_POS(ARGB8888, ALPHA)
                | (red) << GDI_F_COLOR_POS(ARGB8888, RED)
                | (green) << GDI_F_COLOR_POS(ARGB8888, GREEN)
                | (blue) << GDI_F_COLOR_POS(ARGB8888, BLUE);
}

static bool lv_port_gpu_cf_is_alpha(d2_s32 cf)
{
        switch (cf & ~d2_mode_rle) {
        case d2_mode_alpha8:
        case d2_mode_alpha4:
        case d2_mode_alpha2:
        case d2_mode_alpha1:
                return true;
        default:
                return false;
        }
}

static int lv_port_gpu_cf_bpp(d2_s32 cf)
{
        switch (cf & ~(d2_mode_clut | d2_mode_rle)) {
        case d2_mode_argb8888: return 32;
        case d2_mode_rgba8888: return 32;
        case d2_mode_rgb888:   return 32;
        case d2_mode_argb4444: return 16;
        case d2_mode_rgba4444: return 16;
        case d2_mode_argb1555: return 16;
        case d2_mode_rgba5551: return 16;
        case d2_mode_rgb565:   return 16;
        case d2_mode_ai44:     return 8;
        case d2_mode_i8:       return 8;
        case d2_mode_i4:       return 4;
        case d2_mode_i2:       return 2;
        case d2_mode_i1:       return 1;
        case d2_mode_alpha8:   return 8;
        case d2_mode_alpha4:   return 4;
        case d2_mode_alpha2:   return 2;
        case d2_mode_alpha1:   return 1;
        default:               return 0;
        }
}

static bool lv_port_gpu_cf_has_alpha(d2_s32 cf)
{
        switch (cf & ~(d2_mode_clut | d2_mode_rle)) {
        case d2_mode_argb8888:
        case d2_mode_rgba8888:
        case d2_mode_argb4444:
        case d2_mode_rgba4444:
        case d2_mode_argb1555:
        case d2_mode_rgba5551:
        case d2_mode_ai44:
        case d2_mode_i8:
        case d2_mode_i4:
        case d2_mode_i2:
        case d2_mode_i1:
        case d2_mode_alpha8:
        case d2_mode_alpha4:
        case d2_mode_alpha2:
        case d2_mode_alpha1:
                return true;
        default:
                return false;
        }
}

static d2_s32 lv_port_gpu_cf_lv_to_d2(lv_img_cf_t cf)
{
        d2_s32 d2_cf;

#if (DLG_LVGL_CF == 1)
        switch (cf & ~LV_IMG_CF_RLE_FLAG) {
#else
        switch (cf) {
#endif /* (DLG_LVGL_CF == 1) */
        case LV_IMG_CF_TRUE_COLOR:              d2_cf = lv_port_gpu_cf_get_default(); break;
#if LV_COLOR_DEPTH == 32
        case LV_IMG_CF_TRUE_COLOR_ALPHA:        d2_cf = lv_port_gpu_cf_get_default(); break;
#endif
        case LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED: d2_cf = lv_port_gpu_cf_get_default(); break;
        case LV_IMG_CF_ALPHA_1BIT:              d2_cf = d2_mode_alpha1;               break;
        case LV_IMG_CF_ALPHA_2BIT:              d2_cf = d2_mode_alpha2;               break;
        case LV_IMG_CF_ALPHA_4BIT:              d2_cf = d2_mode_alpha4;               break;
        case LV_IMG_CF_ALPHA_8BIT:              d2_cf = d2_mode_alpha8;               break;
        case LV_IMG_CF_INDEXED_1BIT:            d2_cf = d2_mode_i1 | d2_mode_clut;    break;
        case LV_IMG_CF_INDEXED_2BIT:            d2_cf = d2_mode_i2 | d2_mode_clut;    break;
        case LV_IMG_CF_INDEXED_4BIT:            d2_cf = d2_mode_i4 | d2_mode_clut;    break;
        case LV_IMG_CF_INDEXED_8BIT:            d2_cf = d2_mode_i8 | d2_mode_clut;    break;
#if (DLG_LVGL_CF == 1)
        case LV_IMG_CF_RGB565:                  d2_cf = d2_mode_rgb565;               break;
        case LV_IMG_CF_RGB888:                  d2_cf = d2_mode_rgb888;               break;
        case LV_IMG_CF_ARGB8888:                d2_cf = d2_mode_argb8888;             break;
        case LV_IMG_CF_ARGB4444:                d2_cf = d2_mode_argb4444;             break;
        case LV_IMG_CF_ARGB1555:                d2_cf = d2_mode_argb1555;             break;
        case LV_IMG_CF_RGBA8888:                d2_cf = d2_mode_rgba8888;             break;
        case LV_IMG_CF_RGBA4444:                d2_cf = d2_mode_rgba4444;             break;
        case LV_IMG_CF_RGBA5551:                d2_cf = d2_mode_rgba5551;             break;
        case LV_IMG_CF_AI44:                    d2_cf = d2_mode_ai44 | d2_mode_clut;  break;
#endif /* DLG_LVGL_CF */
        default:                                return -1;
        }

#if (DLG_LVGL_CF == 1)
        return d2_cf | (cf & LV_IMG_CF_RLE_FLAG ? d2_mode_rle : 0);
#else
        return d2_cf;
#endif /* (DLG_LVGL_CF == 1) */
}

static bool lv_port_gpu_cf_fb_valid(d2_s32 cf)
{
        if ((cf & (d2_mode_rle | d2_mode_clut)) || cf < 0) {
                return false;
        }

        switch (cf) {
        case d2_mode_alpha8:
        case d2_mode_rgb565:
        case d2_mode_argb8888:
        case d2_mode_argb4444:
        case d2_mode_rgba8888:
        case d2_mode_rgba4444:
                return true;
        default:
                return false;
        }
}

static void lv_port_gpu_start_render(void)
{
        if (d2_enabled) {
                lv_port_gpu_hw_init();

                D2_EXEC(d2_selectrenderbuffer(d2_handle, renderbuffer));
        }
}

static void lv_port_gpu_execute_render(void)
{
        if (d2_handle) {
                D2_EXEC(d2_executerenderbuffer(d2_handle, renderbuffer, 0));
        }
}

static void lv_port_gpu_complete_render(void)
{
        if (d2_handle) {
                D2_EXEC(d2_flushframe(d2_handle));
        }

#ifdef PERFORMANCE_METRICS
        d2_slong render_op_time = 0;
        d2_u32 render_op_duration_us = 0;

        render_op_time = d2_getperfcountvalue(d2_handle, 0);
        render_op_duration_us = render_op_time / (d1_deviceclkfreq(d2_handle, D1_DAVE2D) / 1000000);
        d2_setperfcountvalue(d2_handle, 0, 0);

        gdi_perf_render_op_time(render_op_duration_us, metrics_tag);
#endif
}

#ifdef LOG_ERRORS
static void lv_port_gpu_log_error(d2_s32 status, const char *func, int line)
{
        if (status) {
                log_error_list[error_list_index].error = status;
                log_error_list[error_list_index].func = func;
                log_error_list[error_list_index].line = line;
                printf("%s\r\n", d2_geterrorstring(d2_handle));
                printf("%d:\t%d - %s : %d\r\n", error_count,
                        log_error_list[error_list_index].error,
                        log_error_list[error_list_index].func,
                        log_error_list[error_list_index].line);

                error_count++;
                error_list_index++;
                if (error_list_index >= ERROR_LIST_SIZE) {
                        error_list_index = 0;
                }
        }
}
#endif /* LOG_ERRORS */

#endif /* LV_PORT_DISP_GPU_EN */
