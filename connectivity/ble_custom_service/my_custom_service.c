/**
 ****************************************************************************************
 *
 * @file my_custom_service.c
 *
 * @brief BLE custom service
 *
 * Copyright (C) 2015-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "osal.h"
#include "ble_att.h"
#include "ble_bufops.h"
#include "ble_common.h"
#include "ble_gatt.h"
#include "ble_gatts.h"
#include "ble_storage.h"
#include "ble_uuid.h"
#include "my_custom_service.h"


#define UUID_GATT_CLIENT_CHAR_CONFIGURATION (0x2902)

static const char char_user_descriptor_val[]  = "Switch ON/OFF Green LED on DevKit";


void mcs_notify_char_value(ble_service_t *svc, uint16_t conn_idx, const uint8_t *value);

/* Service related variables */
typedef struct {
        ble_service_t svc;

        // User-defined callback functions
        const my_custom_service_cb_t *cb;

        // Attribute handles of Bluetooth LE service
        uint16_t mc_char_value_h;
        uint16_t mc_char_value_ccc_h;

} mc_service_t;




/* This function is called upon write requests to characteristic attribute value */
static att_error_t do_char_value_write(mc_service_t *mcs, uint16_t conn_idx,
                           uint16_t offset, uint16_t length, const uint8_t *value)
{
        uint8_t ct;

        if (offset) {
                return ATT_ERROR_ATTRIBUTE_NOT_LONG;
        }


        /* Check if the length of the envoy data exceed the maximum permitted */
        if (length != 1) {
                return ATT_ERROR_INVALID_VALUE_LENGTH;
        }

        /*
         * Check whether the application has defined a callback function
         * for handling the event.
         */
        if (!mcs->cb || !mcs->cb->set_characteristic_value) {
                return ATT_ERROR_WRITE_NOT_PERMITTED;
        }

        ct = get_u8(value);

        /*
         * The application should get the data written by the peer device.
         */
        mcs->cb->set_characteristic_value(&mcs->svc, conn_idx, &ct);

        return ATT_ERROR_OK;

}


/* This function is called upon write requests to CCC attribute value */
static att_error_t do_char_value_ccc_write(mc_service_t *mcs, uint16_t conn_idx,
                              uint16_t offset, uint16_t length, const uint8_t *value)
{
        uint16_t ccc;

        if (offset) {
                return ATT_ERROR_ATTRIBUTE_NOT_LONG;
        }

        if (length != sizeof(ccc)) {
                return ATT_ERROR_INVALID_VALUE_LENGTH;
        }

        ccc = get_u16(value);

        /* Store the envoy CCC value to the ble storage */
        ble_storage_put_u32(conn_idx, mcs->mc_char_value_ccc_h, ccc, true);

        return ATT_ERROR_OK;
}





/* This function is called upon read requests to characteristic attribue value */
static void do_char_value_read(mc_service_t *mcs, const ble_evt_gatts_read_req_t *evt)
{
        /*
         * Check whether the application has defined a callback function
         * for handling the event.
         */
        if (!mcs->cb || !mcs->cb->get_characteristic_value) {
                ble_gatts_read_cfm(evt->conn_idx, evt->handle,
                                                   ATT_ERROR_READ_NOT_PERMITTED, 0, NULL);
                return;
        }

        /*
         * The application should provide the requested data to the peer device.
         */
        mcs->cb->get_characteristic_value(&mcs->svc, evt->conn_idx);

        // callback executed properly

}

/*-------------------------------------------------------------------------------------*/



/*
 * Notify all the connected peer devices that characteristic attribute value
 * has been updated
 */
void mcs_notify_char_value_all(ble_service_t *svc, const uint8_t *value)
{
        uint8_t num_conn;
        uint16_t *conn_idx;

        ble_gap_get_connected(&num_conn, &conn_idx);

        while (num_conn--) {
                mcs_notify_char_value(svc, conn_idx[num_conn], value);
        }

        if (conn_idx) {
                OS_FREE(conn_idx);
        }
}



/* Notify the peer device that characteristic attribute value has been updated */
void mcs_notify_char_value(ble_service_t *svc, uint16_t conn_idx, const uint8_t *value)
{
        mc_service_t *mcs = (mc_service_t *) svc;

        uint16_t ccc = 0x0000;
        uint8_t pdu[1];

        ble_storage_get_u16(conn_idx, mcs->mc_char_value_ccc_h, &ccc);


        /*
         * Check if the notifications are enabled from the peer device,
         * otherwise don't send anything.
         */
        if (!(ccc & GATT_CCC_NOTIFICATIONS)) {
                return;
        }

        pdu[0] = *((uint8_t *)value);

        ble_gatts_send_event(conn_idx, mcs->mc_char_value_h, GATT_EVENT_NOTIFICATION,
                                                                      sizeof(pdu), pdu);
}




/*
 * This function should be called by the application as a response to read requests
 */
void mcs_get_char_value_cfm(ble_service_t *svc, uint16_t conn_idx, att_error_t status,
                                                                    const uint8_t *value)
{
        mc_service_t *mcs = (mc_service_t *) svc;
        uint8_t pdu[1];

        pdu[0] = *value;

        /* This function should be used as a response for every read request */
        ble_gatts_read_cfm(conn_idx, mcs->mc_char_value_h, ATT_ERROR_OK,
                                                                     sizeof(pdu), pdu);
}


/*
 * This function should be called by the application as a response to write requests
 */
void mcs_set_char_value_cfm(ble_service_t *svc, uint16_t conn_idx, att_error_t status)
{
        mc_service_t *mcs = (mc_service_t *) svc;

        /* This function should be used as a response for every write request */
        ble_gatts_write_cfm(conn_idx, mcs->mc_char_value_h, status);
}


/* Handler for read requests, that is BLE_EVT_GATTS_READ_REQ */
static void handle_read_req(ble_service_t *svc, const ble_evt_gatts_read_req_t *evt)
{
        mc_service_t *mcs = (mc_service_t *) svc;

        /*
         * Identify for which attribute handle the read request has been sent to
         * and call the appropriate function.
         */

        if (evt->handle == mcs->mc_char_value_h) {
                do_char_value_read(mcs, evt);
        } else if (evt->handle == mcs->mc_char_value_ccc_h) {
                uint16_t ccc = 0x0000;

                /* Extract the CCC value from the ble storage */
                ble_storage_get_u16(evt->conn_idx, mcs->mc_char_value_ccc_h, &ccc);

                // We're little-endian - OK to write directly from uint16_t
                ble_gatts_read_cfm(evt->conn_idx, evt->handle, ATT_ERROR_OK,
                                                                  sizeof(ccc), &ccc);

        /* Otherwise read operations are not permitted */
        } else {
                ble_gatts_read_cfm(evt->conn_idx, evt->handle,
                                                ATT_ERROR_READ_NOT_PERMITTED, 0, NULL);
        }

}


/* Handler for write requests, that is BLE_EVT_GATTS_WRITE_REQ */
static void handle_write_req(ble_service_t *svc, const ble_evt_gatts_write_req_t *evt)
{
        mc_service_t *mcs = (mc_service_t *) svc;
        att_error_t status = ATT_ERROR_WRITE_NOT_PERMITTED;

        /*
         * Identify for which attribute handle the write request has been sent to
         * and call the appropriate function.
         */

        if (evt->handle == mcs->mc_char_value_h) {
                status = do_char_value_write(mcs, evt->conn_idx, evt->offset,
                                                            evt->length, evt->value);
        } else if (evt->handle == mcs->mc_char_value_ccc_h) {
                status = do_char_value_ccc_write(mcs, evt->conn_idx, evt->offset,
                                                            evt->length, evt->value);
        }

        if (status == ((att_error_t) - 1)) {
                // Write handler executed properly, will be replied by cfm call
                return;
        }

        /* Otherwise write operations are not permitted */
        ble_gatts_write_cfm(evt->conn_idx, evt->handle, status);
}



/* Function to be called after a cleanup event */
static void cleanup(ble_service_t *svc)
{
        mc_service_t *mcs = (mc_service_t *) svc;

        ble_storage_remove_all(mcs->mc_char_value_ccc_h);

        OS_FREE(mcs);
}


/* Initialization function for My Custom Service (mcs).*/
ble_service_t *mcs_init(const uint8_t *variable_value ,const my_custom_service_cb_t *cb)
{
        mc_service_t *mcs;

        uint16_t num_attr;
        att_uuid_t uuid;

        uint16_t char_user_descriptor_h;

        /* Allocate memory for the sevice hanle */
        mcs = (mc_service_t *)OS_MALLOC(sizeof(*mcs));
        memset(mcs, 0, sizeof(*mcs));


        /* Declare handlers for specific Bluetooth LE events */
        mcs->svc.read_req  = handle_read_req;
        mcs->svc.write_req = handle_write_req;
        mcs->svc.cleanup   = cleanup;
        mcs->cb = cb;


        /*
         * 0 --> Number of Included Services
         * 1 --> Number of Characteristic Declarations
         * 2 --> Number of Descriptors
         */
        num_attr = ble_gatts_get_num_attr(0, 1, 2);


        /* Service declaration */
        ble_uuid_from_string("00000000-1111-2222-2222-333333333333", &uuid);
        ble_gatts_add_service(&uuid, GATT_SERVICE_PRIMARY, num_attr);


        /* Characteristic declaration */
        ble_uuid_from_string("11111111-0000-0000-0000-111111111111", &uuid);
        ble_gatts_add_characteristic(&uuid, GATT_PROP_READ | GATT_PROP_NOTIFY |
                      GATT_PROP_WRITE, ATT_PERM_RW, 1, GATTS_FLAG_CHAR_READ_REQ,
                                                       NULL, &mcs->mc_char_value_h);


        /* Define descriptor of type Client Characteristic Configuration (CCC) */
        ble_uuid_create16(UUID_GATT_CLIENT_CHAR_CONFIGURATION, &uuid);
        ble_gatts_add_descriptor(&uuid, ATT_PERM_RW, 2, 0, &mcs->mc_char_value_ccc_h);

        /* Define descriptor of type Characteristic User Description (CUD) */
        ble_uuid_create16(UUID_GATT_CHAR_USER_DESCRIPTION, &uuid);
        ble_gatts_add_descriptor(&uuid, ATT_PERM_READ, sizeof(char_user_descriptor_val),
                                                              0, &char_user_descriptor_h);


        /*
         * Register all the attribute handles so that they can be updated
         * by the Bluetooth LE manager automatically.
         */
        ble_gatts_register_service(&mcs->svc.start_h, &mcs->mc_char_value_h,
                         &mcs->mc_char_value_ccc_h, &char_user_descriptor_h ,0);


        /* Calculate the last attribute handle of the Bluetooth LE service */
        mcs->svc.end_h = mcs->svc.start_h + num_attr;

        /* Set default attribute values */
        ble_gatts_set_value(mcs->mc_char_value_h, 1, variable_value);
        ble_gatts_set_value(char_user_descriptor_h,  sizeof(char_user_descriptor_val),
                                                               char_user_descriptor_val);

        /* Register the Bluetooth LE service in Bluetooth LE framework */
        ble_service_add(&mcs->svc);

        /* Return the service handle */
        return &mcs->svc;

}
