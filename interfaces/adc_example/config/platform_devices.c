/**
 ****************************************************************************************
 *
 * @file platform_devices.c
 *
 * @brief Platform Devices
 *
 * Copyright (C) 2015-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include "ad_gpadc.h"
#include "platform_devices.h"

/*
 * PLATFORM PERIPHERALS GPIO CONFIGURATION
 *****************************************************************************************
 */

#if dg_configGPADC_ADAPTER || dg_configUSE_HW_GPADC


/* GPADC IO configurations */
const ad_gpadc_io_conf_t io_conf_se = {
        .input0 = {
                .port = HW_GPIO_PORT_0,
                .pin  = HW_GPIO_PIN_5,
                .on   = {HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_ADC, true},
                .off  = {HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_GPIO, false}
        },
        .input1 = {
                .port = HW_GPIO_PORT_NONE,
                .pin  = HW_GPIO_PIN_NONE,
        },
        .voltage_level = HW_GPIO_POWER_VDD1V8P
};

/* GPADC driver configurations */
const ad_gpadc_driver_conf_t drv_conf_se = {
        .input_mode             = HW_GPADC_INPUT_MODE_SINGLE_ENDED,
        .positive               = HW_GPADC_INP_P0_5,
        .temp_sensor            = HW_GPADC_NO_TEMP_SENSOR,
        .sample_time            = 5,
        .continuous             = false,
        .interval               = 0,
        .input_attenuator       = HW_GPADC_INPUT_VOLTAGE_UP_TO_1V8,
        .chopping               = true,
        .oversampling           = HW_GPADC_OVERSAMPLING_4_SAMPLES,
        .result_mode            = HW_GPADC_RESULT_NORMAL,
#if HW_GPADC_DMA_SUPPORT
        .dma_setup              = NULL
#endif
};

/*  External device/module configurations */
const ad_gpadc_controller_conf_t conf_se = {
        .id  = HW_GPADC_1,
        .io  = &io_conf_se,
        .drv = &drv_conf_se
};

gpadc_device POT_DEVICE = &conf_se;

#endif /* dg_configGPADC_ADAPTER || dg_configUSE_HW_GPADC */
