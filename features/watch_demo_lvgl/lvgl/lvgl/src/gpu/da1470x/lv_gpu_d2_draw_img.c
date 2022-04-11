/**
 * @file lv_gpu_d2_draw_img.c
 *
 */
/* Copyright (c) 2021-2022 Modified by Dialog Semiconductor */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if DLG_LVGL_USE_GPU_DA1470X

#include "../../draw/lv_draw_img.h"
#include "../../draw/lv_img_cache.h"
#include "../../hal/lv_hal_disp.h"
#include "../../misc/lv_log.h"
#include "../../core/lv_refr.h"
#include "../../misc/lv_mem.h"
#include "../../misc/lv_math.h"

#include DLG_LVGL_GPU_DA1470X_INCLUDE_PATH

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
LV_ATTRIBUTE_FAST_MEM static lv_res_t lv_img_draw_core(const lv_area_t * coords, const lv_area_t * clip_area,
                                                       const void * src,
                                                       const lv_draw_img_dsc_t * draw_dsc);

LV_ATTRIBUTE_FAST_MEM static void lv_draw_map(const lv_area_t * map_area, const lv_area_t * clip_area,
                                              const uint8_t * map_p,
                                              const lv_draw_img_dsc_t * draw_dsc,
#if (DLG_LVGL_CF == 1)
                                              lv_img_cf_t cf,
#endif /* DLG_LVGL_CF */
                                              bool chroma_key, bool alpha_byte);

static void show_error(const lv_area_t * coords, const lv_area_t * clip_area, const char * msg);
static void draw_cleanup(_lv_img_cache_entry_t * cache);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Draw an image
 * @param coords the coordinates of the image
 * @param mask the image will be drawn only in this area
 * @param src pointer to a lv_color_t array which contains the pixels of the image
 * @param dsc pointer to an initialized `lv_draw_img_dsc_t` variable
 */
void lv_draw_img(const lv_area_t * coords, const lv_area_t * mask, const void * src, const lv_draw_img_dsc_t * dsc)
{
    if(src == NULL) {
        LV_LOG_WARN("Image draw: src is NULL");
        show_error(coords, mask, "No\ndata");
        return;
    }

    if(dsc->opa <= LV_OPA_MIN) return;

    lv_res_t res;
    res = lv_img_draw_core(coords, mask, src, dsc);

    if(res == LV_RES_INV) {
        LV_LOG_WARN("Image draw error");
        show_error(coords, mask, "No\ndata");
        return;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

LV_ATTRIBUTE_FAST_MEM static lv_res_t lv_img_draw_core(const lv_area_t * coords, const lv_area_t * clip_area,
                                                       const void * src,
                                                       const lv_draw_img_dsc_t * draw_dsc)
{
    if(draw_dsc->opa <= LV_OPA_MIN) return LV_RES_OK;

    _lv_img_cache_entry_t * cdsc = _lv_img_cache_open(src, draw_dsc->recolor, draw_dsc->frame_id);

    if(cdsc == NULL) return LV_RES_INV;

    bool chroma_keyed = lv_img_cf_is_chroma_keyed(cdsc->dec_dsc.header.cf);
    bool alpha_byte   = lv_img_cf_has_alpha(cdsc->dec_dsc.header.cf);

    if(cdsc->dec_dsc.error_msg != NULL) {
        LV_LOG_WARN("Image draw error");

        show_error(coords, clip_area, cdsc->dec_dsc.error_msg);
    }
    /*The decoder could open the image and gave the entire uncompressed image.
     *Just draw it!*/
    else if(cdsc->dec_dsc.img_data) {
        lv_area_t map_area_rot;
        lv_area_copy(&map_area_rot, coords);
        if(draw_dsc->angle || draw_dsc->zoom != LV_IMG_ZOOM_NONE) {
            int32_t w = lv_area_get_width(coords);
            int32_t h = lv_area_get_height(coords);

            _lv_img_buf_get_transformed_area(&map_area_rot, w, h, draw_dsc->angle, draw_dsc->zoom, &draw_dsc->pivot);

            map_area_rot.x1 += coords->x1;
            map_area_rot.y1 += coords->y1;
            map_area_rot.x2 += coords->x1;
            map_area_rot.y2 += coords->y1;
        }

        lv_area_t mask_com; /*Common area of mask and coords*/
        bool union_ok;
        union_ok = _lv_area_intersect(&mask_com, clip_area, &map_area_rot);
        /*Out of mask. There is nothing to draw so the image is drawn successfully.*/
        if(union_ok == false) {
            draw_cleanup(cdsc);
            return LV_RES_OK;
        }

        lv_draw_map(coords, &mask_com, cdsc->dec_dsc.img_data, draw_dsc,
#if (DLG_LVGL_CF == 1)
                cdsc->dec_dsc.header.cf | (cdsc->dec_dsc.header.rle ? LV_IMG_CF_RLE_FLAG : 0),
#endif /* DLG_LVGL_CF */
                chroma_keyed, alpha_byte);
    }
    /*The whole uncompressed image is not available. Try to read it line-by-line*/
    else {
        lv_area_t mask_com; /*Common area of mask and coords*/
        bool union_ok;
        union_ok = _lv_area_intersect(&mask_com, clip_area, coords);
        /*Out of mask. There is nothing to draw so the image is drawn successfully.*/
        if(union_ok == false) {
            draw_cleanup(cdsc);
            return LV_RES_OK;
        }

        int32_t width = lv_area_get_width(&mask_com);

        uint8_t  * buf = lv_mem_buf_get(lv_area_get_width(&mask_com) *
                                        LV_IMG_PX_SIZE_ALPHA_BYTE);  /*+1 because of the possible alpha byte*/

        lv_area_t line;
        lv_area_copy(&line, &mask_com);
        lv_area_set_height(&line, 1);
        int32_t x = mask_com.x1 - coords->x1;
        int32_t y = mask_com.y1 - coords->y1;
        int32_t row;
        lv_res_t read_res;
        for(row = mask_com.y1; row <= mask_com.y2; row++) {
            lv_area_t mask_line;
            union_ok = _lv_area_intersect(&mask_line, clip_area, &line);
            if(union_ok == false) continue;

            read_res = lv_img_decoder_read_line(&cdsc->dec_dsc, x, y, width, buf);
            if(read_res != LV_RES_OK) {
                lv_img_decoder_close(&cdsc->dec_dsc);
                LV_LOG_WARN("Image draw can't read the line");
                lv_mem_buf_release(buf);
                draw_cleanup(cdsc);
                return LV_RES_INV;
            }

            lv_draw_map(&line, &mask_line, buf, draw_dsc,
#if (DLG_LVGL_CF == 1)
                cdsc->dec_dsc.header.cf | (cdsc->dec_dsc.header.rle ? LV_IMG_CF_RLE_FLAG : 0),
#endif /* DLG_LVGL_CF */
                chroma_keyed, alpha_byte);
            line.y1++;
            line.y2++;
            y++;
        }
        lv_mem_buf_release(buf);
    }

    draw_cleanup(cdsc);
    return LV_RES_OK;
}

/**
 * Draw a color map to the display (image)
 * @param cords_p coordinates the color map
 * @param mask_p the map will drawn only on this area  (truncated to draw_buf area)
 * @param map_p pointer to a lv_color_t array
 * @param draw_dsc pointer to an initialized `lv_draw_img_dsc_t` variable
 * @param chroma_keyed true: enable transparency of LV_IMG_LV_COLOR_TRANSP color pixels
 * @param alpha_byte true: extra alpha byte is inserted for every pixel
 */
LV_ATTRIBUTE_FAST_MEM static void lv_draw_map(const lv_area_t * map_area, const lv_area_t * clip_area,
                                              const uint8_t * map_p,
                                              const lv_draw_img_dsc_t * draw_dsc,
#if (DLG_LVGL_CF == 1)
                                              lv_img_cf_t cf,
#endif /* DLG_LVGL_CF */
                                              bool chroma_key, bool alpha_byte)
{
    /*Use the clip area as draw area*/
    lv_area_t draw_area;
    lv_area_copy(&draw_area, clip_area);

    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    lv_disp_draw_buf_t * draw_buf = lv_disp_get_draw_buf(disp);
    const lv_area_t * disp_area = &draw_buf->area;

    /*Now `draw_area` has absolute coordinates.
     *Make it relative to `disp_area` to simplify draw to `disp_buf`*/
    draw_area.x1 -= disp_area->x1;
    draw_area.y1 -= disp_area->y1;
    draw_area.x2 -= disp_area->x1;
    draw_area.y2 -= disp_area->y1;

    bool mask_any = lv_draw_mask_is_any(map_area);

    if(disp->driver->gpu_blit_cb && disp->driver->gpu_config_blit_cb && !disp->driver->set_px_cb && !mask_any) {
        if (disp->driver->gpu_config_blit_cb(disp->driver, draw_dsc, LV_IMG_CF_TRUE_COLOR,
#if (DLG_LVGL_CF == 1)
            cf,
#else
                LV_IMG_CF_TRUE_COLOR,
#endif /* DLG_LVGL_CF */
            alpha_byte, chroma_key, true, false)) {
            lv_color_t * disp_buf = draw_buf->buf_act;
            lv_area_t map_area_tmp;

            lv_area_copy(&map_area_tmp, map_area);
            lv_area_move(&map_area_tmp, -disp_area->x1, -disp_area->y1);

            if(disp->driver->gpu_wait_cb) disp->driver->gpu_wait_cb(disp->driver);

            disp->driver->gpu_blit_cb(disp->driver, disp_buf, &draw_area, lv_area_get_width(disp_area),
                                      (lv_color_t *)map_p, &map_area_tmp, draw_dsc->opa);
            return;
        }
    }

#if (DLG_LVGL_CF == 1)
    if (!lv_img_cf_use_sw(cf) || lv_area_get_size(&draw_area) > DLG_LVGL_GPU_BLIT_MASK_SIZE_LIMIT
            || draw_dsc->angle != 0 || draw_dsc->zoom != LV_IMG_ZOOM_NONE) {
        if (disp->driver->gpu_blit_with_mask_cb && disp->driver->gpu_blit_cb && disp->driver->gpu_config_blit_cb && !disp->driver->set_px_cb &&
            mask_any && lv_img_cf_use_gpu(cf))
        {
            /* Check if the everything can be handled by the GPU or not. */
            bool gpu_handled = disp->driver->gpu_config_blit_cb(disp->driver, draw_dsc, LV_IMG_CF_TRUE_COLOR,
                cf, alpha_byte, chroma_key, true, false);


            /* Size calculation */
            lv_img_cf_t bmp_cf = cf;
            bool rle_img = !!(bmp_cf & LV_IMG_CF_RLE_FLAG);
            bool idx_img = ((bmp_cf == LV_IMG_CF_INDEXED_1BIT) ||
                            (bmp_cf == LV_IMG_CF_INDEXED_2BIT) ||
                            (bmp_cf == LV_IMG_CF_INDEXED_4BIT) ||
                            (bmp_cf == LV_IMG_CF_INDEXED_8BIT) ||
                            (bmp_cf == LV_IMG_CF_AI44));
            if (gpu_handled || idx_img || rle_img) {
                bmp_cf = LV_IMG_CF_ARGB8888;
            }
            uint8_t px_size_byte = lv_img_cf_get_px_size(bmp_cf) / 8;
            lv_coord_t draw_area_h = lv_area_get_height(&draw_area);
            lv_coord_t draw_area_w = lv_area_get_width(&draw_area);
            uint32_t hor_res = (uint32_t) lv_disp_get_hor_res(disp);
            uint32_t mask_buf_w = LV_MIN(draw_area_w, hor_res);
            int32_t y_inc = LV_MIN(draw_area_h, DLG_LVGL_GPU_BLIT_MASK_BUFFER_SIZE / (mask_buf_w * px_size_byte));


            /* Allocate buffers */
            lv_opa_t *mask_buf = lv_mem_buf_get(mask_buf_w * y_inc);
            if (!mask_buf) return;
            uint8_t *bmp_buf = NULL;
            if (gpu_handled || idx_img || rle_img) {
                bmp_buf = lv_mem_buf_get(mask_buf_w * px_size_byte * y_inc);
                if (!bmp_buf) {
                    lv_mem_buf_release(mask_buf);
                    return;
                }
            }


            /*Go to the first displayed pixel of the map*/
            lv_area_t blend_area;
            blend_area.x1 = draw_area.x1;
            blend_area.x2 = draw_area.x2;
            blend_area.y1 = draw_area.y1;
            blend_area.y2 = draw_area.y1 + y_inc - 1;
            if (!gpu_handled) {
                lv_area_move(&blend_area, disp_area->x1, disp_area->y1);
            }


            /* Start blending per line chunk. */
            for (int32_t y = 0; y < draw_area_h && y_inc > 0;) {
                lv_draw_img_dsc_t dsc;
                lv_draw_img_dsc_init(&dsc);

                if (!gpu_handled) {
                    lv_area_t blend_area_rel, map_area_rel;

                    lv_area_copy(&map_area_rel, map_area);
                    lv_area_copy(&blend_area_rel, &blend_area);

                    /* Move areas to blend origin */
                    lv_area_move(&map_area_rel, -blend_area.x1, -blend_area.y1);
                    lv_area_move(&blend_area_rel, -blend_area.x1, -blend_area.y1);

                    if (idx_img || rle_img) {
                        // Re-construct lines of indexed image to temp buffer.
                        if (disp->driver->gpu_config_blit_cb(disp->driver, &dsc, bmp_cf, cf, true, false, false, false)) {
                            if(disp->driver->gpu_wait_cb) disp->driver->gpu_wait_cb(disp->driver);
                            disp->driver->gpu_blit_cb(disp->driver, (lv_color_t *)bmp_buf, &blend_area_rel, mask_buf_w,
                                (const lv_color_t *)map_p, &map_area_rel, LV_OPA_COVER);
                        }
                    }

                    /* Prepare the `mask_buf`if there are other masks
                     *  - copy alpha channel to mask buffer */
                    if (disp->driver->gpu_config_blit_cb(disp->driver, &dsc, LV_IMG_CF_ALPHA_8BIT, cf, true, false, false, false)) {
                        disp->driver->gpu_blit_cb(disp->driver, (lv_color_t *)mask_buf, &blend_area_rel, mask_buf_w,
                            (const lv_color_t *)map_p, &map_area_rel, LV_OPA_COVER);
                        /* Wait for completion since mask is immediately used by the CPU */
                        if(disp->driver->gpu_wait_cb) disp->driver->gpu_wait_cb(disp->driver);
                    }
                } else {
                    /* Prepare the `mask_buf`if there are other masks
                     * - no need to get image alpha values into the mask buffer, pixel alpha will be blended with mask by GPU */
                    lv_memset_ff(mask_buf, mask_buf_w * y_inc);
                }

#if LV_DRAW_COMPLEX
                uint32_t px_i_start = 0;
                for (int y_inc_cnt = 0; y_inc_cnt < y_inc; y_inc_cnt++) {
                    /*Apply the masks if any*/
                    lv_draw_mask_res_t mask_res_sub;
                    mask_res_sub = lv_draw_mask_apply(mask_buf + px_i_start,
                                                      draw_area.x1 + draw_buf->area.x1,
                                                      draw_area.y1 + draw_buf->area.y1 + y + y_inc_cnt,
                                                      draw_area_w);
                    if(mask_res_sub == LV_DRAW_MASK_RES_TRANSP) {
                        lv_memset_00(mask_buf + px_i_start, draw_area_w);
                    }

                    px_i_start += draw_area_w;
                }
#endif
                if (gpu_handled) {
                    lv_color_t * disp_buf = draw_buf->buf_act;
                    lv_area_t map_area_rel;

                    lv_area_copy(&map_area_rel, map_area);
                    /* Set map area to display origin */
                    lv_area_move(&map_area_rel, -disp_area->x1, -disp_area->y1);

                    if(disp->driver->gpu_wait_cb) disp->driver->gpu_wait_cb(disp->driver);
                    disp->driver->gpu_blit_with_mask_cb(disp->driver, disp_buf, &blend_area, lv_area_get_width(disp_area),
                        (lv_color_t *)map_p, &map_area_rel, mask_buf, (uint32_t*) bmp_buf, draw_dsc->opa);
                } else {
                    const uint8_t *prnt_map = (idx_img || rle_img) ? bmp_buf : map_p;
                    const lv_area_t *prnt_map_area = (idx_img || rle_img) ? &blend_area : map_area;
                    _lv_blend_map(&blend_area, prnt_map_area, (lv_color_t *)prnt_map, mask_buf, LV_DRAW_MASK_RES_CHANGED,
                        draw_dsc->opa, draw_dsc->blend_mode, bmp_cf);
                }

                /* Check for next line chuck. */
                y += y_inc;
                if (y + y_inc > draw_area_h) {
                    y_inc = draw_area_h - y;
                }

                blend_area.y1 = blend_area.y2 + 1;
                blend_area.y2 = blend_area.y1 + y_inc-1;
            }

            if(gpu_handled && disp->driver->gpu_wait_cb) disp->driver->gpu_wait_cb(disp->driver);

            lv_mem_buf_release(mask_buf);
            if (bmp_buf) {
                lv_mem_buf_release(bmp_buf);
            }

            return;
        }
    }

    if (!lv_img_cf_use_sw(cf)) {
        LV_LOG_WARN("Image format not supported error");
        show_error(map_area, clip_area, "No\ndata");
        return;
    }
#endif /* DLG_LVGL_CF */

    /*The simplest case just copy the pixels into the draw_buf*/
    if(!mask_any && draw_dsc->angle == 0 && draw_dsc->zoom == LV_IMG_ZOOM_NONE &&
       chroma_key == false && alpha_byte == false && draw_dsc->recolor_opa == LV_OPA_TRANSP) {
        _lv_blend_map(clip_area, map_area, (lv_color_t *)map_p, NULL, LV_DRAW_MASK_RES_FULL_COVER, draw_dsc->opa,
                      draw_dsc->blend_mode
#if (DLG_LVGL_CF == 1)
                      , cf
#endif /* DLG_LVGL_CF */
                      );
    }

    /*In the other cases every pixel need to be checked one-by-one*/
    else {
        //#if LV_DRAW_COMPLEX
        /*The pixel size in byte is different if an alpha byte is added too*/
        uint8_t px_size_byte = alpha_byte ? LV_IMG_PX_SIZE_ALPHA_BYTE : sizeof(lv_color_t);

        /*Go to the first displayed pixel of the map*/
        int32_t map_w = lv_area_get_width(map_area);
        const uint8_t * map_buf_tmp = map_p;
        map_buf_tmp += map_w * (draw_area.y1 - (map_area->y1 - disp_area->y1)) * px_size_byte;
        map_buf_tmp += (draw_area.x1 - (map_area->x1 - disp_area->x1)) * px_size_byte;

        lv_color_t c;
        lv_color_t chroma_keyed_color = LV_COLOR_CHROMA_KEY;
        uint32_t px_i = 0;

        const uint8_t * map_px;

        lv_area_t blend_area;
        blend_area.x1 = draw_area.x1 + disp_area->x1;
        blend_area.x2 = blend_area.x1 + lv_area_get_width(&draw_area) - 1;
        blend_area.y1 = disp_area->y1 + draw_area.y1;
        blend_area.y2 = blend_area.y1;

        lv_coord_t draw_area_h = lv_area_get_height(&draw_area);
        lv_coord_t draw_area_w = lv_area_get_width(&draw_area);

        bool transform = draw_dsc->angle != 0 || draw_dsc->zoom != LV_IMG_ZOOM_NONE ? true : false;
        /*Simple ARGB image. Handle it as special case because it's very common*/
        if(!mask_any && !transform && !chroma_key && draw_dsc->recolor_opa == LV_OPA_TRANSP && alpha_byte) {
            uint32_t hor_res = (uint32_t) lv_disp_get_hor_res(disp);
            uint32_t mask_buf_size = lv_area_get_size(&draw_area) > (uint32_t) hor_res ? hor_res : lv_area_get_size(&draw_area);
            lv_color_t * map2 = lv_mem_buf_get(mask_buf_size * sizeof(lv_color_t));
            lv_opa_t * mask_buf = lv_mem_buf_get(mask_buf_size);

            int32_t x;
            int32_t y;
            for(y = 0; y < draw_area_h; y++) {
                map_px = map_buf_tmp;
                for(x = 0; x < draw_area_w; x++, map_px += px_size_byte, px_i++) {
                    lv_opa_t px_opa = map_px[LV_IMG_PX_SIZE_ALPHA_BYTE - 1];
                    mask_buf[px_i] = px_opa;
                    if(px_opa) {
#if LV_COLOR_DEPTH == 8 || LV_COLOR_DEPTH == 1
                        map2[px_i].full =  map_px[0];
#elif LV_COLOR_DEPTH == 16
                        map2[px_i].full =  map_px[0] + (map_px[1] << 8);
#elif LV_COLOR_DEPTH == 32
                        map2[px_i].full =  *((uint32_t *)map_px);
#endif
                    }
#if LV_COLOR_DEPTH == 32
                    map2[px_i].ch.alpha = 0xFF;
#endif
                }

                map_buf_tmp += map_w * px_size_byte;
                if(px_i + lv_area_get_width(&draw_area) < mask_buf_size) {
                    blend_area.y2 ++;
                }
                else {
                    _lv_blend_map(clip_area, &blend_area, map2, mask_buf, LV_DRAW_MASK_RES_CHANGED, draw_dsc->opa, draw_dsc->blend_mode
#if (DLG_LVGL_CF == 1)
                      , cf
#endif /* DLG_LVGL_CF */
                      );

                    blend_area.y1 = blend_area.y2 + 1;
                    blend_area.y2 = blend_area.y1;

                    px_i = 0;
                }
            }
            /*Flush the last part*/
            if(blend_area.y1 != blend_area.y2) {
                blend_area.y2--;
                _lv_blend_map(clip_area, &blend_area, map2, mask_buf, LV_DRAW_MASK_RES_CHANGED, draw_dsc->opa, draw_dsc->blend_mode
#if (DLG_LVGL_CF == 1)
                      , cf
#endif /* DLG_LVGL_CF */
                      );
            }

            lv_mem_buf_release(mask_buf);
            lv_mem_buf_release(map2);
        }
        /*Most complicated case: transform or other mask or chroma keyed*/
        else {
            /*Build the image and a mask line-by-line*/
            uint32_t hor_res = (uint32_t) lv_disp_get_hor_res(disp);
            uint32_t mask_buf_size = lv_area_get_size(&draw_area) > hor_res ? hor_res : lv_area_get_size(&draw_area);
            lv_color_t * map2 = lv_mem_buf_get(mask_buf_size * sizeof(lv_color_t));
            lv_opa_t * mask_buf = lv_mem_buf_get(mask_buf_size);

#if LV_DRAW_COMPLEX
            lv_img_transform_dsc_t trans_dsc;
            lv_memset_00(&trans_dsc, sizeof(lv_img_transform_dsc_t));
            if(transform) {
                lv_img_cf_t cf = LV_IMG_CF_TRUE_COLOR;
                if(alpha_byte) cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
                else if(chroma_key) cf = LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED;

                trans_dsc.cfg.angle = draw_dsc->angle;
                trans_dsc.cfg.zoom = draw_dsc->zoom;
                trans_dsc.cfg.src = map_p;
                trans_dsc.cfg.src_w = map_w;
                trans_dsc.cfg.src_h = lv_area_get_height(map_area);;
                trans_dsc.cfg.cf = cf;
                trans_dsc.cfg.pivot_x = draw_dsc->pivot.x;
                trans_dsc.cfg.pivot_y = draw_dsc->pivot.y;
                trans_dsc.cfg.color = draw_dsc->recolor;
                trans_dsc.cfg.antialias = draw_dsc->antialias;

                _lv_img_buf_transform_init(&trans_dsc);
            }
#endif
            uint16_t recolor_premult[3] = {0};
            lv_opa_t recolor_opa_inv = 255 - draw_dsc->recolor_opa;
            if(draw_dsc->recolor_opa != 0) {
                lv_color_premult(draw_dsc->recolor, draw_dsc->recolor_opa, recolor_premult);
            }

            lv_draw_mask_res_t mask_res;
            mask_res = (alpha_byte || chroma_key || draw_dsc->angle ||
                        draw_dsc->zoom != LV_IMG_ZOOM_NONE) ? LV_DRAW_MASK_RES_CHANGED : LV_DRAW_MASK_RES_FULL_COVER;

            /*Prepare the `mask_buf`if there are other masks*/
            if(mask_any) {
                lv_memset_ff(mask_buf, mask_buf_size);
            }

            int32_t x;
            int32_t y;
#if LV_DRAW_COMPLEX
            int32_t rot_y = disp_area->y1 + draw_area.y1 - map_area->y1;
#endif
            for(y = 0; y < draw_area_h; y++) {
                map_px = map_buf_tmp;
#if LV_DRAW_COMPLEX
                uint32_t px_i_start = px_i;
                int32_t rot_x = disp_area->x1 + draw_area.x1 - map_area->x1;
#endif

                for(x = 0; x < draw_area_w; x++, map_px += px_size_byte, px_i++) {

#if LV_DRAW_COMPLEX
                    if(transform) {

                        /*Transform*/
                        bool ret;
                        ret = _lv_img_buf_transform(&trans_dsc, rot_x + x, rot_y + y);
                        if(ret == false) {
                            mask_buf[px_i] = LV_OPA_TRANSP;
                            continue;
                        }
                        else {
                            mask_buf[px_i] = trans_dsc.res.opa;
                            c.full = trans_dsc.res.color.full;
                        }
                    }
                    /*No transform*/
                    else
#endif
                    {
                        if(alpha_byte) {
                            lv_opa_t px_opa = map_px[LV_IMG_PX_SIZE_ALPHA_BYTE - 1];
                            mask_buf[px_i] = px_opa;
                            if(px_opa == 0) {
#if  LV_COLOR_DEPTH == 32
                                map2[px_i].full = 0;
#endif
                                continue;
                            }
                        }
                        else {
                            mask_buf[px_i] = 0xFF;
                        }

#if LV_COLOR_DEPTH == 1
                        c.full = map_px[0];
#elif LV_COLOR_DEPTH == 8
                        c.full =  map_px[0];
#elif LV_COLOR_DEPTH == 16
                        c.full =  map_px[0] + (map_px[1] << 8);
#elif LV_COLOR_DEPTH == 32
                        c.full =  *((uint32_t *)map_px);
                        c.ch.alpha = 0xFF;
#endif
                        if(chroma_key) {
                            if(c.full == chroma_keyed_color.full) {
                                mask_buf[px_i] = LV_OPA_TRANSP;
#if  LV_COLOR_DEPTH == 32
                                map2[px_i].full = 0;
#endif
                                continue;
                            }
                        }

                    }
                    if(draw_dsc->recolor_opa != 0) {
                        c = lv_color_mix_premult(recolor_premult, c, recolor_opa_inv);
                    }

                    map2[px_i].full = c.full;
                }
#if LV_DRAW_COMPLEX
                /*Apply the masks if any*/
                if(mask_any) {
                    lv_draw_mask_res_t mask_res_sub;
                    mask_res_sub = lv_draw_mask_apply(mask_buf + px_i_start, draw_area.x1 + draw_buf->area.x1,
                                                      y + draw_area.y1 + draw_buf->area.y1,
                                                      lv_area_get_width(&draw_area));
                    if(mask_res_sub == LV_DRAW_MASK_RES_TRANSP) {
                        lv_memset_00(mask_buf + px_i_start, lv_area_get_width(&draw_area));
                        mask_res = LV_DRAW_MASK_RES_CHANGED;
                    }
                    else if(mask_res_sub == LV_DRAW_MASK_RES_CHANGED) {
                        mask_res = LV_DRAW_MASK_RES_CHANGED;
                    }
                }
#endif

                map_buf_tmp += map_w * px_size_byte;
                if(px_i + lv_area_get_width(&draw_area) < mask_buf_size) {
                    blend_area.y2 ++;
                }
                else {

                    _lv_blend_map(clip_area, &blend_area, map2, mask_buf, mask_res, draw_dsc->opa, draw_dsc->blend_mode
#if (DLG_LVGL_CF == 1)
                      , cf
#endif /* DLG_LVGL_CF */
                      );

                    blend_area.y1 = blend_area.y2 + 1;
                    blend_area.y2 = blend_area.y1;

                    px_i = 0;
                    mask_res = (alpha_byte || chroma_key || draw_dsc->angle ||
                                draw_dsc->zoom != LV_IMG_ZOOM_NONE) ? LV_DRAW_MASK_RES_CHANGED : LV_DRAW_MASK_RES_FULL_COVER;

                    /*Prepare the `mask_buf`if there are other masks*/
                    if(mask_any) {
                        lv_memset_ff(mask_buf, mask_buf_size);
                    }
                }
            }

            /*Flush the last part*/
            if(blend_area.y1 != blend_area.y2) {
                blend_area.y2--;
                _lv_blend_map(clip_area, &blend_area, map2, mask_buf, mask_res, draw_dsc->opa, draw_dsc->blend_mode
#if (DLG_LVGL_CF == 1)
                      , cf
#endif /* DLG_LVGL_CF */
                      );
            }

            lv_mem_buf_release(mask_buf);
            lv_mem_buf_release(map2);
        }
    }
}

static void show_error(const lv_area_t * coords, const lv_area_t * clip_area, const char * msg)
{
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = lv_color_white();
    lv_draw_rect(coords, clip_area, &rect_dsc);

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    lv_draw_label(coords, clip_area, &label_dsc, msg, NULL);
}

static void draw_cleanup(_lv_img_cache_entry_t * cache)
{
    /*Automatically close images with no caching*/
#if LV_IMG_CACHE_DEF_SIZE == 0
    lv_img_decoder_close(&cache->dec_dsc);
#else
    LV_UNUSED(cache);
#endif
}

#endif /* DLG_LVGL_USE_GPU_DA1470X */
