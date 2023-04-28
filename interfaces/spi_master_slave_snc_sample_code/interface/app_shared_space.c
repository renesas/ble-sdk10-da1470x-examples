
/**
 *****************************************************************************************
 *
 * @file snc_shared_space.c
 *
 * @brief SNC-SYSCPU shared space environment definition.
 *
 * Copyright (C) 2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include <string.h>
#include <stdbool.h>
#include "sdk_defs.h"
#include "snc.h"
#include "app_shared_space.h"
#include "mailbox.h"

typedef struct {
        volatile uint32_t snc_sf;
        volatile uint32_t syscpu_sf;
} app_semph_t;

typedef struct {
        volatile uint32_t shared_space_ready;   /* Shared space is ready */
        volatile app_semph_t semph;             /* Shared space access semaphore */
} app_shared_info_t;

/*
 * Application shared space info
 */
#if (MAIN_PROCESSOR_BUILD)
__RETAINED app_shared_info_t *app_shared_info_ptr;
__RETAINED app_shared_data_t *app_shared_data_ptr;
#elif (SNC_PROCESSOR_BUILD)
__SNC_SHARED app_shared_info_t app_shared_info;
__SNC_SHARED app_shared_data_t app_shared_data;

#define app_shared_info_ptr     ( &app_shared_info )
#define app_shared_data_ptr     ( &app_shared_data )
#endif /* SNC_PROCESSOR_BUILD */

/*
 * Macros indicating the semaphore signal flag used for each master
 */
#if (MAIN_PROCESSOR_BUILD)
#define THIS_MASTER_SF          syscpu_sf
#define OTHER_MASTER_SF         snc_sf
#elif (SNC_PROCESSOR_BUILD)
#define THIS_MASTER_SF          snc_sf
#define OTHER_MASTER_SF         syscpu_sf
#endif /* SNC_PROCESSOR_BUILD */

/*
 * SHARED SPACE ENVIRONMENT FUNCTIONS
 **************************************
 */

void app_shared_space_ctrl_init(void)
{
#if (SNC_PROCESSOR_BUILD)
        /* Initialize the shared space that hosts control info. */
        OPT_MEMSET(app_shared_info_ptr, 0, sizeof(app_shared_info_t));
        /*
         * Publish the control data shared space to the SNC service by using the corresponding
         * application-defined index.
         */
        snc_set_shared_space_addr(app_shared_info_ptr, SNC_SHARED_SPACE_APP(APP_SHARED_SPACE_CTRL));
#endif
}

void app_shared_space_data_init(void)
{
#if (SNC_PROCESSOR_BUILD)
        /* Initialize the shared space that hosts raw data. */
        OPT_MEMSET(app_shared_data_ptr, 0 , sizeof(app_shared_data_t));
        /*
         * Publish the raw data shared space to the SNC service by using the corresponding
         * application-defined index.
         */
        snc_set_shared_space_addr(app_shared_data_ptr, SNC_SHARED_SPACE_APP(APP_SHARED_SPACE_DATA));
#endif
}

void app_shared_space_ctrl_set_ready(void)
{
#if (SNC_PROCESSOR_BUILD)
        app_shared_info_ptr->shared_space_ready = true;
#endif
}

app_shared_data_t *app_shared_space_data_get(void)
{
#if (MAIN_PROCESSOR_BUILD)
        ASSERT_WARNING(app_shared_data_ptr);
#endif
        return app_shared_data_ptr;
}

void app_shared_data_notify_syscpu(void)
{
#if (SNC_PROCESSOR_BUILD)
# if dg_configUSE_MAILBOX
        /* Set the bit index associated to consuming the raw data */
        mailbox_set_int(MAILBOX_ID_MAIN_PROCESSOR, MAILBOX_INT_MAIN_APP);
# endif /* dg_configUSE_MAILBOX */
        /* Time to assert SNC2SYS IRQ line. */
        snc_set_snc2sys_int();
#endif /* SNC_PROCESSOR_BUILD */
}

bool app_shared_space_ctrl_is_ready(void)
{
        bool ret = false;

#if (MAIN_PROCESSOR_BUILD)
        if (app_shared_info_ptr == NULL) {
                app_shared_info_t *ptr;

                ptr = snc_get_shared_space_addr(SNC_SHARED_SPACE_APP(APP_SHARED_SPACE_CTRL));

                if (ptr != NULL) {
                        app_shared_info_ptr = ptr;
                }
        }

        if (app_shared_info_ptr) {
                ret =  app_shared_info_ptr->shared_space_ready ? true : false;
        } else {
                ret = false;
        }
#elif (SNC_PROCESSOR_BUILD)
        ret =  app_shared_info_ptr->shared_space_ready ? true : false;
#endif /* SNC_PROCESSOR_BUILD */

        return ret;
}

bool app_shared_space_data_is_ready(void)
{
        bool ret = false;
#if (MAIN_PROCESSOR_BUILD)
        if (app_shared_data_ptr == NULL) {
                app_shared_data_t *ptr;

                ptr = snc_get_shared_space_addr(SNC_SHARED_SPACE_APP(APP_SHARED_SPACE_DATA));

                if (ptr != NULL) {
                        app_shared_data_ptr = ptr;
                }
        }

        if (app_shared_data_ptr) {
                ret =  true;
        } else {
                ret = false;
        }
#endif
        return ret;
}

void app_semph_take(void)
{
#if (MAIN_PROCESSOR_BUILD)
        ASSERT_WARNING(app_shared_info_ptr);
#endif
        do {
                app_shared_info_ptr->semph.THIS_MASTER_SF = 1;

                if (app_shared_info_ptr->semph.OTHER_MASTER_SF == 0) {
                        break;
                }

                app_shared_info_ptr->semph.THIS_MASTER_SF = 0;
                while (app_shared_info_ptr->semph.OTHER_MASTER_SF);
        } while (1);
}

void app_semph_give(void)
{
#if (MAIN_PROCESSOR_BUILD)
        ASSERT_WARNING(app_shared_info_ptr);
#endif
        app_shared_info_ptr->semph.THIS_MASTER_SF = 0;
}
