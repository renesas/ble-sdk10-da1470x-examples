/**
 ****************************************************************************************
 *
 * @file init_screens.c
 *
 * @brief Initialize screens source file
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
/*
 *      INCLUDES
 *****************************************************************************************
 */

#include "screens/watch_face_screen.h"
#include "screens/menu_list_screen.h"
#include "module.h"
#ifdef PERFORMANCE_METRICS
#include "metrics.h"
#endif
#if TWO_LAYERS_HORIZONTAL_SLIDING
#include "lv_port_disp.h"
#endif

/*
 *  STATIC PROTOTYPES
 *****************************************************************************************
 */
static void lv_propagate_to_children(lv_obj_t *parent, lv_event_code_t code);
static void scroll_event_cb(lv_event_t *e);

/*
 *   GLOBAL FUNCTIONS
 *****************************************************************************************
 */
void create_basic_screens()
{
        /* Initialize Modules*/
        module_init();

        /* Add Modules*/
        module_add(&watch_face);
        module_add(&menu_list);

        lv_obj_t *parent_obj;
        static lv_style_t style_screen;

        /* Create a style for each child screen */
        lv_style_init(&style_screen);
        lv_style_set_border_width(&style_screen, 0);
        lv_style_set_pad_top(&style_screen, 0);
        lv_style_set_pad_bottom(&style_screen, 0);
        lv_style_set_pad_left(&style_screen, 0);
        lv_style_set_pad_right(&style_screen, 0);

        /* Create a parent object containing both watch face and menu screen */
        parent_obj = lv_scr_act();
        lv_obj_set_style_bg_color(parent_obj, lv_color_black(), LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(parent_obj, LV_SCROLLBAR_MODE_OFF);
        lv_obj_set_size(parent_obj, 2 * DEMO_RESX, DEMO_RESY);
        lv_obj_set_pos(parent_obj, 0, 0);
        lv_obj_set_scroll_dir(parent_obj, LV_DIR_HOR);
        lv_obj_set_scroll_snap_x(parent_obj, LV_SCROLL_SNAP_START | LV_SCROLL_SNAP_END);
        lv_obj_add_event_cb(parent_obj, scroll_event_cb, LV_EVENT_ALL, NULL);

        /* Create watch face screen */
        module_prop[0].module->startup(parent_obj, &style_screen, 0, 0);

        /* Create menu list screen */
        module_prop[1].module->startup(parent_obj, &style_screen, DEMO_RESX, 0);
}

/*
 *   STATIC FUNCTIONS
 *****************************************************************************************
 */
static void lv_propagate_to_children(lv_obj_t *parent, lv_event_code_t code)
{
        uint32_t i;
        uint32_t child_cnt = lv_obj_get_child_cnt(parent);
        for (i = 0; i < child_cnt; i++) {
                lv_obj_t *child = lv_obj_get_child(parent, i);
                if (lv_obj_has_flag_any(child, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_FLOATING))
                        continue;
                lv_event_send(child, code, NULL);
        }
}

static void scroll_event_cb(lv_event_t *e)
{
        static int completed_scroll_cnt = 0;
        lv_event_code_t code = lv_event_get_code(e);

#if TWO_LAYERS_HORIZONTAL_SLIDING
        static lv_coord_t prev_sx = 0;
        static int position = 0;
        int posXLayer0 = 0, posXLayer1 = DEMO_RESX;
        static lv_dir_t dir = LV_DIR_NONE;
        lv_coord_t scroll_dx, sx;
#endif

        if (code == LV_EVENT_SCROLL_BEGIN || code == LV_EVENT_SCROLL
                || code == LV_EVENT_SCROLL_END) {
                lv_obj_t *obj = lv_event_get_target(e);
#if TWO_LAYERS_HORIZONTAL_SLIDING
                sx = lv_obj_get_scroll_x(obj);
                scroll_dx = prev_sx - sx;
                prev_sx = sx;
#endif

                if (code == LV_EVENT_SCROLL_BEGIN) {
                        if (completed_scroll_cnt == 0) {
                                lv_propagate_to_children(obj, code);
#ifdef PERFORMANCE_METRICS
                                OS_ENTER_CRITICAL_SECTION();
                                metrics_set_tag(METRICS_TAG_SLIDING_WATCH_FACE_TO_MENU);
                                OS_LEAVE_CRITICAL_SECTION();
#endif

#if TWO_LAYERS_HORIZONTAL_SLIDING
                                /* Change Rect values of all related windows to draw the second FB */
                                if (lv_obj_get_scroll_right(obj) == DEMO_RESX) { //watch -> menu
                                        dir = LV_DIR_RIGHT;
                                }
                                else if (lv_obj_get_scroll_left(obj) == DEMO_RESX) { //menu -> watch
                                        dir = LV_DIR_LEFT;
                                }

                                lv_port_disp_slide_init(obj, dir);
#endif
                        }
                        completed_scroll_cnt++;
                }

#if TWO_LAYERS_HORIZONTAL_SLIDING
                if (code == LV_EVENT_SCROLL) {
                        /* 2 Layers sliding */
                        position += scroll_dx;

                        if (dir == LV_DIR_RIGHT) {
                                posXLayer0 = DEMO_RESX + position;
                        }
                        else if (dir == LV_DIR_LEFT) {
                                posXLayer0 = position - DEMO_RESX;
                        }
                        posXLayer1 = position;

                        lv_port_disp_slide(posXLayer0, posXLayer1);
                }
#endif

                if (code == LV_EVENT_SCROLL_END) {
                        if (completed_scroll_cnt == 1) {
#if TWO_LAYERS_HORIZONTAL_SLIDING
                                /* Reset layer positions */
                                position = 0;
                                posXLayer0 = 0;
                                posXLayer1 = 0;

                                lv_port_disp_slide_end();
#endif
                                lv_propagate_to_children(obj, code);

#ifdef PERFORMANCE_METRICS
                                OS_ENTER_CRITICAL_SECTION();
                                metrics_set_tag(METRICS_TAG_NO_LOGGING);
                                OS_LEAVE_CRITICAL_SECTION();
#endif
                        }
                        completed_scroll_cnt--;
                }
        }
}
