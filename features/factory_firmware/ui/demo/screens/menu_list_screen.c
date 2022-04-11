/**
 ****************************************************************************************
 *
 * @file menu_list_screen.c
 *
 * @brief Menu list screen source file
 *
 * Copyright (C) 2021-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

/*
 *      INCLUDES
 *****************************************************************************************
 */
#include <stdio.h>
#include "Resources.h"
#include "module.h"
#include "timer_screen.h"
#include "activity_screen.h"
#include "compass_screen.h"

void menu_list_screen(lv_obj_t *parent_obj, lv_style_t *style_screen, lv_coord_t x, lv_coord_t y);

/*
 *      TYPEDEFS
 *****************************************************************************************
 */
/* Startup module: menu list */
ModuleItem_Typedef menu_list =
{
        1,
        "menu",
        0,
        menu_list_screen,
        NULL,
};

typedef void (*item_fn)(lv_event_t *e);

/*
 * Bitmaps structure for the radial Menu
 */
typedef struct
{
        const void * src;
        const char *pText;
        const item_fn event_cb;
} ITEM;

/*
 *      MACROS
 *****************************************************************************************
 */
LV_IMG_DECLARE(timer);
LV_IMG_DECLARE(activity);
LV_IMG_DECLARE(compass_menu_icon);
LV_IMG_DECLARE(heart_rate);
LV_IMG_DECLARE(messages);
LV_IMG_DECLARE(sleep_monitor);
LV_IMG_DECLARE(weather);

/*
 *  STATIC PROTOTYPES
 *****************************************************************************************
 */
static void create_cell(lv_obj_t *obj, lv_obj_t *img_obj, const lv_img_dsc_t img_dcs,
        char *text, lv_coord_t x, lv_coord_t y);
static void create_item_text(lv_obj_t *title_obj, const lv_img_dsc_t item_img, lv_coord_t x,
        lv_coord_t y, char *text);

/*
 *  STATIC VARIABLES
 *****************************************************************************************
 */
/*
 * Table of the items that are contained in the Main Menu
 */
static const ITEM item_list[] =
{
        { &timer,              "Timer",       show_timer_cb },
        { &activity,           "Activity",    show_activity_cb },
        { &compass_menu_icon,  "Compass",     show_compass_cb },
        { &heart_rate,         "Heart Rate",  NULL },
        { &messages,           "Messages",    NULL },
        { &sleep_monitor,      "Sleep",       NULL },
        { &weather,            "Weather",     NULL }
};

/*
 *      DEFINES
 *****************************************************************************************
 */
#define ITEMS_PER_SCREEN        (3)
#define MENU_ITEMS_NUM          (sizeof(item_list) / sizeof(ITEM))

/*
 *   GLOBAL FUNCTIONS
 *****************************************************************************************
 */
void menu_list_screen(lv_obj_t *parent_obj, lv_style_t *style_screen, lv_coord_t x, lv_coord_t y)
{
        static lv_coord_t col_dsc[] = { DEMO_RESX, LV_GRID_TEMPLATE_LAST };
        static lv_coord_t row_dsc[MENU_ITEMS_NUM + 1];

        lv_obj_t *menu_list_screen_obj, *cell_obj, *img_obj;
        lv_coord_t item_x, item_y;
        uint8_t text[20];

        menu_list_screen_obj = lv_obj_create(parent_obj);

        lv_obj_remove_style_all(menu_list_screen_obj);
        lv_obj_set_size(menu_list_screen_obj, DEMO_RESX, DEMO_RESY);
        lv_obj_set_pos(menu_list_screen_obj, x, y);
        lv_obj_set_style_bg_color(menu_list_screen_obj, lv_color_black(), LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(menu_list_screen_obj, LV_SCROLLBAR_MODE_OFF);
        lv_obj_add_style(menu_list_screen_obj, style_screen, LV_PART_MAIN);

        /* Create a grid layout */
        for (uint8_t r = 0; r <= MENU_ITEMS_NUM; r++) {
                if (r != MENU_ITEMS_NUM) {
                        row_dsc[r] = DEMO_RESY / ITEMS_PER_SCREEN;
                }
                else {
                        row_dsc[r] = LV_GRID_TEMPLATE_LAST;
                }
        }

        lv_obj_set_style_pad_row(menu_list_screen_obj, 0, LV_PART_MAIN);
        lv_obj_set_grid_dsc_array(menu_list_screen_obj, col_dsc, row_dsc);

        item_x = DEMO_RESX / 5;
        item_y = DEMO_RESY / (3 * ITEMS_PER_SCREEN);

        /* Draw menu items */
        for (uint8_t i = 0; i < MENU_ITEMS_NUM; i++) {
                cell_obj = lv_obj_create(menu_list_screen_obj);
                img_obj = lv_img_create(cell_obj);
                lv_img_set_src(img_obj, item_list[i].src);
                sprintf((char*)text, "#ffffff %s", item_list[i].pText);
                create_cell(cell_obj, img_obj, timer, (char*)text, item_x, item_y);
                if (item_list[i].event_cb != NULL) {
                        lv_obj_add_event_cb(cell_obj, item_list[i].event_cb, LV_EVENT_CLICKED, NULL);
                        lv_obj_add_flag(cell_obj, LV_OBJ_FLAG_CLICKABLE);
                }
        }
}

/*
 *   STATIC FUNCTIONS
 *****************************************************************************************
 */
static void create_cell(lv_obj_t *obj, lv_obj_t *img_obj, const lv_img_dsc_t img_dcs,
        char *text, lv_coord_t x, lv_coord_t y)
{
        static int cell_num = 0;

        lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_bg_color(obj, lv_color_black(), LV_PART_MAIN);
        lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN);
        lv_obj_set_size(obj, DEMO_RESX, DEMO_RESY / 3);
        lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, cell_num, 1);

        lv_obj_set_pos(img_obj, x, y - (img_dcs.header.h / 2));

        lv_obj_t *title = lv_label_create(obj);
        create_item_text(title, img_dcs, x, y, text);

        cell_num++;
}

static void create_item_text(lv_obj_t *title_obj, const lv_img_dsc_t item_img, lv_coord_t x,
        lv_coord_t y, char *text)
{
        lv_label_set_recolor(title_obj, true);
        lv_obj_set_style_text_font(title_obj, &lv_font_montserrat_32, 0);
        lv_label_set_text(title_obj, text);
        lv_obj_set_pos(title_obj, 2 * x,
                y - (item_img.header.h / 2) + (item_img.header.h - 32) / 2);
}
