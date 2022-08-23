
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
#include "osal.h"

typedef struct {
        volatile uint32_t snc_sf;
        volatile uint32_t syscpu_sf;
} app_semph_t;

typedef struct {
        volatile uint32_t shared_space_ready;   /* Shared space is ready */
        volatile app_semph_t semph;             /* Shared space access semaphore */
} app_shared_info_t;

#define _DATA_BUFFER_WORD_ALIGNMENT(_buffer)  ((_buffer + 3) / 4 * 4)

/*
 * Application shared space info
 */
#if (MAIN_PROCESSOR_BUILD)
__RETAINED app_shared_info_t *app_shared_info_ptr;
__RETAINED app_shared_data_t *app_shared_data_ptr;
#elif (SNC_PROCESSOR_BUILD)
__SNC_SHARED app_shared_info_t app_shared_info;
__SNC_SHARED app_shared_data_t app_shared_data;
__SNC_SHARED static uint8_t shared_buffer[((sizeof(chunk_t) +
                _DATA_BUFFER_WORD_ALIGNMENT(APP_DATA_CHUNK_MAX_SIZE)) * APP_DATA_NUM_OF_CHUNKS)];
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
        app_shared_data_ptr->num_of_chunks = APP_DATA_NUM_OF_CHUNKS;
        /*
         * Make sure data headers are 32-bit aligned; otherwise system exception will be triggered
         * because of data access misalignment.
         */
        app_shared_data_ptr->chunk_size = _DATA_BUFFER_WORD_ALIGNMENT(APP_DATA_CHUNK_MAX_SIZE);
        app_shared_data_ptr->chunk_total_size = (sizeof(chunk_t) + app_shared_data_ptr->chunk_size);

        app_shared_data_ptr->chunk_data = shared_buffer;

        /* Mark all chunks of the data shared space as 'not in use' */
        for (int i = 0; i < app_shared_data_ptr->num_of_chunks; i++) {
                chunk_t *cur_chunk =
                        (chunk_t *)&app_shared_data_ptr->chunk_data[i * app_shared_data_ptr->chunk_total_size];
                cur_chunk->valid = false;
        }

        /* Initialize the circular buffer pointers to point to the base address */
        app_shared_data_ptr->chunk_w = (chunk_t *)app_shared_data_ptr->chunk_data;
        app_shared_data_ptr->chunk_r = (chunk_t *)app_shared_data_ptr->chunk_data;

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

uint32_t app_shared_space_data_get_alloc_chunks(void)
{
#if (MAIN_PROCESSOR_BUILD)
        ASSERT_WARNING(app_shared_data_ptr);
#endif
        uint32_t cnt = 0;

        /* Traverse all chunks of the shared space */
        for (int i = 0; i < app_shared_data_ptr->num_of_chunks; i++) {
                chunk_t *cur_chunk =
                        (chunk_t *)&app_shared_data_ptr->chunk_data[i * app_shared_data_ptr->chunk_total_size];
#if (MAIN_PROCESSOR_BUILD)
                /* All pointers should be translated to SYSCPU address space */
                cur_chunk = snc_convert_snc2sys_addr((const void *)cur_chunk);
#endif
                /* Check if chunk contains valid data and increase the counter */
                if (cur_chunk->valid) {
                        cnt++;
                }
        }

        return cnt;
}

size_t app_shared_space_data_get_cur_chunk_bytes(void)
{
        size_t ret = 0;
        chunk_t *chunk_r = app_shared_data_ptr->chunk_r;

#if (MAIN_PROCESSOR_BUILD)
        /* Pointers should be translated to the SYSCPU address space */
        chunk_r = (chunk_t *)snc_convert_snc2sys_addr((const void *)chunk_r);
#endif
        if (chunk_r->valid) {
                ret = chunk_r->data_size;
        }

        return ret;
}

bool app_shared_space_data_queue_pop(uint8_t *data, size_t *size, uint32_t *timestamp)
{
        bool ret = false;

#if (MAIN_PROCESSOR_BUILD)
        /* Pointers should be translated to the SYSCPU address space */
        chunk_t *chunk_r =
                 (chunk_t *)snc_convert_snc2sys_addr((const void *)app_shared_data_ptr->chunk_r);

        /*
         * Read pointer should have been advanced since the last call; check if it points
         * to an empty/free chunk. Otherwise do not continue.
         */
        if (chunk_r->valid) {
                if (data) {
                        /*
                         * [chunk_r->data] is not a real pointer so it will point to the SYSCPU
                         * address space; no need to translate it!!!
                         */
                        OPT_MEMCPY(data, (void *)chunk_r->data , chunk_r->data_size);
                }
                if (size) {
                        *size = chunk_r->data_size;
                }
                if (timestamp) {
                        *timestamp = chunk_r->timestamp;
                }
                /*
                 * Data has been fetched; invalidate/free the chunk so it can be used for further
                 * push operations.
                 */
                chunk_r->valid = false;

                /* Advance the read pointer of the circular buffer  */
                chunk_r = (chunk_t *)(((uint8_t *)chunk_r) + app_shared_data_ptr->chunk_total_size);
                chunk_t *buffer_end =
                        (chunk_t *)snc_convert_snc2sys_addr((const void *)&app_shared_data_ptr->chunk_data[app_shared_data_ptr->num_of_chunks * app_shared_data_ptr->chunk_total_size]);

                /* Check if we have reached the end of the circular buffer and if so, wrap around to the base buffer address. */
                if (chunk_r >= buffer_end) {
                        /* Store the base address in SNC address space */
                        chunk_r =
                                (chunk_t *)snc_convert_snc2sys_addr((const void *)app_shared_data_ptr->chunk_data);
                }

                /*
                 * All addresses stored in the shared memory should be normalized; should be translated
                 * to the SNC address space.
                 */
                app_shared_data_ptr->chunk_r = (chunk_t *)snc_convert_sys2snc_addr((const void *)chunk_r);

                ret = true;
        }
#endif

        return ret;
}

bool app_shared_space_data_queue_push(uint8_t *data, size_t size)
{
        bool ret = false;

#if (SNC_PROCESSOR_BUILD)
        do {
                /* Get the next chunk pointed by the write pointer of the circular buffer.
                 *
                 * \note The write pointer should have been advanced since the last successful
                 *       push operation.
                 */
                chunk_t *chunk_w = app_shared_data_ptr->chunk_w;

                /*
                 * Check if the chunk is indeed free to store new data; if not abort the push
                 * operation as we have run out of free chunks.
                 */
                if (chunk_w->valid) {
                        break;
                }

                chunk_w->data_size = size;

                /* If zero, abort the push operation */
                if (chunk_w->data_size == 0) {
                        break;
                }
                /*
                 * Check if size requested exceeds chunk's capacity and if so, make sure we
                 * do not exceed the max. capacity.
                 */
                if (chunk_w->data_size > app_shared_data_ptr->chunk_size) {
                        chunk_w->data_size = app_shared_data_ptr->chunk_size;
                }
                /* Get a timestamp value */
                chunk_w->timestamp = (uint32_t)OS_TICKS_2_MS(OS_GET_TICK_COUNT());

                OPT_MEMCPY((void *)chunk_w->data, data, chunk_w->data_size);

                /*
                 * Chunk's header and raw data have been filled; it's time to mark it as valid
                 * so, it can be consumed.
                 */
                chunk_w->valid = true;

                /* Advance write pointer's value */
                chunk_w = (chunk_t *)(((uint8_t *)chunk_w) + app_shared_data_ptr->chunk_total_size);

                chunk_t *buf_end = (chunk_t *)&app_shared_data_ptr->chunk_data[app_shared_data_ptr->num_of_chunks * app_shared_data_ptr->chunk_total_size];
                /* Check if we have reached the end of the circular buffer and if so, wrap around to the base buffer address. */
                if (chunk_w >= buf_end) {
                        chunk_w = (chunk_t *)app_shared_data_ptr->chunk_data;
                }
                app_shared_data_ptr->chunk_w = chunk_w;

                ret = true;
        } while (0);
#endif

        return ret;
}
