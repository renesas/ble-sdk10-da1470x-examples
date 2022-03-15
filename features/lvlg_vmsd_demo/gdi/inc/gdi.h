/**
 * \addtogroup UI
 * \{
 * \addtogroup GDI
 *
 * \brief Graphics Device Interface
 * \{
 */
/**
 ****************************************************************************************
 *
 * @file gdi.h
 *
 * @brief Graphics Device Interface
 *
 * Copyright (C) 2020-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef GDI_H_
#define GDI_H_

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "osal.h"
#ifdef OS_BAREMETAL
#include "osal_baremetal.h"
#endif
#include "ad_lcdc.h"

/**********************************************************************
 *
 *       Types
 *
 **********************************************************************
 */
#ifndef GDI_DRV_CONF_ATTR
#define GDI_DRV_CONF_ATTR       const
#endif

typedef uint16_t gdi_coord_t;

typedef enum {
#if (DEVICE_FAMILY == DA1469X)
        GDI_FORMAT_L1,
        GDI_FORMAT_L4,
        GDI_FORMAT_L8,
#endif
        GDI_FORMAT_RGB332,
        GDI_FORMAT_RGB565,
        GDI_FORMAT_RGBA5551,
#if (DEVICE_FAMILY == DA1470X)
        GDI_FORMAT_RGB888,
        GDI_FORMAT_RGBA4444,
        GDI_FORMAT_ARGB4444,
#endif
        GDI_FORMAT_ARGB8888,
        GDI_FORMAT_RGBA8888,
        GDI_FORMAT_ABGR8888,
        GDI_FORMAT_BGRA8888,
} gdi_color_fmt_t;


/**
 * \brief Touch event data
 */
typedef struct {
        gdi_coord_t x;      /**< X position of the touch event in display's coordinates */
        gdi_coord_t y;      /**< Y position of the touch event in display's coordinates */
        uint8_t pressed;    /**< Designates whether the display is pressed or not */
} gdi_touch_data_t;

/**
 * Callback function for the asynchronous update of the LCD
 *
 * \param[in] user_data         User data as provided with the update function
 *
 * \sa gdi_display_update_async()
 */
typedef void (*draw_callback)(bool underflow, void *user_data);

/**
 * Callback function for storing touch events into the graphics buffers
 *
 * \param[in] touch_data      Pointer to touch data
 *
 * \sa gdi_set_callback_store_touch()
 */
typedef void (*store_touch_callback)(gdi_touch_data_t *touch_data);

#if dg_configLCDC_ADAPTER
typedef void (*screen_set_partial_update_area_callback)(hw_lcdc_frame_t *frame);

/**
 * Callback function to configure display's color mode
 *
 * \param[in]  color_mode  Color mode to configure
 *
 * \sa gdi_set_color_mode()
 */
typedef bool (*screen_set_color_mode_callback)(HW_LCDC_OUTPUT_COLOR_MODE color_mode);

/**
 * \brief GDI type definition
 */
typedef struct {
        size_t single_buff_sz;                          /**< Size of a single frame buffer in bytes  */
        uint8_t active_buf;                             /**< Active frame buffer */
        uint8_t bufs_num;                               /**< Number of buffers available */
        hw_lcdc_layer_t layer;                          /**< Active layer */
        bool layer_dirty;
        bool layer_enable;
        uint8_t **buffer;                               /**< Frame buffer pointers */
} gdi_layer_t;

typedef struct {
        const ad_lcdc_controller_conf_t *config;
        const uint8_t *screen_init_cmds;
        const uint8_t *screen_power_on_cmds;
        const uint8_t *screen_enable_cmds;
        const uint8_t *screen_disable_cmds;
        const uint8_t *screen_power_off_cmds;
        const uint8_t *screen_clear_cmds;
        const HW_LCDC_OUTPUT_COLOR_MODE *screen_color_modes;
        screen_set_partial_update_area_callback screen_set_partial_update_area;
        screen_set_color_mode_callback screen_set_color_mode;
        uint16_t screen_color_modes_len;
        uint16_t screen_init_cmds_len;
        uint16_t screen_power_on_cmds_len;
        uint16_t screen_enable_cmds_len;
        uint16_t screen_disable_cmds_len;
        uint16_t screen_power_off_cmds_len;
        uint16_t screen_clear_cmds_len;
        int16_t screen_offsetx;
        int16_t screen_offsety;
        gdi_coord_t width;                              /**< Width of the LCD in pixels */
        gdi_coord_t height;                             /**< Height of the LCD in pixels */
        bool display_powered;                           /**< Display power state */
        bool display_enabled;                           /**< Display state */
        bool continuous_mode_enable_current;            /**< Continuous mode current state */
        bool continuous_mode_enable_config;             /**< Continuous mode declaration state*/
        ad_lcdc_handle_t display_h;                     /**< Display handle */
        void *touch_h;                                  /**< Touch controller handle */
        hw_lcdc_frame_t frame;                          /**< Active frame dimensions */
#ifndef OS_BAREMETAL
        OS_MUTEX block_mutex_display_status;            /**< MUTEX for display status */
        OS_MUTEX block_mutex_multiplex_display_touch;   /**< MUTEX for display operations used when the display and touch controllers are multiplexed */
#endif
        OS_EVENT dma_event;                             /**< Event for DMA transactions */
        OS_EVENT draw_smphr;                            /**< Semaphore for asynchronous drawing operations */
        OS_EVENT draw_event;                            /**< Event for synchronous/asynchronous drawing operations */
        draw_callback draw_cb;
        store_touch_callback store_touch_cb;
        void *user_data;
        gdi_layer_t layer[HW_LCDC_LAYER_MAX];
#if dg_configLCDC_TEST_APP
        uint32_t bg_color;
#endif
        bool underflow;
} gdi_t;

typedef struct {
        char *name;
        const ad_lcdc_controller_conf_t *config;
        int16_t offsetx;
        int16_t offsety;
        bool continuous_mode_config; /**< Continuous mode declaration state */
        const uint8_t *init_cmds;
        const uint8_t *power_on_cmds;
        const uint8_t *enable_cmds;
        const uint8_t *disable_cmds;
        const uint8_t *power_off_cmds;
        const uint8_t *clear_cmds;
        const HW_LCDC_OUTPUT_COLOR_MODE *color_modes;
        screen_set_partial_update_area_callback set_partial_update_area;
        screen_set_color_mode_callback set_color_mode;
        uint16_t color_modes_len;
        uint16_t init_cmds_len;
        uint16_t power_on_cmds_len;
        uint16_t enable_cmds_len;
        uint16_t disable_cmds_len;
        uint16_t power_off_cmds_len;
        uint16_t clear_cmds_len;
} gdi_lcd_entry;

/* Display entries */
typedef const gdi_lcd_entry ** gdi_display_entries;
#endif /* dg_configLCDC_ADAPTER */

#if dg_configLCDC_TEST_APP
#define GDI_BG_COLOR_ALPHA_WIDTH    (8UL)
#define GDI_BG_COLOR_BLUE_WIDTH     (8UL)
#define GDI_BG_COLOR_GREEN_WIDTH    (8UL)
#define GDI_BG_COLOR_RED_WIDTH      (8UL)

/* Choose an arbitrary color channel position */
#define GDI_BG_COLOR_BLUE_POS       (24UL)
#define GDI_BG_COLOR_GREEN_POS      (16UL)
#define GDI_BG_COLOR_RED_POS        (8UL)
#define GDI_BG_COLOR_ALPHA_POS      (0UL)

#define GDI_BG_COLOR_ALPHA_MSK      (((1 << GDI_BG_COLOR_ALPHA_WIDTH) - 1) << GDI_BG_COLOR_ALPHA_POS)
#define GDI_BG_COLOR_BLUE_MSK       (((1 << GDI_BG_COLOR_BLUE_WIDTH ) - 1) << GDI_BG_COLOR_BLUE_POS )
#define GDI_BG_COLOR_GREEN_MSK      (((1 << GDI_BG_COLOR_GREEN_WIDTH) - 1) << GDI_BG_COLOR_GREEN_POS)
#define GDI_BG_COLOR_RED_MSK        (((1 << GDI_BG_COLOR_RED_WIDTH  ) - 1) << GDI_BG_COLOR_RED_POS  )

#define GDI_BG_COLOR_FIELD(rgba, field)      ((rgba & GDI_BG_COLOR_ ## field ## _MSK) >> GDI_BG_COLOR_ ## field ## _POS)

/**
 * \brief Get the alpha field of a formatted BG color value
 */
#define GDI_BG_COLOR_ALPHA_FIELD(rgba)      GDI_BG_COLOR_FIELD(rgba, ALPHA)
/**
 * \brief Get the blue field of a formatted BG color value
 */
#define GDI_BG_COLOR_BLUE_FIELD(rgba)       GDI_BG_COLOR_FIELD(rgba, BLUE)
/**
 * \brief Get the green field of a formatted BG color value
 */
#define GDI_BG_COLOR_GREEN_FIELD(rgba)      GDI_BG_COLOR_FIELD(rgba, GREEN)
/**
 * \brief Get the red field of a formatted BG color value
 */
#define GDI_BG_COLOR_RED_FIELD(rgba)        GDI_BG_COLOR_FIELD(rgba, RED)

/**
 * \brief Format controller's BG color into a 32-bit value
 *
 * \param[in] r  Red color channel value (max value is 255)
 * \param[in] g  Green color channel value (max value is 255)
 * \param[in] b  Blue color channel value (max value is 255)
 * \param[in] a  Alpha color channel value (max value is 255)
 */
#define GDI_BG_COLOR_FORMAT(r, g, b, a)     ( ((r << GDI_BG_COLOR_RED_POS  ) & GDI_BG_COLOR_RED_MSK   )   \
                                            | ((g << GDI_BG_COLOR_GREEN_POS) & GDI_BG_COLOR_GREEN_MSK )   \
                                            | ((b << GDI_BG_COLOR_BLUE_POS ) & GDI_BG_COLOR_BLUE_MSK  )   \
                                            | ((a << GDI_BG_COLOR_ALPHA_POS) & GDI_BG_COLOR_ALPHA_MSK ) )
#endif /* dg_configLCDC_TEST_APP */

#include "gdi_config.h"

/**********************************************************************
 *
 *       Defines
 *
 **********************************************************************
 */
#ifndef GDI_TOUCH_ENABLE
#define GDI_TOUCH_ENABLE                (0)
#endif

#define CF_NATIVE_AUTO                  (0)
#define CF_NATIVE_RGB332                (3)
#define CF_NATIVE_RGB565                (4)
#define CF_NATIVE_RGBA8888              (5)
#define CF_NATIVE_ARGB8888              (6)

#ifndef GDI_FB_COLOR_FORMAT
#define GDI_FB_COLOR_FORMAT             (CF_NATIVE_AUTO)
#endif

#define GDI_TOUCH_INTERFACE_I2C         (0)
#define GDI_TOUCH_INTERFACE_SPI         (1)
#define GDI_TOUCH_INTERFACE_OTHER       (2)

#if GDI_FB_COLOR_FORMAT == CF_NATIVE_AUTO
#undef GDI_FB_COLOR_FORMAT
#define ___GDI_EXTEND_COLOR(x)          _GDI_##x
#define __GDI_EXTEND_COLOR(x)           ___GDI_EXTEND_COLOR x
#define _GDI_EXTEND_COLOR(x)            __GDI_EXTEND_COLOR(x)

#define _GDI_HW_LCDC_OCM_8RGB111_1      (0)
#define _GDI_HW_LCDC_OCM_8RGB111_2      (1)
#define _GDI_HW_LCDC_OCM_RGB111         (2)
#define _GDI_HW_LCDC_OCM_L1             (3)
#define _GDI_HW_LCDC_OCM_8RGB332        (4)
#define _GDI_HW_LCDC_OCM_8RGB444        (5)
#define _GDI_HW_LCDC_OCM_8RGB565        (6)
#define _GDI_HW_LCDC_OCM_8RGB666        (7)
#define _GDI_HW_LCDC_OCM_8RGB888        (8)
#define _GDI_HW_LCDC_OCM_RGB222         (9)

#if _GDI_EXTEND_COLOR(GDI_DISP_COLOR) == _GDI_EXTEND_COLOR((HW_LCDC_OCM_L1))
#define GDI_FB_COLOR_FORMAT                (CF_NATIVE_RGB332)
#elif (_GDI_EXTEND_COLOR(GDI_DISP_COLOR) == _GDI_EXTEND_COLOR((HW_LCDC_OCM_8RGB111_1))) \
        || (_GDI_EXTEND_COLOR(GDI_DISP_COLOR) == _GDI_EXTEND_COLOR((HW_LCDC_OCM_8RGB111_2))) \
        || (_GDI_EXTEND_COLOR(GDI_DISP_COLOR) == _GDI_EXTEND_COLOR((HW_LCDC_OCM_RGB111))) \
        || (_GDI_EXTEND_COLOR(GDI_DISP_COLOR) == _GDI_EXTEND_COLOR((HW_LCDC_OCM_RGB222))) \
        || (_GDI_EXTEND_COLOR(GDI_DISP_COLOR) == _GDI_EXTEND_COLOR((HW_LCDC_OCM_8RGB332)))
#define GDI_FB_COLOR_FORMAT                (CF_NATIVE_RGB332)
#elif _GDI_EXTEND_COLOR(GDI_DISP_COLOR) == _GDI_EXTEND_COLOR((HW_LCDC_OCM_8RGB444)) \
        || _GDI_EXTEND_COLOR(GDI_DISP_COLOR) == _GDI_EXTEND_COLOR((HW_LCDC_OCM_8RGB565))
#define GDI_FB_COLOR_FORMAT                (CF_NATIVE_RGB565)
#elif _GDI_EXTEND_COLOR(GDI_DISP_COLOR) == _GDI_EXTEND_COLOR((HW_LCDC_OCM_8RGB666)) \
        || _GDI_EXTEND_COLOR(GDI_DISP_COLOR) == _GDI_EXTEND_COLOR((HW_LCDC_OCM_8RGB888))
#define GDI_FB_COLOR_FORMAT                (CF_NATIVE_RGBA8888)
#endif /* GDI_DISP_COLOR */
#endif /* GDI_FB_COLOR_FORMAT */

#ifndef GDI_SINGLE_FB_NUM
#define GDI_SINGLE_FB_NUM               (2)
#endif

#ifndef GDI_GUI_HEAP_SIZE
#define GDI_GUI_HEAP_SIZE               (0)
#endif

#define GDI_RGB332_COLOR_BYTES          (1UL)
#define GDI_RGB332_RED_POS              (5UL)
#define GDI_RGB332_GREEN_POS            (2UL)
#define GDI_RGB332_BLUE_POS             (0UL)
#define GDI_RGB332_ALPHA_POS            (0UL)
#define GDI_RGB332_RED_WIDTH            (3UL)
#define GDI_RGB332_GREEN_WIDTH          (3UL)
#define GDI_RGB332_BLUE_WIDTH           (2UL)
#define GDI_RGB332_ALPHA_WIDTH          (0UL)

#define GDI_RGB565_COLOR_BYTES          (2UL)
#define GDI_RGB565_RED_POS              (11UL)
#define GDI_RGB565_GREEN_POS            (5UL)
#define GDI_RGB565_BLUE_POS             (0UL)
#define GDI_RGB565_ALPHA_POS            (0UL)
#define GDI_RGB565_RED_WIDTH            (5UL)
#define GDI_RGB565_GREEN_WIDTH          (6UL)
#define GDI_RGB565_BLUE_WIDTH           (5UL)
#define GDI_RGB565_ALPHA_WIDTH          (0UL)

#define GDI_RGB666_COLOR_BYTES          (3UL)
#define GDI_RGB666_RED_POS              (16UL)
#define GDI_RGB666_GREEN_POS            (8UL)
#define GDI_RGB666_BLUE_POS             (0UL)
#define GDI_RGB666_ALPHA_POS            (0UL)
#define GDI_RGB666_RED_WIDTH            (6UL)
#define GDI_RGB666_GREEN_WIDTH          (6UL)
#define GDI_RGB666_BLUE_WIDTH           (6UL)
#define GDI_RGB666_ALPHA_WIDTH          (0UL)

#define GDI_RGB888_COLOR_BYTES          (3UL)
#define GDI_RGB888_RED_POS              (16UL)
#define GDI_RGB888_GREEN_POS            (8UL)
#define GDI_RGB888_BLUE_POS             (0UL)
#define GDI_RGB888_ALPHA_POS            (0UL)
#define GDI_RGB888_RED_WIDTH            (8UL)
#define GDI_RGB888_GREEN_WIDTH          (8UL)
#define GDI_RGB888_BLUE_WIDTH           (8UL)
#define GDI_RGB888_ALPHA_WIDTH          (0UL)

#define GDI_BGR888_COLOR_BYTES          (3UL)
#define GDI_BGR888_RED_POS              (0UL)
#define GDI_BGR888_GREEN_POS            (8UL)
#define GDI_BGR888_BLUE_POS             (16UL)
#define GDI_BGR888_ALPHA_POS            (0UL)
#define GDI_BGR888_RED_WIDTH            (8UL)
#define GDI_BGR888_GREEN_WIDTH          (8UL)
#define GDI_BGR888_BLUE_WIDTH           (8UL)
#define GDI_BGR888_ALPHA_WIDTH          (0UL)

#define GDI_RGBA8888_COLOR_BYTES        (4UL)
#define GDI_RGBA8888_RED_POS            (24UL)
#define GDI_RGBA8888_GREEN_POS          (16UL)
#define GDI_RGBA8888_BLUE_POS           (8UL)
#define GDI_RGBA8888_ALPHA_POS          (0UL)
#define GDI_RGBA8888_RED_WIDTH          (8UL)
#define GDI_RGBA8888_GREEN_WIDTH        (8UL)
#define GDI_RGBA8888_BLUE_WIDTH         (8UL)
#define GDI_RGBA8888_ALPHA_WIDTH        (8UL)

#define GDI_ARGB8888_COLOR_BYTES        (4UL)
#define GDI_ARGB8888_ALPHA_POS          (24UL)
#define GDI_ARGB8888_RED_POS            (16UL)
#define GDI_ARGB8888_GREEN_POS          (8UL)
#define GDI_ARGB8888_BLUE_POS           (0UL)
#define GDI_ARGB8888_RED_WIDTH          (8UL)
#define GDI_ARGB8888_GREEN_WIDTH        (8UL)
#define GDI_ARGB8888_BLUE_WIDTH         (8UL)
#define GDI_ARGB8888_ALPHA_WIDTH        (8UL)

#define GDI_BGRA8888_COLOR_BYTES        (4UL)
#define GDI_BGRA8888_ALPHA_POS          (0UL)
#define GDI_BGRA8888_RED_POS            (8UL)
#define GDI_BGRA8888_GREEN_POS          (16UL)
#define GDI_BGRA8888_BLUE_POS           (24UL)
#define GDI_BGRA8888_RED_WIDTH          (8UL)
#define GDI_BGRA8888_GREEN_WIDTH        (8UL)
#define GDI_BGRA8888_BLUE_WIDTH         (8UL)
#define GDI_BGRA8888_ALPHA_WIDTH        (8UL)

#define GDI_ABGR8888_COLOR_BYTES        (4UL)
#define GDI_ABGR8888_ALPHA_POS          (24UL)
#define GDI_ABGR8888_RED_POS            (0UL)
#define GDI_ABGR8888_GREEN_POS          (8UL)
#define GDI_ABGR8888_BLUE_POS           (16UL)
#define GDI_ABGR8888_RED_WIDTH          (8UL)
#define GDI_ABGR8888_GREEN_WIDTH        (8UL)
#define GDI_ABGR8888_BLUE_WIDTH         (8UL)
#define GDI_ABGR8888_ALPHA_WIDTH        (8UL)

#define _GDI_F_COLOR_BYTES(f)           GDI_ ## f ## _COLOR_BYTES
#define _GDI_F_COLOR_POS(f, c)          GDI_ ## f ## _ ## c ## _POS
#define _GDI_F_COLOR_WIDTH(f, c)        GDI_ ## f ## _ ## c ## _WIDTH
#define GDI_F_COLOR_BYTES(f)            _GDI_F_COLOR_BYTES(f)
#define GDI_F_COLOR_POS(f, c)           _GDI_F_COLOR_POS(f, c)
#define GDI_F_COLOR_WIDTH(f, c)         _GDI_F_COLOR_WIDTH(f, c)
#define GDI_F_FIELD(f, c)               ((unsigned)((1 << GDI_ ## f ## _ ## c ## _WIDTH) - 1) << GDI_ ## f ## _ ## c ## _POS)

#define GDI_F_RED_FIELD(f)              (GDI_F_FIELD(f, RED))
#define GDI_F_GREEN_FIELD(f)            (GDI_F_FIELD(f, GREEN))
#define GDI_F_BLUE_FIELD(f)             (GDI_F_FIELD(f, BLUE))
#define GDI_F_COLOR_FIELD(f)            (GDI_F_FIELD(f, RED) | GDI_FIELD(f, GREEN) | GDI_FIELD(f, BLUE))
#define GDI_F_ALPHA_FIELD(f)            (GDI_F_FIELD(f, ALPHA))

#if GDI_FB_COLOR_FORMAT == CF_NATIVE_RGB332
#define GDI_FORMAT                      RGB332
#elif GDI_FB_COLOR_FORMAT == CF_NATIVE_RGB565
#define GDI_FORMAT                      RGB565
#elif GDI_FB_COLOR_FORMAT == CF_NATIVE_RGBA8888
#define GDI_FORMAT                      RGBA8888
#elif GDI_FB_COLOR_FORMAT == CF_NATIVE_ARGB8888
#define GDI_FORMAT                      ARGB8888
#endif /* GDI_FB_COLOR_FORMAT */

#define GDI_COLOR_BYTES                 GDI_F_COLOR_BYTES(GDI_FORMAT)
#define GDI_ALPHA_POS                   GDI_F_COLOR_POS(GDI_FORMAT, ALPHA)
#define GDI_RED_POS                     GDI_F_COLOR_POS(GDI_FORMAT, RED)
#define GDI_GREEN_POS                   GDI_F_COLOR_POS(GDI_FORMAT, GREEN)
#define GDI_BLUE_POS                    GDI_F_COLOR_POS(GDI_FORMAT, BLUE)
#define GDI_ALPHA_WIDTH                 GDI_F_COLOR_WIDTH(GDI_FORMAT, ALPHA)
#define GDI_RED_WIDTH                   GDI_F_COLOR_WIDTH(GDI_FORMAT, RED)
#define GDI_GREEN_WIDTH                 GDI_F_COLOR_WIDTH(GDI_FORMAT, GREEN)
#define GDI_BLUE_WIDTH                  GDI_F_COLOR_WIDTH(GDI_FORMAT, BLUE)

#define GDI_RED_FIELD                   GDI_F_RED_FIELD(GDI_FORMAT)
#define GDI_GREEN_FIELD                 GDI_F_GREEN_FIELD(GDI_FORMAT)
#define GDI_BLUE_FIELD                  GDI_F_BLUE_FIELD(GDI_FORMAT)
#define GDI_COLOR_FIELD                 GDI_F_COLOR_FIELD(GDI_FORMAT)
#define GDI_ALPHA_FIELD                 GDI_F_ALPHA_FIELD(GDI_FORMAT)

typedef enum {
        GDI_COLOR_TRANS   = 0x0UL,
        GDI_COLOR_BLACK   = GDI_ALPHA_FIELD,
        GDI_COLOR_RED     = GDI_ALPHA_FIELD | GDI_RED_FIELD |             0UL |            0UL,
        GDI_COLOR_YELLOW  = GDI_ALPHA_FIELD | GDI_RED_FIELD | GDI_GREEN_FIELD |            0UL,
        GDI_COLOR_GREEN   = GDI_ALPHA_FIELD |           0UL | GDI_GREEN_FIELD |            0UL,
        GDI_COLOR_CYAN    = GDI_ALPHA_FIELD |           0UL | GDI_GREEN_FIELD | GDI_BLUE_FIELD,
        GDI_COLOR_BLUE    = GDI_ALPHA_FIELD |           0UL |             0UL | GDI_BLUE_FIELD,
        GDI_COLOR_MAGENTA = GDI_ALPHA_FIELD | GDI_RED_FIELD |             0UL | GDI_BLUE_FIELD,
        GDI_COLOR_WHITE   = GDI_ALPHA_FIELD | GDI_RED_FIELD | GDI_GREEN_FIELD | GDI_BLUE_FIELD,
} gdi_color_t;

#ifndef GDI_FB_RESX
#define GDI_FB_RESX                             (GDI_DISP_RESX)
#endif

#ifndef GDI_FB_RESY
#define GDI_FB_RESY                             (GDI_DISP_RESY)
#endif

#if dg_configLCDC_TEST_APP
/*
 * \brief Change alpha channel value
 *
 */
#define GDI_SET_ALPHA_VALUE(color, alpha)    ((color & ~GDI_ALPHA_FIELD) | ((alpha << GDI_ALPHA_POS) & GDI_ALPHA_FIELD))

#define GDI_GAMMA_LUT_BLUE_WIDTH     GDI_BLUE_WIDTH
#define GDI_GAMMA_LUT_GREEN_WIDTH    GDI_GREEN_WIDTH
#define GDI_GAMMA_LUT_RED_WIDTH      GDI_RED_WIDTH
#define GDI_GAMMA_LUT_ALPHA_WIDTH    GDI_ALPHA_WIDTH

#define GDI_GAMMA_LUT_BLUE_POS       GDI_BLUE_POS
#define GDI_GAMMA_LUT_GREEN_POS      GDI_GREEN_POS
#define GDI_GAMMA_LUT_RED_POS        GDI_RED_POS
#define GDI_GAMMA_LUT_ALPHA_POS      GDI_ALPHA_POS

#define GDI_GAMMA_LUT_ALPHA_MSK      (((1 << GDI_GAMMA_LUT_ALPHA_WIDTH) - 1) << GDI_GAMMA_LUT_ALPHA_POS)
#define GDI_GAMMA_LUT_BLUE_MSK       (((1 << GDI_GAMMA_LUT_BLUE_WIDTH ) - 1) << GDI_GAMMA_LUT_BLUE_POS )
#define GDI_GAMMA_LUT_GREEN_MSK      (((1 << GDI_GAMMA_LUT_GREEN_WIDTH) - 1) << GDI_GAMMA_LUT_GREEN_POS)
#define GDI_GAMMA_LUT_RED_MSK        (((1 << GDI_GAMMA_LUT_RED_WIDTH  ) - 1) << GDI_GAMMA_LUT_RED_POS  )

#define GDI_GAMMA_LUT_FIELD(color, field)    ((color << GDI_GAMMA_LUT_ ## field ## _POS) & GDI_GAMMA_LUT_ ## field ## _MSK)

#define GDI_GAMMA_LUT_ALPHA_FIELD(value)   GDI_GAMMA_LUT_FIELD(value, ALPHA)
#define GDI_GAMMA_LUT_BLUE_FIELD(value)    GDI_GAMMA_LUT_FIELD(value, BLUE )
#define GDI_GAMMA_LUT_GREEN_FIELD(value)   GDI_GAMMA_LUT_FIELD(value, GREEN)
#define GDI_GAMMA_LUT_RED_FIELD(value)     GDI_GAMMA_LUT_FIELD(value, RED  )

#endif /* dg_configLCDC_TEST_APP */

#if dg_configLCDC_ADAPTER
/**
 * \brief GDI initialization
 *
 * Initializes the GDI instance, allocate memory and set default background color
 */
gdi_t *gdi_init(void);

size_t gdi_setup_layer(HW_LCDC_LAYER layer_no, void *address, gdi_coord_t resx, gdi_coord_t resy, gdi_color_fmt_t format, int buffers);

/**
 * \brief GDI task notification
 *
 * Sends a notification to the GDI task. This routine should be called from within an ISR.
 */
void gdi_touch_event(void);

/**
 * \brief Set callback function used to store touch events into the graphics buffers.
 *
 * \param[in] cb  User-defined callback function
 */
void gdi_set_callback_store_touch(store_touch_callback cb);

/**
 * \brief Get system uptime in ticks
 *
 * \return The uptime ticks
 */
uint64_t gdi_get_sys_uptime_ticks(void);

/**
 * \brief Convert system ticks in microseconds
 *
 * \param[in] ticks             System ticks to convert
 *
 * \return Calculated time in microseconds
 */
uint64_t gdi_convert_ticks_to_us(uint64_t ticks);

/**
 * \brief Power on the display
 */
void gdi_display_power_on(void);

/**
 * \brief Power off the display
 */
void gdi_display_power_off(void);

/**
 * \brief Checks if display is powered.
 *
 * \return True if display is powered, false otherwise.
 */
bool gdi_display_is_powered(void);

/**
 * \brief Enable the display
 */
void gdi_display_enable(void);

/**
 * \brief Disable the display
 */
void gdi_display_disable(void);

/**
 * \brief Checks if display is enabled.
 *
 * \return True if display is enabled, false otherwise.
 */
bool gdi_display_is_enabled(void);

/**
 * \brief Update the display (synchronously)
 *
 * Move data from the active frame buffer to the display's internal memory.
 */
void gdi_display_update(void);

/**
 * \brief Update the display (asynchronously)
 *
 * Move data from the active frame buffer to the display's internal memory. User can register
 * their own callback function and pass their own data.
 *
 * \param[in] cb          User-defined callback function to be called once the frame update is done.
 *
 * \param[in] user_data   User-defined data to be passed in the registered callback function.
 */
void gdi_display_update_async(draw_callback cb, void *user_data);

/**
 * \brief Modifies the provided partial update area parameters according to the LCD requirements
 *
 * \param[in,out] x0 First column of the area to be updated
 * \param[in,out] y0 First row of the area to be updated
 * \param[in,out] x1 Last column of the area to be updated
 * \param[in,out] y1 Last row of the area to be updated
 */
void gdi_round_partial_update_area(gdi_coord_t *x0, gdi_coord_t *y0, gdi_coord_t *x1, gdi_coord_t *y1);

/**
 * \brief Set area of the LCD that will be updated
 *
 * \param[in] x0     First column of the area to be updated
 * \param[in] y0     First row of the area to be updated
 * \param[in] x1     Last column of the area to be updated
 * \param[in] y1     Last row of the area to be updated
 */
void gdi_set_partial_update_area(gdi_coord_t x0, gdi_coord_t y0, gdi_coord_t x1, gdi_coord_t y1);

/**
 * \brief Clear display
 *
 * Clear the display
 */
void gdi_display_clear(void);

/**
 * \brief Select the next available frame buffer
 *
 * \return Number of the currently selected frame buffer
 */
uint8_t gdi_set_next_frame_buffer(HW_LCDC_LAYER layer_no);

/**
 * \brief Get the number of the current frame buffer
 *
 * \return Number of the currently selected frame buffer
 */
uint8_t gdi_get_current_frame_buffer(HW_LCDC_LAYER layer_no);

/**
 * \brief Set active frame buffer
 *
 * \param [in] frame            number of frame buffer
 */
void gdi_set_frame_buffer(HW_LCDC_LAYER layer_no, uint8_t frame);

/**
 * \brief Get the address of the assigned frame buffers area
 */
void *gdi_get_frame_buffer_addr(HW_LCDC_LAYER layer_no);

/**
 * \brief Get the starting address of assigned heap area
 */
void *gdi_get_gui_heap_addr(void);

/**
 * \brief Translate GDI to layer color format
 *
 * \param[in]  format   GDI color format
 *
 * \return Translated layer color format
 */
HW_LCDC_LAYER_COLOR_MODE gdi_to_layer_color_format(gdi_color_fmt_t format);

/**
 * \brief Translate layer to GDI color format
 *
 * \param[in]  format   Layer color format
 *
 * \return Translated GDI color format
 */
gdi_color_fmt_t gdi_from_layer_color_format(HW_LCDC_LAYER_COLOR_MODE format);

/**
 * \brief Perform a DMA memcpy
 *
 * \param[out] dst      Destination address
 * \param[in] src       Source address
 * \param[in] length    Length in bytes
 */
void gdi_memcpy(void *dst, const void *src, size_t length);

/**
 * \brief Perform a 2D DMA copy
 *
 * Multiple DMAs are initiated automatically based on the provided parameters. Starting from the
 * initial addresses, a DMA of the provided length is performed. Then the addresses are increased by
 * the provided steps and procedure is repeated a total of \r reps times.
 *
 * \param[out] dst      Destination address
 * \param[in] src       Source address
 * \param[in] length    Length in bytes of each iteration
 * \param[in] dst_step  Increase step of the destination address at each iteration
 * \param[in] src_step  Increase step of the source address at each iteration
 * \param[in] reps      Number of iteration to run
 */
void gdi_memcpy_2d(void *dst, const void *src, size_t length, int dst_step, int src_step, size_t reps);

/**
 * \brief Copies the contents of the \ref src buffer to the \ref dst buffer.
 *
 * \param[in] dst       Destination buffer
 * \param[in] src       Source buffer
 */
void gdi_buffer_memcpy(HW_LCDC_LAYER dst_layer, uint8_t dst, HW_LCDC_LAYER src_layer, uint8_t src);

/**
 * \brief Set output color mode
 *
 * Routine to update the output color mode and call callback function (if registered) to
 * configure the target display module.
 *
 * \param[in]  mode   Output color mode
 */
bool gdi_set_color_mode(HW_LCDC_OUTPUT_COLOR_MODE mode);

/**
 * \brief Get GDI configuration structure
 *
 * \return Pointer to GDI structure
 */
gdi_t * gdi_get_gdi_structure(void);

/**
 * \brief Setup a controller's layer
 *
 * \param[in]  layer_no  Layer to which settings should be applied to.
 * \param[in]  address   Layer base address
 * \param[in]  resx      Layer resolution is x coordinate
 * \param[in]  resy      Layer resolution in y coordinate
 * \param[in]  format    Layer input color mode
 */
void gdi_set_layer_src(HW_LCDC_LAYER layer_no, void *address, gdi_coord_t resx, gdi_coord_t resy, gdi_color_fmt_t format);

/**
 * \brief Set layer starting offset
 *
 * \param[in]  layer_no  Layer to which settings should be applied to
 * \param[in]  startx    Offset in x coordinate
 * \param[in]  starty    Offset in y coordinate
 */
void gdi_set_layer_start(HW_LCDC_LAYER layer_no, int startx, int starty);

/**
 * \brief Set layer blending mode
 *
 * \param[in]  layer_no   Layer to which settings should be applied to
 * \param[in]  blendmode  A valid blend mode
 * \param[in]  alpha      Layer global alpha value
 */
void gdi_set_layer_blending(HW_LCDC_LAYER layer_no, HW_LCDC_BLEND_MODE blendmode, uint8_t alpha);

/**
 * \brief Set layer 'dirty' status
 *
 * \param[in]  layer_no   Layer to which settings should be applied to
 * \param[in]  dirty      1 to mark layer dirty, 0 otherwise
 */
void gdi_set_layer_dirty(HW_LCDC_LAYER layer_no, bool dirty);

#if dg_configLCDC_TEST_APP
/**
 * \brief Set controller's background color
 *
 * \param[in]  bg_color   Formatted background color in form of A[7-0]B[7-0]G[7-0]R[7-0]
 *
 * \sa GDI_BG_COLOR_FORMAT
 */
void gdi_set_bg_color(uint32_t bg_color);
#endif /* dg_configLCDC_TEST_APP  */

/**
 * \brief Set layer 'enable' status
 *
 * \param[in]  layer_no   Layer to which settings should be applied to
 * \param[in]  enabled    1 to mark layer enabled, 0 otherwise
 */
void gdi_set_layer_enable(HW_LCDC_LAYER layer_no, bool enable);

/**
 * \brief Get layer 'enable' status
 *
 * \param[in]  layer_no   Layer from which status should be got
 *
 * \return Status of the given layer, 1 if layer is enabled, 0 otherwise
 */
bool gdi_get_layer_enable(HW_LCDC_LAYER layer_no);

/**
 * \brief Setup layer LUT
 *
 * \param[in]  palette_lut  Pointer to LUT that holds 256 color entries for gamma correction
 *
 * \return Previous color LUT address
 */
uint32_t * gdi_set_palette_lut(uint32_t *palette_lut);

/**
 * \brief Get palette entries
 *
 * \param[in]  index       Start index to read from
 * \param[in]  color       Array to store the palette entries
 * \param[in]  color_num   Number of palette entries to read
 *
 * \return Number of entries read
 */
int gdi_get_palette_lut(int index, uint32_t *color, int color_num);

/**
 * \brief Indicates that a specific render operation has started (used for performance measurements)
 *
 * \param[in] tag               Tag of the render operation
 */
void gdi_perf_render_op_start(uint8_t tag);

/**
 * \brief Indicates that a specific render operation has ended (used for performance measurements)
 */
void gdi_perf_render_op_end(void);

/**
 * brief Provides render operation duration measured by the caller (used for performance measurements)
 * for each GPU command
 *
 * \param[in] time_us   Measured time in micro seconds
 * \param[in] tag       Tag of the render operation
 */
void gdi_perf_render_op_time(int time_us, uint8_t tag);

/**
 * \brief Indicates that render operation for current screen has started (used for performance measurements)
 */
void gdi_perf_render_start(void);

/**
 * \brief Indicates that render operation for current screen has ended (used for performance measurements)
 */
void gdi_perf_render_end(void);

/**
 * brief Provides render operation duration measured by the caller (used for performance measurements)
 *
 * \param[in] time_us   Measured time in micro seconds
 */
void gdi_perf_render_time(int time_us);

/**
 * \brief Indicates that transfer to LCD for current screen has started (used for performance measurements)
 */
void gdi_perf_transfer_start(void);

/**
 * \brief Indicates that transfer to LCD for current screen has ended (used for performance measurements)
 */
void gdi_perf_transfer_end(void);

/**
 * brief Provides transfer to LCD duration measured by the caller (used for performance measurements)
 *
 * \param[in] time_us   Measured time in micro seconds
 */
void gdi_perf_transfer_time(int time_us);

/**
 * brief Provides the information to LCD if it is the last area of the refreshing process. (used for performance measurements)
 *
 * \param[in] last   True: it's the last area to flush; False: there are other areas too which will be refreshed soon
 */
void gdi_perf_transfer_last(bool last);

#endif /* dg_configLCDC_ADAPTER */

#endif /* GDI_H_ */

/**
 * \}
 * \}
 */
