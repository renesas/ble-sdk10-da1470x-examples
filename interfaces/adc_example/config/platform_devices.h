/**
 ****************************************************************************************
 *
 * @file platform_devices.h
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


#ifdef __cplusplus
extern "C" {
#endif


#if dg_configGPADC_ADAPTER || dg_configUSE_HW_GPADC

/*
 * \brief GPADC device handle
 */
typedef const ad_gpadc_controller_conf_t* gpadc_device;

#endif /* dg_configGPADC_ADAPTER || dg_configUSE_HW_GPADC */


/* List of devices */
extern gpadc_device POT_DEVICE;


#ifdef __cplusplus
}
#endif
