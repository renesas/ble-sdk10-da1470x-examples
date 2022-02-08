/**
 ****************************************************************************************
 *
 * @file my_custom_service.h
 *
 * @brief BLE custom service
 *
 * Copyright (C) 2015-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef MY_CUSTOM_SERVICE_H_
#define MY_CUSTOM_SERVICE_H_

#include <stdint.h>
#include <ble_service.h>


/* User-defined callback functions - Prototyping */
typedef void (* mcs_get_char_value_cb_t) (ble_service_t *svc, uint16_t conn_idx);

typedef void (* mcs_set_char_value_cb_t) (ble_service_t *svc, uint16_t conn_idx,
                                                               const uint8_t *value);



/* User-defined callback functions */
typedef struct {

        /* Handler for read requests - Triggered on application context */
        mcs_get_char_value_cb_t get_characteristic_value;

        /* Handler for write requests - Triggered on application context */
        mcs_set_char_value_cb_t set_characteristic_value;

} my_custom_service_cb_t;


/*
 * \brief This function creates the custom BLE service and registers it in BLE framework
 *
 * \param [in] variable_value Default characteristic value
 * \param [in] cb             Application callback functions
 *
 * \return service handle
 *
 */
ble_service_t *mcs_init(const uint8_t *variable_value, const my_custom_service_cb_t *cb);



/*
 * This function should be called by the application as a response to a read request
 *
 * \param[in] svc       service instance
 * \param[in] conn_idx  connection index
 * \param[in] status    ATT error
 * \param[in] value     attribute value
 */
void mcs_get_char_value_cfm(ble_service_t *svc, uint16_t conn_idx, att_error_t status,
                                                                    const uint8_t *value);


/*
 * This function should be called by the application as a response to a write request
 *
 * \param[in] svc       service instance
 * \param[in] conn_idx  connection index
 * \param[in] status    ATT error
 */
void mcs_set_char_value_cfm(ble_service_t *svc, uint16_t conn_idx, att_error_t status);



/*
 * Notify all the connected peer devices that characteristic value has been updated.
 *
 * \param[in] svc       service instance
 * \param[in] value     updated characteristic value
 */
void mcs_notify_char_value_all(ble_service_t *svc, const uint8_t *value);



#endif /* MY_CUSTOM_SERVICE_H_ */
