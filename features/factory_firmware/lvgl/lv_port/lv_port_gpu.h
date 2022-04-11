/**
 ****************************************************************************************
 *
 * @file lv_port_gpu.h
 *
 * @brief GPU driver
 *
 * Copyright (C) 2021-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#ifndef LV_PORT_GPU_H_
#define LV_PORT_GPU_H_

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

/*********************
 *      DEFINES
 *********************/
#ifndef LV_PORT_DISP_GPU_EN
#define LV_PORT_DISP_GPU_EN                     (DLG_LVGL_USE_GPU_DA1470X)
#endif /* LV_PORT_DISP_GPU_EN */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_port_gpu_init(void);

void lv_port_gpu_wait(lv_disp_drv_t * disp_drv);

void lv_port_gpu_fill(lv_disp_drv_t *disp_drv, lv_color_t *dst, lv_coord_t dst_width,
        const lv_area_t *fill_area, lv_color_t color, lv_opa_t opa);

void lv_port_gpu_blit(lv_disp_drv_t *disp_drv, lv_color_t *dst, const lv_area_t * dst_area, lv_coord_t dst_pitch,
        const lv_color_t *src_buf, const lv_area_t * src_area, lv_opa_t opa);

bool lv_port_gpu_config_blit(lv_disp_drv_t *disp_drv, const lv_draw_img_dsc_t *draw_dsc,  lv_img_cf_t dst_cf,
        lv_img_cf_t src_cf, bool alpha_en, bool color_key_en, bool blend_en, bool colorize_en);

void lv_port_gpu_blit_with_mask(lv_disp_drv_t *disp_drv, lv_color_t *dst, const lv_area_t * dst_area, lv_coord_t dst_pitch,
        const lv_color_t *src, const lv_area_t * src_area, const lv_opa_t *msk, uint32_t *work_buf, lv_opa_t opa);

void lv_port_gpu_render_box(lv_disp_drv_t *disp_drv,  lv_color_t *dst, lv_coord_t dst_pitch,
        lv_coord_t x, lv_coord_t y, lv_coord_t w, lv_coord_t h, lv_color_t color);

void lv_port_gpu_flush(void);
/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_PORT_GPU_H_*/
