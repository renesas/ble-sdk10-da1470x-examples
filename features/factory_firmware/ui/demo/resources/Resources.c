/**
 ****************************************************************************************
 *
 * @file Resources.c
 *
 * @brief Resources - bitmaps stored in flash source file
 *
 * Copyright (C) 2021-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#include "Resources.h"

#define AT_XIP_FLASH                    (1)
#define AT_S_FLASH                      (2)
#define RESOURCES_LOCATION              AT_S_FLASH
#if (RESOURCES_LOCATION == AT_XIP_FLASH)
#define RESOURCES_OFFSET                (0x100000)
#define RESOURCES_BASE_ADDRESS          (MEMORY_OQSPIC_S_BASE + RESOURCES_OFFSET)
#elif (RESOURCES_LOCATION == AT_S_FLASH)
#define RESOURCES_OFFSET                (0x0)
#define RESOURCES_BASE_ADDRESS          (MEMORY_QSPIC_BASE + RESOURCES_OFFSET)
#endif

#define HEADER_OFFSET                   (sizeof(lv_img_header_t))

#define CLOCK_BG_BITMAP_OFFSET          (0x0)
#define STAMENS_BITMAP_OFFSET           (0x4A44C)
#define HOUR_BITMAP_OFFSET              (0x58CDC)
#define MINUTE_BITMAP_OFFSET            (0x5A8A0)
#define SECOND_BITMAP_OFFSET            (0x5BC04)
#define TIMER_BITMAP_OFFSET             (0x5C4F8)
#define RESET_TIMER_BITMAP_OFFSET       (0x5EC0C)
#define ACTIVITY_BITMAP_OFFSET          (0x61320)
#define TRACK_BITMAP_OFFSET             (0x63A34)
#define COMPASS_MENU_BITMAP_OFFSET      (0x8AA34)
#define COMPASS_BITMAP_OFFSET           (0x8D148)
#define COMPASS_EARTH_BITMAP_OFFSET     (0xD7594)
#define COMPASS_INDEX_BITMAP_OFFSET     (0xF83B8)
#define HEART_RATE_BITMAP_OFFSET        (0xFBC10)
#define MESSAGES_BITMAP_OFFSET          (0xFE324)
#define SLEEP_MONITOR_BITMAP_OFFSET     (0x100A38)
#define WEATHER_BITMAP_OFFSET           (0x10314C)

#define CLOCK_BG_BITMAP_SIZE            (0x4A44C)
#define STAMENS_BITMAP_SIZE             (0xE890)
#define HOUR_BITMAP_SIZE                (0x1BC4)
#define MINUTE_BITMAP_SIZE              (0x1364)
#define SECOND_BITMAP_SIZE              (0x08F4)
#define TIMER_BITMAP_SIZE               (0x2714)
#define RESET_TIMER_BITMAP_SIZE         (0x2714)
#define ACTIVITY_BITMAP_SIZE            (0x2714)
#define TRACK_BITMAP_SIZE               (0x27000)
#define COMPASS_MENU_BITMAP_SIZE        (0x2714)
#define COMPASS_BITMAP_SIZE             (0x4A44C)
#define COMPASS_EARTH_BITMAP_SIZE       (0x20E24)
#define COMPASS_INDEX_BITMAP_SIZE       (0x3858)
#define HEART_RATE_BITMAP_SIZE          (0x2714)
#define MESSAGES_BITMAP_SIZE            (0x2714)
#define SLEEP_MONITOR_BITMAP_SIZE       (0x2714)
#define WEATHER_BITMAP_SIZE             (0x2714)

const lv_img_dsc_t clock_bg = {
        .header.always_zero = 0,
        .header.w = 390,
        .header.h = 390,
        .data_size = CLOCK_BG_BITMAP_SIZE,
        .header.cf = LV_IMG_CF_TRUE_COLOR,
        .data = (uint8_t*) RESOURCES_BASE_ADDRESS + CLOCK_BG_BITMAP_OFFSET + HEADER_OFFSET, // Pointer to picture data
};

const lv_img_dsc_t stamens = {
        .header.always_zero = 0,
        .header.w = 121,
        .header.h = 123,
        .data_size = STAMENS_BITMAP_SIZE,
        .header.cf = LV_IMG_CF_ARGB8888,
        .data = (uint8_t*) RESOURCES_BASE_ADDRESS + STAMENS_BITMAP_OFFSET + HEADER_OFFSET, // Pointer to picture data
};

const lv_img_dsc_t tick_hour = {
        .header.always_zero = 0,
        .header.w = 16,
        .header.h = 111,
        .data_size = HOUR_BITMAP_SIZE,
        .header.cf = LV_IMG_CF_ARGB8888,
        .data = (uint8_t*) RESOURCES_BASE_ADDRESS + HOUR_BITMAP_OFFSET + HEADER_OFFSET,  // Pointer to picture data
};

const lv_img_dsc_t tick_minute = {
        .header.always_zero = 0,
        .header.w = 8,
        .header.h = 155,
        .data_size = MINUTE_BITMAP_SIZE,
        .header.cf = LV_IMG_CF_ARGB8888,
        .data = (uint8_t*) RESOURCES_BASE_ADDRESS + MINUTE_BITMAP_OFFSET + HEADER_OFFSET, // Pointer to picture data
};

const lv_img_dsc_t tick_second = {
        .header.always_zero = 0,
        .header.w = 4,
        .header.h = 143,
        .data_size = SECOND_BITMAP_SIZE,
        .header.cf = LV_IMG_CF_ARGB8888,
        .data = (uint8_t*) RESOURCES_BASE_ADDRESS + SECOND_BITMAP_OFFSET + HEADER_OFFSET, // Pointer to picture data
};

const lv_img_dsc_t timer = {
        .header.always_zero = 0,
        .header.w = 50,
        .header.h = 50,
        .data_size = TIMER_BITMAP_SIZE,
        .header.cf = LV_IMG_CF_ARGB8888,
        .data = (uint8_t*) RESOURCES_BASE_ADDRESS + TIMER_BITMAP_OFFSET + HEADER_OFFSET, // Pointer to picture data
};

const lv_img_dsc_t reset_timer = {
        .header.always_zero = 0,
        .header.w = 50,
        .header.h = 50,
        .data_size = RESET_TIMER_BITMAP_SIZE,
        .header.cf = LV_IMG_CF_ARGB8888,
        .data = (uint8_t*) RESOURCES_BASE_ADDRESS + RESET_TIMER_BITMAP_OFFSET + HEADER_OFFSET, // Pointer to picture data
};

const lv_img_dsc_t activity = {
        .header.always_zero = 0,
        .header.w = 50,
        .header.h = 50,
        .data_size = ACTIVITY_BITMAP_SIZE,
        .header.cf = LV_IMG_CF_ARGB8888,
        .data = (uint8_t*) RESOURCES_BASE_ADDRESS + ACTIVITY_BITMAP_OFFSET + HEADER_OFFSET, // Pointer to picture data
};

const lv_img_dsc_t track = {
        .header.always_zero = 0,
        .header.w = 245,
        .header.h = 326,
        .data_size = TRACK_BITMAP_SIZE,
        .header.cf = LV_IMG_CF_TRUE_COLOR,
        .data = (uint8_t*) RESOURCES_BASE_ADDRESS + TRACK_BITMAP_OFFSET + HEADER_OFFSET, // Pointer to picture data
};

const lv_img_dsc_t compass_menu_icon = {
        .header.always_zero = 0,
        .header.w = 50,
        .header.h = 50,
        .data_size = COMPASS_MENU_BITMAP_SIZE,
        .header.cf = LV_IMG_CF_ARGB8888,
        .data = (uint8_t*) RESOURCES_BASE_ADDRESS + COMPASS_MENU_BITMAP_OFFSET + HEADER_OFFSET, // Pointer to picture data
};

const lv_img_dsc_t compass = {
        .header.always_zero = 0,
        .header.w = 390,
        .header.h = 390,
        .data_size = COMPASS_BITMAP_SIZE,
        .header.cf = LV_IMG_CF_TRUE_COLOR,
        .data = (uint8_t*) RESOURCES_BASE_ADDRESS + COMPASS_BITMAP_OFFSET + HEADER_OFFSET, // Pointer to picture data
};

const lv_img_dsc_t compass_earth = {
        .header.always_zero = 0,
        .header.w = 183,
        .header.h = 184,
        .data_size = COMPASS_EARTH_BITMAP_SIZE,
        .header.cf = LV_IMG_CF_ARGB8888,
        .data = (uint8_t*) RESOURCES_BASE_ADDRESS + COMPASS_EARTH_BITMAP_OFFSET + HEADER_OFFSET, // Pointer to picture data
};

const lv_img_dsc_t compass_index = {
        .header.always_zero = 0,
        .header.w = 35,
        .header.h = 103,
        .data_size = COMPASS_INDEX_BITMAP_SIZE,
        .header.cf = LV_IMG_CF_ARGB8888,
        .data = (uint8_t*) RESOURCES_BASE_ADDRESS + COMPASS_INDEX_BITMAP_OFFSET + HEADER_OFFSET, // Pointer to picture data
};

const lv_img_dsc_t heart_rate = {
        .header.always_zero = 0,
        .header.w = 50,
        .header.h = 50,
        .data_size = HEART_RATE_BITMAP_SIZE,
        .header.cf = LV_IMG_CF_ARGB8888,
        .data = (uint8_t*) RESOURCES_BASE_ADDRESS + HEART_RATE_BITMAP_OFFSET + HEADER_OFFSET, // Pointer to picture data
};

const lv_img_dsc_t messages = {
        .header.always_zero = 0,
        .header.w = 50,
        .header.h = 50,
        .data_size = MESSAGES_BITMAP_SIZE,
        .header.cf = LV_IMG_CF_ARGB8888,
        .data = (uint8_t*) RESOURCES_BASE_ADDRESS + MESSAGES_BITMAP_OFFSET + HEADER_OFFSET, // Pointer to picture data
};

const lv_img_dsc_t sleep_monitor = {
        .header.always_zero = 0,
        .header.w = 50,
        .header.h = 50,
        .data_size = SLEEP_MONITOR_BITMAP_SIZE,
        .header.cf = LV_IMG_CF_ARGB8888,
        .data = (uint8_t*) RESOURCES_BASE_ADDRESS + SLEEP_MONITOR_BITMAP_OFFSET + HEADER_OFFSET, // Pointer to picture data
};

const lv_img_dsc_t weather = {
        .header.always_zero = 0,
        .header.w = 50,
        .header.h = 50,
        .data_size = WEATHER_BITMAP_SIZE,
        .header.cf = LV_IMG_CF_ARGB8888,
        .data = (uint8_t*) RESOURCES_BASE_ADDRESS + WEATHER_BITMAP_OFFSET + HEADER_OFFSET, // Pointer to picture data
};
