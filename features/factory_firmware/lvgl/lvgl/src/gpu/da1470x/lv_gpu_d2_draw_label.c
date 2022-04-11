/**
 * @file lv_gpu_d2_draw_label.c
 *
 */
/* Copyright (c) 2021-2022 Modified by Dialog Semiconductor */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if DLG_LVGL_USE_GPU_DA1470X

#include "../../draw/lv_draw_label.h"
#include "../../misc/lv_math.h"
#include "../../hal/lv_hal_disp.h"
#include "../../core/lv_refr.h"
#include "../../misc/lv_bidi.h"
#include "../../misc/lv_assert.h"

#include DLG_LVGL_GPU_DA1470X_INCLUDE_PATH

/*********************
 *      DEFINES
 *********************/
#define LABEL_RECOLOR_PAR_LENGTH 6
#define LV_LABEL_HINT_UPDATE_TH 1024 /*Update the "hint" if the label's y coordinates have changed more then this*/

/**********************
 *      TYPEDEFS
 **********************/
enum {
    CMD_STATE_WAIT,
    CMD_STATE_PAR,
    CMD_STATE_IN,
};
typedef uint8_t cmd_state_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

LV_ATTRIBUTE_FAST_MEM static void draw_letter_normal(lv_coord_t pos_x, lv_coord_t pos_y, lv_font_glyph_dsc_t * g,
                                                     const lv_area_t * clip_area,
                                                     const uint8_t * map_p, lv_color_t color, lv_opa_t opa, lv_blend_mode_t blend_mode);

#if LV_DRAW_COMPLEX && LV_USE_FONT_SUBPX
static void draw_letter_subpx(lv_coord_t pos_x, lv_coord_t pos_y, lv_font_glyph_dsc_t * g, const lv_area_t * clip_area,
                              const uint8_t * map_p, lv_color_t color, lv_opa_t opa, lv_blend_mode_t blend_mode);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/

extern const uint8_t _lv_bpp1_opa_table[2];  /*Opacity mapping with bpp = 1 (Just for compatibility)*/
extern const uint8_t _lv_bpp2_opa_table[4];  /*Opacity mapping with bpp = 2*/
extern const uint8_t _lv_bpp3_opa_table[8];  /*Opacity mapping with bpp = 3*/
extern const uint8_t _lv_bpp4_opa_table[16]; /*Opacity mapping with bpp = 4*/
extern const uint8_t _lv_bpp8_opa_table[256];

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Draw a letter in the Virtual Display Buffer
 * @param pos_p left-top coordinate of the latter
 * @param mask_p the letter will be drawn only on this area  (truncated to draw_buf area)
 * @param font_p pointer to font
 * @param letter a letter to draw
 * @param color color of letter
 * @param opa opacity of letter (0..255)
 */
LV_ATTRIBUTE_FAST_MEM void lv_draw_letter(const lv_point_t * pos_p, const lv_area_t * clip_area,
                                          const lv_font_t * font_p,
                                          uint32_t letter,
                                          lv_color_t color, lv_opa_t opa, lv_blend_mode_t blend_mode)
{
    if(opa < LV_OPA_MIN) return;
    if(opa > LV_OPA_MAX) opa = LV_OPA_COVER;

    if(font_p == NULL) {
        LV_LOG_WARN("lv_draw_letter: font is NULL");
        return;
    }

    lv_font_glyph_dsc_t g;
    bool g_ret = lv_font_get_glyph_dsc(font_p, &g, letter, '\0');
    if(g_ret == false)  {
        /*Add warning if the dsc is not found
         *but do not print warning for non printable ASCII chars (e.g. '\n')*/
        if(letter >= 0x20 &&
           letter != 0xf8ff && /*LV_SYMBOL_DUMMY*/
           letter != 0x200c) { /*ZERO WIDTH NON-JOINER*/
            LV_LOG_WARN("lv_draw_letter: glyph dsc. not found for U+%X", letter);
        }
        return;
    }

    /*Don't draw anything if the character is empty. E.g. space*/
    if((g.box_h == 0) || (g.box_w == 0)) return;

    int32_t pos_x = pos_p->x + g.ofs_x;
    int32_t pos_y = pos_p->y + (font_p->line_height - font_p->base_line) - g.box_h - g.ofs_y;

    /*If the letter is completely out of mask don't draw it*/
    if(pos_x + g.box_w < clip_area->x1 ||
       pos_x > clip_area->x2 ||
       pos_y + g.box_h < clip_area->y1 ||
       pos_y > clip_area->y2)  {
        return;
    }

    const uint8_t * map_p = lv_font_get_glyph_bitmap(font_p, letter);
    if(map_p == NULL) {
        LV_LOG_WARN("lv_draw_letter: character's bitmap not found");
        return;
    }

    if(font_p->subpx) {
#if LV_DRAW_COMPLEX && LV_USE_FONT_SUBPX
        draw_letter_subpx(pos_x, pos_y, &g, clip_area, map_p, color, opa, blend_mode);
#else
        LV_LOG_WARN("Can't draw sub-pixel rendered letter because LV_USE_FONT_SUBPX == 0 in lv_conf.h");
#endif
    }
    else {
        draw_letter_normal(pos_x, pos_y, &g, clip_area, map_p, color, opa, blend_mode);
    }
}

LV_ATTRIBUTE_FAST_MEM static void draw_letter_normal(lv_coord_t pos_x, lv_coord_t pos_y, lv_font_glyph_dsc_t * g,
                                                     const lv_area_t * clip_area,
                                                     const uint8_t * map_p, lv_color_t color, lv_opa_t opa, lv_blend_mode_t blend_mode)
{
    const uint8_t * bpp_opa_table_p;
    uint32_t bitmask_init;
    uint32_t bitmask;
    uint32_t bpp = g->bpp;
    uint32_t shades;
    if(bpp == 3) bpp = 4;

    switch(bpp) {
        case 1:
            bpp_opa_table_p = _lv_bpp1_opa_table;
            bitmask_init  = 0x80;
            shades = 2;
            break;
        case 2:
            bpp_opa_table_p = _lv_bpp2_opa_table;
            bitmask_init  = 0xC0;
            shades = 4;
            break;
        case 4:
            bpp_opa_table_p = _lv_bpp4_opa_table;
            bitmask_init  = 0xF0;
            shades = 16;
            break;
        case 8:
            bpp_opa_table_p = _lv_bpp8_opa_table;
            bitmask_init  = 0xFF;
            shades = 256;
            break;       /*No opa table, pixel value will be used directly*/
        default:
            LV_LOG_WARN("lv_draw_letter: invalid bpp");
            return; /*Invalid bpp. Can't render the letter*/
    }

    static lv_opa_t opa_table[256];
    static lv_opa_t prev_opa = LV_OPA_TRANSP;
    static uint32_t prev_bpp = 0;
    if(opa < LV_OPA_MAX) {
        if(prev_opa != opa || prev_bpp != bpp) {
            uint32_t i;
            for(i = 0; i < shades; i++) {
                opa_table[i] = bpp_opa_table_p[i] == LV_OPA_COVER ? opa : ((bpp_opa_table_p[i] * opa) >> 8);
            }
        }
        bpp_opa_table_p = opa_table;
        prev_opa = opa;
        prev_bpp = bpp;
    }

    int32_t col, row;
    int32_t box_w = g->box_w;
    int32_t box_h = g->box_h;
    int32_t width_bit = box_w * bpp; /*Letter width in bits*/

    /*Calculate the col/row start/end on the map*/
    int32_t col_start = pos_x >= clip_area->x1 ? 0 : clip_area->x1 - pos_x;
    int32_t col_end   = pos_x + box_w <= clip_area->x2 ? box_w : clip_area->x2 - pos_x + 1;
    int32_t row_start = pos_y >= clip_area->y1 ? 0 : clip_area->y1 - pos_y;
    int32_t row_end   = pos_y + box_h <= clip_area->y2 ? box_h : clip_area->y2 - pos_y + 1;

    /*Move on the map too*/
    uint32_t bit_ofs = (row_start * width_bit) + (col_start * bpp);
    map_p += bit_ofs >> 3;

    uint8_t letter_px;
    uint32_t col_bit;
    col_bit = bit_ofs & 0x7; /*"& 0x7" equals to "% 8" just faster*/

    lv_coord_t hor_res = lv_disp_get_hor_res(_lv_refr_get_disp_refreshing());
    uint32_t mask_buf_size = box_w * box_h > hor_res ? hor_res : box_w * box_h;
    lv_opa_t * mask_buf = lv_mem_buf_get(mask_buf_size);
    int32_t mask_p = 0;

    lv_area_t fill_area;
    fill_area.x1 = col_start + pos_x;
    fill_area.x2 = col_end  + pos_x - 1;
    fill_area.y1 = row_start + pos_y;
    fill_area.y2 = fill_area.y1;
#if LV_DRAW_COMPLEX
    bool mask_any = lv_draw_mask_is_any(&fill_area);
#endif

    uint32_t col_bit_max = 8 - bpp;
    uint32_t col_bit_row_ofs = (box_w + col_start - col_end) * bpp;

    for(row = row_start ; row < row_end; row++) {
#if LV_DRAW_COMPLEX
        int32_t mask_p_start = mask_p;
#endif
        bitmask = bitmask_init >> col_bit;
        for(col = col_start; col < col_end; col++) {
            /*Load the pixel's opacity into the mask*/
            letter_px = (*map_p & bitmask) >> (col_bit_max - col_bit);
            if(letter_px) {
                mask_buf[mask_p] = bpp_opa_table_p[letter_px];
            }
            else {
                mask_buf[mask_p] = 0;
            }

            /*Go to the next column*/
            if(col_bit < col_bit_max) {
                col_bit += bpp;
                bitmask = bitmask >> bpp;
            }
            else {
                col_bit = 0;
                bitmask = bitmask_init;
                map_p++;
            }

            /*Next mask byte*/
            mask_p++;
        }

#if LV_DRAW_COMPLEX
        /*Apply masks if any*/
        if(mask_any) {
            lv_draw_mask_res_t mask_res = lv_draw_mask_apply(mask_buf + mask_p_start, fill_area.x1, fill_area.y2,
                                                             lv_area_get_width(&fill_area));
            if(mask_res == LV_DRAW_MASK_RES_TRANSP) {
                lv_memset_00(mask_buf + mask_p_start, lv_area_get_width(&fill_area));
            }
        }
#endif

        if((uint32_t) mask_p + (col_end - col_start) < mask_buf_size) {
            fill_area.y2 ++;
        }
        else {
            _lv_blend_fill(clip_area, &fill_area,
                           color, mask_buf, LV_DRAW_MASK_RES_CHANGED, LV_OPA_COVER,
                           blend_mode);

            fill_area.y1 = fill_area.y2 + 1;
            fill_area.y2 = fill_area.y1;
            mask_p = 0;
        }

        col_bit += col_bit_row_ofs;
        map_p += (col_bit >> 3);
        col_bit = col_bit & 0x7;
    }

    /*Flush the last part*/
    if(fill_area.y1 != fill_area.y2) {
        fill_area.y2--;
        _lv_blend_fill(clip_area, &fill_area,
                       color, mask_buf, LV_DRAW_MASK_RES_CHANGED, LV_OPA_COVER,
                       blend_mode);
        mask_p = 0;
    }

    lv_mem_buf_release(mask_buf);
}

#if LV_DRAW_COMPLEX && LV_USE_FONT_SUBPX
static void draw_letter_subpx(lv_coord_t pos_x, lv_coord_t pos_y, lv_font_glyph_dsc_t * g, const lv_area_t * clip_area,
                              const uint8_t * map_p, lv_color_t color, lv_opa_t opa, lv_blend_mode_t blend_mode)
{
    const uint8_t * bpp_opa_table;
    uint32_t bitmask_init;
    uint32_t bitmask;
    uint32_t bpp = g->bpp;
    if(bpp == 3) bpp = 4;

    switch(bpp) {
        case 1:
            bpp_opa_table = _lv_bpp1_opa_table;
            bitmask_init  = 0x80;
            break;
        case 2:
            bpp_opa_table = _lv_bpp2_opa_table;
            bitmask_init  = 0xC0;
            break;
        case 4:
            bpp_opa_table = _lv_bpp4_opa_table;
            bitmask_init  = 0xF0;
            break;
        case 8:
            bpp_opa_table = _lv_bpp8_opa_table;
            bitmask_init  = 0xFF;
            break;       /*No opa table, pixel value will be used directly*/
        default:
            LV_LOG_WARN("lv_draw_letter: invalid bpp not found");
            return; /*Invalid bpp. Can't render the letter*/
    }

    int32_t col, row;

    int32_t box_w = g->box_w;
    int32_t box_h = g->box_h;
    int32_t width_bit = box_w * bpp; /*Letter width in bits*/

    /*Calculate the col/row start/end on the map*/
    int32_t col_start = pos_x >= clip_area->x1 ? 0 : (clip_area->x1 - pos_x) * 3;
    int32_t col_end   = pos_x + box_w / 3 <= clip_area->x2 ? box_w : (clip_area->x2 - pos_x + 1) * 3;
    int32_t row_start = pos_y >= clip_area->y1 ? 0 : clip_area->y1 - pos_y;
    int32_t row_end   = pos_y + box_h <= clip_area->y2 ? box_h : clip_area->y2 - pos_y + 1;

    /*Move on the map too*/
    int32_t bit_ofs = (row_start * width_bit) + (col_start * bpp);
    map_p += bit_ofs >> 3;

    uint8_t letter_px;
    lv_opa_t px_opa;
    int32_t col_bit;
    col_bit = bit_ofs & 0x7; /*"& 0x7" equals to "% 8" just faster*/

    lv_area_t map_area;
    map_area.x1 = col_start / 3 + pos_x;
    map_area.x2 = col_end / 3  + pos_x - 1;
    map_area.y1 = row_start + pos_y;
    map_area.y2 = map_area.y1;

    if(map_area.x2 <= map_area.x1) return;

    int32_t mask_buf_size = box_w * box_h > _LV_MASK_BUF_MAX_SIZE ? _LV_MASK_BUF_MAX_SIZE : g->box_w * g->box_h;
    lv_opa_t * mask_buf = lv_mem_buf_get(mask_buf_size);
    int32_t mask_p = 0;

    lv_color_t * color_buf = lv_mem_buf_get(mask_buf_size * sizeof(lv_color_t));

    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    lv_disp_draw_buf_t * draw_buf = lv_disp_get_draw_buf(disp);

    int32_t disp_buf_width = lv_area_get_width(&draw_buf->area);
    lv_color_t * disp_buf_buf_tmp = draw_buf->buf_act;

    /*Set a pointer on draw_buf to the first pixel of the letter*/
    disp_buf_buf_tmp += ((pos_y - draw_buf->area.y1) * disp_buf_width) + pos_x - draw_buf->area.x1;

    /*If the letter is partially out of mask the move there on draw_buf*/
    disp_buf_buf_tmp += (row_start * disp_buf_width) + col_start / 3;

    bool mask_any = lv_draw_mask_is_any(&map_area);
    uint8_t font_rgb[3];

#if LV_COLOR_16_SWAP == 0
    uint8_t txt_rgb[3] = {color.ch.red, color.ch.green, color.ch.blue};
#else
    uint8_t txt_rgb[3] = {color.ch.red, (color.ch.green_h << 3) + color.ch.green_l, color.ch.blue};
#endif

    for(row = row_start ; row < row_end; row++) {
        uint32_t subpx_cnt = 0;
        bitmask = bitmask_init >> col_bit;
        int32_t mask_p_start = mask_p;

        for(col = col_start; col < col_end; col++) {
            /*Load the pixel's opacity into the mask*/
            letter_px = (*map_p & bitmask) >> (8 - col_bit - bpp);
            if(letter_px != 0) {
                if(opa == LV_OPA_COVER) {
                    px_opa = bpp == 8 ? letter_px : bpp_opa_table[letter_px];
                }
                else {
                    px_opa = bpp == 8 ? (uint32_t)((uint32_t)letter_px * opa) >> 8
                             : (uint32_t)((uint32_t)bpp_opa_table[letter_px] * opa) >> 8;
                }
            }
            else {
                px_opa = 0;
            }

            font_rgb[subpx_cnt] = px_opa;

            subpx_cnt ++;
            if(subpx_cnt == 3) {
                subpx_cnt = 0;

                lv_color_t res_color;
#if LV_COLOR_16_SWAP == 0
                uint8_t bg_rgb[3] = {disp_buf_buf_tmp->ch.red, disp_buf_buf_tmp->ch.green, disp_buf_buf_tmp->ch.blue};
#else
                uint8_t bg_rgb[3] = {disp_buf_buf_tmp->ch.red,
                                     (disp_buf_buf_tmp->ch.green_h << 3) + disp_buf_buf_tmp->ch.green_l,
                                     disp_buf_buf_tmp->ch.blue
                                    };
#endif

#if LV_FONT_SUBPX_BGR
                res_color.ch.blue = (uint32_t)((uint32_t)txt_rgb[0] * font_rgb[0] + (bg_rgb[0] * (255 - font_rgb[0]))) >> 8;
                res_color.ch.red = (uint32_t)((uint32_t)txt_rgb[2] * font_rgb[2] + (bg_rgb[2] * (255 - font_rgb[2]))) >> 8;
#else
                res_color.ch.red = (uint32_t)((uint16_t)txt_rgb[0] * font_rgb[0] + (bg_rgb[0] * (255 - font_rgb[0]))) >> 8;
                res_color.ch.blue = (uint32_t)((uint16_t)txt_rgb[2] * font_rgb[2] + (bg_rgb[2] * (255 - font_rgb[2]))) >> 8;
#endif

#if LV_COLOR_16_SWAP == 0
                res_color.ch.green = (uint32_t)((uint32_t)txt_rgb[1] * font_rgb[1] + (bg_rgb[1] * (255 - font_rgb[1]))) >> 8;
#else
                uint8_t green = (uint32_t)((uint32_t)txt_rgb[1] * font_rgb[1] + (bg_rgb[1] * (255 - font_rgb[1]))) >> 8;
                res_color.ch.green_h = green >> 3;
                res_color.ch.green_l = green & 0x7;
#endif

#if LV_COLOR_DEPTH == 32
                res_color.ch.alpha =  0xff;
#endif

                if(font_rgb[0] == 0 && font_rgb[1] == 0 && font_rgb[2] == 0) mask_buf[mask_p] = LV_OPA_TRANSP;
                else mask_buf[mask_p] = LV_OPA_COVER;
                color_buf[mask_p] = res_color;

                /*Next mask byte*/
                mask_p++;
                disp_buf_buf_tmp++;
            }

            /*Go to the next column*/
            if(col_bit < (int32_t)(8 - bpp)) {
                col_bit += bpp;
                bitmask = bitmask >> bpp;
            }
            else {
                col_bit = 0;
                bitmask = bitmask_init;
                map_p++;
            }
        }

        /*Apply masks if any*/
        if(mask_any) {
            lv_draw_mask_res_t mask_res = lv_draw_mask_apply(mask_buf + mask_p_start, map_area.x1, map_area.y2,
                                                             lv_area_get_width(&map_area));
            if(mask_res == LV_DRAW_MASK_RES_TRANSP) {
                lv_memset_00(mask_buf + mask_p_start, lv_area_get_width(&map_area));
            }
        }

        if((int32_t) mask_p + (col_end - col_start) < mask_buf_size) {
            map_area.y2 ++;
        }
        else {
            _lv_blend_map(clip_area, &map_area, color_buf, mask_buf, LV_DRAW_MASK_RES_CHANGED, opa, blend_mode);

            map_area.y1 = map_area.y2 + 1;
            map_area.y2 = map_area.y1;
            mask_p = 0;
        }

        col_bit += ((box_w - col_end) + col_start) * bpp;

        map_p += (col_bit >> 3);
        col_bit = col_bit & 0x7;

        /*Next row in draw_buf*/
        disp_buf_buf_tmp += disp_buf_width - (col_end - col_start) / 3;
    }

    /*Flush the last part*/
    if(map_area.y1 != map_area.y2) {
        map_area.y2--;
        _lv_blend_map(clip_area, &map_area, color_buf, mask_buf, LV_DRAW_MASK_RES_CHANGED, opa, blend_mode);
    }

    lv_mem_buf_release(mask_buf);
    lv_mem_buf_release(color_buf);
}
#endif

#endif /* DLG_LVGL_USE_GPU_DA1470X */
