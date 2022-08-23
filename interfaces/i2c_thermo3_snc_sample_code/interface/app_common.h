/**
 ****************************************************************************************
 *
 * @file app_common.h
 *
 * @brief File that includes common definitions
 *
 * Copyright (C) 2020-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef APP_COMMON_H
#define APP_COMMON_H

/*
 * This macro tells the SNC service the number of the application-defined addresses to be published.
 * It is imperative that this macro be commonly seen by both the SNC and SYSCPU projects
 * Otherwise, there will be mismatch in the shared space interpretation between the SNC and SYSCPU
 * context; thus triggering assertions.
 */
#define SNC_SHARED_SPACE_APP_COUNT              ( 2 )

#define APP_RPMSG_LITE_SNC_EPT_ADDR             ( 0x30 )
#define APP_RPMSG_LITE_SYSCPU_EPT_ADDR          ( 0x40 )

/* Required to avoid compiler errors for double macro definitions; should not be changed. */
#undef dg_configUSE_MAILBOX
#undef dg_configUSE_RPMSG_LITE

/*
 * A positive value will trigger message exchanging (SNC-SYSCPU) via the mailbox service.
 * A zero value will demonstrate message exchanging based on simple SNC-SYSCPU interrupt.
 */
#define dg_configUSE_MAILBOX                    ( 1 )

/*
 * A positive value will trigger message exchanging (SNC-SYSCPU) via the RPMSG-Lite framework.
 * If both mailbox and RPMSG-Lite macro definitions are enabled, the RPMSG-Lite demonstration
 * will take precedence and be exhibited.
 */
#define dg_configUSE_RPMSG_LITE                 ( 0 )

/* The RPMSG-Lite service makes use of the mailbox service; should not be changed. */
#if dg_configUSE_RPMSG_LITE
#undef dg_configUSE_MAILBOX
#define dg_configUSE_MAILBOX                    ( 1 )
#endif

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
