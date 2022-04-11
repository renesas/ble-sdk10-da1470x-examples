/**
 ****************************************************************************************
 *
 * @file platform_devices.h
 *
 * @brief Configuration of devices connected to board
 *
 * Copyright (C) 2021-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#ifndef PLATFORM_DEVICES_H_
#define PLATFORM_DEVICES_H_

#include "ad_i2c.h"
#include "ad_spi.h"
#include "ad_lcdc.h"
#include "peripheral_setup.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (dg_configGPADC_ADAPTER == 1)

/*
 * Define sources connected to GPADC
 */

#endif /* dg_configGPADC_ADAPTER */

#if dg_configLCDC_ADAPTER
#if dg_configUSE_H13TB18A
extern const ad_lcdc_io_conf_t h13tb18a_io;
#endif
#if dg_configUSE_TFT12864_57_E
extern const ad_lcdc_io_conf_t tft12864_57_e_io;
#endif
#if dg_configUSE_LPM010M297B
extern const ad_lcdc_io_conf_t lpm010m297b_io;
#endif
#if dg_configUSE_HM80160A090
extern const ad_lcdc_io_conf_t hm80160a090_io;
#endif
#if dg_configUSE_E1394AA65A
extern const ad_lcdc_io_conf_t e1394aa65a_io;
#endif
#if dg_configUSE_E1394AA65A_DSPI
extern const ad_lcdc_io_conf_t e1394aa65a_dspi_io;
#endif
#if dg_configUSE_LPM012M134B
extern const ad_lcdc_io_conf_t lpm012m134b_io;
#endif
#if dg_configUSE_NHD43480272EFASXN
extern const ad_lcdc_io_conf_t nhd43480272efasxn_io;
#if dg_configUSE_FT5306
extern const ad_i2c_io_conf_t ft5306_io;
#endif
#endif
#if dg_configUSE_LS013B7DH06
extern const ad_lcdc_io_conf_t ls013b7dh06_io;
#endif
#if dg_configUSE_LS013B7DH03
extern const ad_lcdc_io_conf_t ls013b7dh03_io;
#endif
#if dg_configUSE_DT280QV10CT
extern const ad_lcdc_io_conf_t dt280qv10ct_io;
#endif
#if dg_configUSE_T1D3BP006 || dg_configUSE_T1D3BP006_DSPI
extern const ad_lcdc_io_conf_t t1d3bp006_io;
#endif
#if dg_configUSE_T1D54BP002
extern const ad_lcdc_io_conf_t t1d54bp002_io;
#endif
#if dg_configUSE_PSP27801
extern const ad_lcdc_io_conf_t psp27801_io;
#endif
#if dg_configUSE_MCT024L6W240320PML
extern const ad_lcdc_io_conf_t mct024l6w240320pml_io;
#endif
#if dg_configUSE_LPM013M091A
extern const ad_lcdc_io_conf_t lpm013m091a_io;
#endif
#if dg_configUSE_ILI9341
extern const ad_lcdc_io_conf_t ili9341_io;
#endif
#if dg_configUSE_XSJ120TY2401_QSPI || dg_configUSE_XSJ120TY2401_DSPI || dg_configUSE_XSJ120TY2401_SPI3 || dg_configUSE_XSJ120TY2401_SPI4
extern const ad_lcdc_io_conf_t xsj120ty2401_spi_io;
#endif
#if dg_configUSE_XSJ120TY2401_DBIB
extern const ad_lcdc_io_conf_t xsj120ty2401_dbib_io;
#endif
#if dg_configUSE_ONOC40108_SPI4
extern const ad_lcdc_io_conf_t onoc40108_spi4_io;
#endif
#if dg_configUSE_MRB3973_DBIB
extern const ad_lcdc_io_conf_t mrb3973_dbib_io;
#endif
#if dg_configUSE_BOE139F454SM
extern const ad_lcdc_io_conf_t boe139f454sm_io;
#endif
#if dg_configUSE_E120A390QSR
extern const ad_lcdc_io_conf_t e120a390qsr_io;
#endif
#endif /* dg_configLCDC_ADAPTER */

#if dg_configUSE_FT6206
extern const ad_i2c_io_conf_t ft6206_io;
#endif
#if dg_configUSE_ZT2628
extern const ad_i2c_io_conf_t zt2628_io;
#endif

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_DEVICES_H_ */
