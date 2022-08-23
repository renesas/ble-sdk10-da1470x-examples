/**
 ****************************************************************************************
 *
 * @file app_common.h
 *
 * @brief File that includes common application-specific definitions.
 *
 * Copyright (C) 2020-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef APP_COMMON_H
#define APP_COMMON_H

#include "tmp102_reg.h"

/* The high temperature value (in Celsius) that defines the boundaries of the alter signaling. */
#define THERMO3_ALERT_T_HIGH       ( 26 )
/* The low temperature value (in Celsius) that defines the boundaries of the alter signaling. */
#define THERMO3_ALERT_T_LOW        ( 25 )

/* Alert signal polarity */
#define THERMO3_ALEERT_POL         TMP102_ALERT_POLARITY_ACTIVE_HIGH

#ifndef __APP_STATIC
# if (SNC_PROCESSOR_BUILD)
#  define __APP_STATIC static
# elif (MAIN_PROCESSOR_BUILD)
#  define __APP_STATIC
# endif
#endif /* __APP_STATIC */

#ifndef DBG_LOG
# ifdef CONFIG_RETARGET
#  define DBG_LOG(args...)    printf(args)
# else
#  define DBG_LOG(args...)
# endif
#endif

#endif /* APP_COMMON_H */
