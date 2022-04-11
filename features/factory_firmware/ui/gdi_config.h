/**
 * \addtogroup UI
 * \{
 * \addtogroup GDI
 *
 * \brief GDI configuration
 * \{
 */
/**
 ****************************************************************************************
 *
 * @file gdi_config.h
 *
 * @brief Configures GDI parameters
 *
 * Copyright (C) 2020-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef GDI_CONFIG_H_
#define GDI_CONFIG_H_

#include "demo.h"

#if dg_configUSE_HM80160A090
#include "hm80160a090.h"
#elif dg_configUSE_E1394AA65A
#include "e1394aa65a.h"
#elif dg_configUSE_E1394AA65A_DSPI
#include "e1394aa65a_dspi.h"
#elif dg_configUSE_LPM012M134B
#include "lpm012m134b.h"
#elif dg_configUSE_NHD43480272EFASXN
#include "nhd43480272efasxn.h"
#elif dg_configUSE_LS013B7DH06
#include "ls013b7dh06.h"
#elif dg_configUSE_LS013B7DH03
#include "ls013b7dh03.h"
#elif dg_configUSE_DT280QV10CT
#include "dt280qv10ct.h"
#elif dg_configUSE_T1D3BP006
#include "t1d3bp006.h"
#elif dg_configUSE_T1D54BP002
#include "t1d54bp002.h"
#elif dg_configUSE_PSP27801
#include "psp27801.h"
#elif dg_configUSE_MCT024L6W240320PML
#include "mct024l6w240320pml.h"
#elif dg_configUSE_LPM013M091A
#include "lpm013m091a.h"
#elif dg_configUSE_ILI9341
#include "ili9341.h"
#elif dg_configUSE_RM69310
#include "rm69310.h"
#elif dg_configUSE_XSJ120TY2401_QSPI
#include "xsj120ty2401_qspi.h"
#elif dg_configUSE_XSJ120TY2401_SPI3
#include "xsj120ty2401_spi3.h"
#elif dg_configUSE_XSJ120TY2401_SPI4
#include "xsj120ty2401_spi4.h"
#elif dg_configUSE_XSJ120TY2401_DSPI
#include "xsj120ty2401_dspi.h"
#elif dg_configUSE_XSJ120TY2401_DBIB
#include "xsj120ty2401_dbib.h"
#elif dg_configUSE_MRB3973_DBIB
#include "mrb3973_dbib.h"
#elif dg_configUSE_BOE139F454SM
#include "boe139f454sm.h"
#elif dg_configUSE_E120A390QSR
#include "e120a390qsr.h"
#endif
#if dg_configUSE_FT6206
#include "ft6206.h"
#elif dg_configUSE_ZT2628
#include "zt2628.h"
#endif
#if dg_configUSE_TOUCH_SIMULATION
#include "touch_simulation.h"
#endif

#define GDI_SINGLE_FB_NUM       (2)
#define GDI_FB_COLOR_FORMAT     (FB_COLOR_FORMAT)
#define GDI_FB_RESX             (DEMO_RESX)
#define GDI_FB_RESY             (DEMO_RESY)

/*
 * \note Printing log messages will adversely affect system's performance
 */
#ifdef PERFORMANCE_METRICS
#define GDI_CONSOLE_LOG         (1)
#else
#define GDI_CONSOLE_LOG         (0)
#endif

#endif /* GDI_CONFIG_H_ */

/**
 * \}
 * \}
 */
