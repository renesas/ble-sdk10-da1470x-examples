/**
 *****************************************************************************************
 *
 * @file snc_shared_space.h
 *
 * @brief SNC-SYSCPU shared space environment.
 *
 * Copyright (C) 2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef SNC_SHARED_SPACE_H_
#define SNC_SHARED_SPACE_H_

#include <stdint.h>
#include <stdbool.h>
#include "app_common.h"

typedef struct {
        uint32_t timestamp;
        bool valid;
        size_t data_size;
        uint8_t data[];
} chunk_t;

/**
 * \brief Application shared data type
 */
typedef struct {
         size_t chunk_size;
         size_t chunk_total_size;
         size_t num_of_chunks;
         uint8_t *chunk_data;
         chunk_t *chunk_r;
         chunk_t *chunk_w;
} app_shared_data_t;

/*
 * The shared space that hosts the raw data is split into chunks each capable of
 * holding a predefined number of bytes \p APP_DATA_CHUNK_MAX_SIZE.
 */
#ifndef APP_DATA_NUM_OF_CHUNKS
#define APP_DATA_NUM_OF_CHUNKS                  4
#endif

/* Max. number of bytes a chunk of the shared space can accommodate. */
#ifndef APP_DATA_CHUNK_MAX_SIZE
#define APP_DATA_CHUNK_MAX_SIZE                 10
#endif

/**
 * \brief Application shared space handle IDs
 *
 * Use snc_set_shared_space_addr() (in SNC) to publish the address of application shared data.
 * Use snc_get_shared_space_addr() to acquire the address of application shared data.
 */
typedef enum {
        APP_SHARED_SPACE_CTRL,          /**< Handle id for control data in shared space */
        APP_SHARED_SPACE_DATA           /**< Handle id for application data in shared space */
} APP_SHARED_SPACE_TYPE;

/*
 * SHARED SPACE CONTROL FUNCTIONS
 **************************************
 */

/**
 * \brief Initialize the shared space that accommodates the control data
 *
 * \note It can be called only in SNC context. SNC defines the shared space environment.
 */
void app_shared_space_ctrl_init(void);

/**
 * \brief Initialize the shared space that accommodates the raw data
 *
 * \note It can be called only in SNC context. SNC defines the shared space environment.
 */
void app_shared_space_data_init(void);

/**
 * \brief Get the number of allocated chunks
 *
 * \return The number of chunks that contain valid data and should be consumed.
 *
 * \note It can be called by both SNC and SYSPCU contexts.
 */
uint32_t app_shared_space_data_get_alloc_chunks(void);

/**
 * \brief Get the number of bytes contained in the current chunk
 *
 * This API should be called by the SYSCPU so the appropriate space can be allocated
 * before popping data from the shared space.
 *
 * {code}
 *
 * while (app_shared_space_data_get_alloc_chunks()) {
 *      uint8_t *buf = NULL;
 *      size_t data_read;
 *      uint32_t timestamp;
 *
 *      size_t chunk_byte_size = app_shared_space_data_get_cur_chunk_bytes();
 *
 *      if (chunk_byte_size) {
 *              buf = OS_MALLOC(chunk_byte_size);
 *      }
 *      if (!buf) {
 *              continue;
 *      }
 *
 *      if (app_shared_space_data_queue_pop(buf, &data_read, &timestamp)) {
 *                     // Process the data retrieved from the current chunk
 *      }
 *
 *     OS_FREE(buf);
 * }
 *
 * {code}
 *
 * \return The number of bytes available in the chunk pointed by the read pointer
 *         of the circular buffer.
 *
 * \note It can be called by both SNC and SYSPCU contexts.
 */
size_t app_shared_space_data_get_cur_chunk_bytes(void);

/**
 * \brief Helper function to pop data from the shared space that accommodates the raw data
 *
 * \param[in]   data      Pointer to buffer in which data will be copied to.
 * \param[out]  size      Number of bytes copied to the \p data buffer.
 * \param[out]  timestamp Timestamp associated to when data in the chunk was pushed into.
 *
 * \return True is the pop operation succeeded; False is there are no available data in
 *         the shared space.
 *
 * \note It should be called only in SYSCPU context.
 */
bool app_shared_space_data_queue_pop(uint8_t *data, size_t *size, uint32_t *timestamp);

/**
 * \brief Helper function to push data into the shared space that accommodates the raw data
 *
 * \param[in]  data  Pointer to data that should be pushed into the shared space.
 * \param[in]  size  Number of bytes that should be copied from \p the data buffer.
 *
 * \return True is the push operation succeeded; False is the shared space is fully occupied
 *         and no free chunks are available.
 *
 * \note It should be called only in SNC context.
 */
bool app_shared_space_data_queue_push(uint8_t *data, size_t size);

/**
 * \brief Mark the shared space environment as ready
 *
 * \note It can be called only in SNC context. SNC should call this API when the whole shared
 *       environment is setup (control and data space).
 */
void app_shared_space_ctrl_set_ready(void);

/**
 * \brief Check whether control data environment is ready
 */
bool app_shared_space_ctrl_is_ready(void);

/**
 * \brief Check whether raw data environment is ready
 */
bool app_shared_space_data_is_ready(void);

/**
 * \brief Helper function to notify the SYSCPU master when plain SNC-SYSCPU IRQ interrupts
 *        or the mailbox service is demonstrated.
 *
 * \note It can be called only in SNC context. The SNC should invoke this API when raw data
 *       are pushed into the shared space so the remote master can further process them.
 */
void app_shared_data_notify_syscpu(void);

/**
 * \brief Acquire semaphore for this master.
 *
 * This is a blocking routine meaning that code execution will block until
 * the semaphore is acquired. DG Coroutines can be blocked only in a task's
 * scope and so no blocking operations (e.g OS_DELAY_MS()) are invoked;
 * a polling approach is followed instead.
 *
 * \note Task execution might be significantly delayed is the semaphore is not handled
 *       carefully. This mighty also trigger the WDOG counter to expire.
 *
 *  A non-blocking implementation could be as follows:
 *
 *        {code}
 *
 *        bool app_semph_take(void)
 *        {
 *             do {
 *                       app_shared_info_ptr->semph.THIS_MASTER_SF = 1;
 *
 *                       if (app_shared_info_ptr->semph.OTHER_MASTER_SF == 0) {
 *                               return true;
 *                       }
 *
 *                       app_shared_info_ptr->semph.THIS_MASTER_SF = 0;
 *                       return false;
 *               } while (0);
 *           }
 *
 *           // API Usage
 *
 *           while (!app_seph_take()) {
 *              // In a co-operative task blocking events are allowed only within tasks' scope.
 *              OS_DELAY_MS(10);
 *           }
 *        {code}
 */
void app_semph_take(void);

/**
 * \brief Release semaphore for this master
 */
void app_semph_give(void);

#endif /* SNC_SHARED_SPACE_H_ */
