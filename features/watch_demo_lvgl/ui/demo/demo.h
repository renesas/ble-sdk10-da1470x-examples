/**
 ****************************************************************************************
 *
 * @file demo.h
 *
 * @brief Demo header file
 *
 * Copyright (C) 2021-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#ifndef __DEMO_H
#define __DEMO_H

#define FB_COLOR_FORMAT                 (CF_NATIVE_RGB565)
#define GDI_FB_USE_QSPI_RAM             (0)

#define ORIGINAL                        (1)
#define OPTIMAL                         (2)
#define NO_GPU                          (3)
#define SCENARIO                        (OPTIMAL)

#define COMPASS_ROTATION_USES_CANVAS    (0)

#define TWO_LAYERS_HORIZONTAL_SLIDING   (1)

#if !COMPASS_ROTATION_USES_CANVAS
#define DEMO_GUI_HEAP_SIZE              (15 * 1024)
#else
#define DEMO_GUI_HEAP_SIZE              (320 * 1024)
#endif

#ifndef PERFORMANCE_METRICS
#define LV_PORT_INDEV_TOUCH_QUEUE_EN    (0)
#endif

#endif /*__DEMO_H */
