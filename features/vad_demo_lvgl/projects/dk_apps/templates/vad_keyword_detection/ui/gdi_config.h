/**
 ****************************************************************************************
 *
 * @file gdi_config.h
 *
 * @brief Configures GDI parameters
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
#ifndef GDI_CONFIG_H_
#define GDI_CONFIG_H_

#include "demo.h"

#if dg_configUSE_LPM012M134B
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
