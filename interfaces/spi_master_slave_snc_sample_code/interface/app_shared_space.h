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

/**
 * \brief Application shared data type
 */
typedef struct {
        volatile uint8_t *rx_data_ptr;
        volatile size_t rx_data_size;
} app_shared_data_t;

/**
 * \brief Application shared space handle IDs
 *
 * Use snc_set_shared_space_addr() to publish the address of application shared data.
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

/*
 * \brief Get the address of the shared space that accommodates the raw data
 *
 * The \p app_shared_data_t structure contains a pointer to the shared space
 * along with a counter indicating the number of valid bytes available in
 * the memory. It is user's responsibility to allocate a shared space and then
 * initialize the pointer.
 *
 * \code{.c}
 *
 * // SNC allocates some shared space to host the raw data
 * __SNC_SHARED uint8_t app_rx_data[...];
 *
 * OS_TASK_FUNCTION(...)
 * {
 *
 *      app_shared_data_t *app_shared_data_ptr = app_shared_space_data_get();
 *      app_shared_data_ptr->rx_data_ptr = app_rx_data;
 *
 *      for (;;) {
 *      }
 * }
 *
 * \endcode
 *
 * \return Pointer to the shared data structure
 */
app_shared_data_t *app_shared_space_data_get(void);

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
 * scope and so, no blocking operations (e.g OS_DELAY_MS()) are invoked;
 *
 * Task execution might be significantly delayed if the semaphore is not handled
 * carefully. This mighty also trigger the WDOG counter to expire.
 *
 * A non-blocking implementation could be as follows:
 *
 * \code{.c}
 *
 * bool app_semph_take(void)
 * {
 *      do {
 *              app_shared_info_ptr->semph.THIS_MASTER_SF = 1;
 *
 *              if (app_shared_info_ptr->semph.OTHER_MASTER_SF == 0) {
 *                      return true;
 *              }
 *
 *              app_shared_info_ptr->semph.THIS_MASTER_SF = 0;
 *              return false;
 *       } while (0);
 * }
 *
 * // API Usage
 *
 * for (;;) {
 *      while (!app_seph_take()) {
 *      // In a co-operative task blocking events are allowed only within tasks' scope.
 *      OS_DELAY_MS(10);
 * }
 *
 * \endcode
 */
void app_semph_take(void);

/**
 * \brief Release semaphore for this master
 */
void app_semph_give(void);

#endif /* SNC_SHARED_SPACE_H_ */
