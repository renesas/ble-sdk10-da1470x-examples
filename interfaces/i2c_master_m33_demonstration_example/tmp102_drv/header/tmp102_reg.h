/**
 ****************************************************************************************
 *
 * @file tmp102_reg.h
 *
 * @brief File that includes driver definitions for the TMP102 temperature sensor.
 *
 * Copyright (C) 2020-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef TMP102_REG_H_
#define TMP102_REG_H_

#include "osal.h"

#define TMP102_TEMP_RESOLUTION          ( (float)0.0625 )

/*
 * Helper macro to convert a float value (i.e. temperature) to the corresponding raw value
 * that will be written directly to the registers of the sensor.
 *
 * \sa tmp102_low_limit_set()
 * \sa tmp102_high_limit_set()
 */
#define TMP102_FLOAT_TO_RAW(_float)     ( (int16_t)((float)(_float) / TMP102_TEMP_RESOLUTION) )

/*
 * Helper macro to convert a raw value (as retrieved from the register of the sensor) to the
 * corresponding real value (i.e. temperature).
 *
 * \sa tmp102_temperature_read()
 * \sa tmp102_low_limit_get()
 * \sa tmp102_high_limit_get()
 */
#define TMP102_RAW_TO_FLOAT(_raw)       ( (float)((float)(_raw) * TMP102_TEMP_RESOLUTION) )

/* Function pointer to perform a write access to the TMP102 sensor */
typedef int32_t (*tmp102_write_ptr)(void *, uint8_t, uint8_t *, size_t);
/* Function pointer to perform a read access to the TMP1028 sensor */
typedef int32_t (*tmp102_read_ptr)(void *, uint8_t, uint8_t *, size_t);

/*
 * \brief TMP102 sensor context.
 *
 * The application should define the actual context which will be used by the low level
 * drivers of the sensor to perform the actual write/read operations.
 */
typedef struct {
        tmp102_write_ptr write_reg;
        tmp102_read_ptr read_reg;
        void *handle;
} tmp102_ctx_t;

/*
 * \brief TMP102 sensor configuration register
 *
 * The sensor comprises a single 2-byte configuration register. The user can define the
 * configuration value either directly or use the bit-field definitions to define each
 * sub-field separately and with the help of the corresponding enumeration structures.
 *
 * \sa TMP102_CONVERSION_RATE
 * \sa TMP102_EXTENDED_MODE
 * \sa TMP102_FAULT_QUEUE
 * \sa TMP102_ALERT_POLARITY
 */
typedef struct {
        union {
                uint16_t value;
                struct { // bit-field structure
                        uint16_t unused           : 4;
                        uint16_t extended_mode    : 1;
                        uint16_t alert            : 1;
                        uint16_t conversion_rate  : 2;
                        uint16_t shutdown_mode    : 1;
                        uint16_t thermostat_mode  : 1;
                        uint16_t alert_polarity   : 1;
                        uint16_t fault_queue      : 2;
                        uint16_t resolution       : 2;
                        uint16_t one_shot         : 1;
                };
        };
} tmp102_cfg_t;

/*
 * \brief TMP102 sensor conversion rate
 */
typedef enum {
        TMP102_CONVERSION_RATE_0_25_HZ  = 0,
        TMP102_CONVERSION_RATE_1_HZ        ,
        TMP102_CONVERSION_RATE_4_HZ        ,
        TMP102_CONVERSION_RATE_8_HZ
} TMP102_CONVERSION_RATE;

/*
 * \brief TMP102 converter resolution
 *
 * The sensor supports two resolutions, that is 12 and 13 bits. The values can be either
 * negative or positive which is indicated by the MSBit. The extended mode allows values
 * greater than 128C.
 */
typedef enum {
        TMP102_EXTENDED_MODE_12_BIT  = 0,
        TMP102_EXTENDED_MODE_13_BIT
} TMP102_EXTENDED_MODE;

/*
 * \brief TMP102 sensor fault queue samples.
 *
 * The fault queue is provided to prevent a false alert as a result of environmental
 * noise. The fault queue requires consecutive fault measurements in order to
 * trigger the alert function.
 */
typedef enum {
        TMP102_FAULT_QUEUE_1_SAMPLE  = 0,
        TMP102_FAULT_QUEUE_2_SAMPLES    ,
        TMP102_FAULT_QUEUE_4_SAMPLES    ,
        TMP102_FAULT_QUEUE_6_SAMPLES
} TMP102_FAULT_QUEUE;

/*
 * \brief TMP102 sensor alert polarity output.
 */
typedef enum {
        TMP102_ALERT_POLARITY_ACTIVE_LOW  = 0,
        TMP102_ALERT_POLARITY_ACTIVE_HIGH
} TMP102_ALERT_POLARITY;

/*
 * \brief TMP102 sensor alert mode.
 *
 * The sensor supports two alert mode. In specific:
 *
 * Comparator Mode: The alert pin is activated when the temperature equals or exceeds
 *                  Thigh remains active until the temperature falls below Tlow.
 *
 * Interrupt Mode: The alert pin is activated when the temperature exceed Thigh or goes
 *                  below Tlow.
 */
typedef enum {
        TMP102_MODE_COMPARATOR  = 0,
        TMP102_MODE_INTERRUPT
} TMP102_MODE;

/*
 * \brief Get the current temperature.
 *
 * Two bytes must be read to obtain the temperature; byte 1 is the MSB, followed by byte 2,
 * the LSB. The first 12 (or 13 bits in extended mode) are used to indicate the temperature.
 *
 * \param[in]  ctx          Pointer to the temperature context containing information on how
 *                          to access the sensor.
 *
 * \param[out] temperature  Pointer to the temperature value returned by the driver. To convert it
 *                          to a real value use the TMP102_RAW_TO_FLOAT() helper macro.
 *
 * \note Bit D0 of byte 2 indicates normal or extended mode and can be used to distinguish
 *       between the two temperature register data formats
 */
int32_t tmp102_temperature_read(tmp102_ctx_t *ctx, int16_t *temperature);

/*
 * \brief Set the low temperature threshold (boundaries of the alert signaling)
 *
 * \param[in] ctx           Pointer to the sensor context containing information on how to access
 *                          the sensor.
 *
 * \param[in] low_limit     The raw value that should be written to the corresponding register.
 *                          The user can use the TMP102_FLOAT_TO_RAW() helper macro to convert
 *                          a real value to the corresponding raw value.
 *
 * \param[in] extended_mode True if \p low_limit reflects a 13-bit value; false if it reflects a
 *                          12-bit value.
 */
int32_t tmp102_low_limit_set(tmp102_ctx_t *ctx, int16_t low_limit, bool extended_mode);

/*
 * \brief Set the high temperature threshold (boundaries of the alert signaling)
 *
 * \param[in] ctx           Pointer to the sensor context containing information on how
 *                          to access the sensor
 *
 * \param[in] high_limit    The raw value that should be written to the corresponding register.
 *                          The user can use the TMP102_FLOAT_TO_RAW() helper macro to convert
 *                          a real value to the corresponding raw value.
 *
 * \param[in] extended_mode True if \p high_limit reflects a 13-bit value; false if it reflects a
 *                          12-bit value.
 */
int32_t tmp102_high_limit_set(tmp102_ctx_t *ctx, int16_t high_limit, bool extended_mode);

/*
 * \brief Get the low temperature threshold (boundaries of the alert signaling)
 *
 * \param[in]  ctx           Pointer to the sensor context containing information on how to access
 *                           the sensor.
 *
 * \param[out] low_limit     Pointer to the low limited value returned by the driver. User should
 *                           use the TMP102_RAW_TO_FLOAT() helper macro to convert it to a real
 *                           temperature value.
 *
 * \param[in]  extended_mode True if \p low_limit reflects a 13-bit value; false if it reflects a
 *                           12-bit value.
 */
int32_t tmp102_low_limit_get(tmp102_ctx_t *ctx, int16_t *low_limit, bool extended_mode);

/*
 * \brief Get the high temperature threshold (boundaries of the alert signaling)
 *
 * \param[in]  ctx           Pointer to the sensor context containing information on how
 *                           to access the sensor.
 *
 * \param[out] high_limit    Pointer to the low limited value returned by the driver. User should
 *                           use the TMP102_RAW_TO_FLOAT() helper macro to convert it to a real
 *                           temperature value.
 *
 * \param[in]  extended_mode True if \p high_limit reflects a 13-bit value; false if it reflects a
 *                           12-bit value.
 */
int32_t tmp102_high_limit_get(tmp102_ctx_t *ctx, int16_t *high_limit, bool extended_mode);

/*
 * \brief Set the control value. The TMP102 sensor exhibits a single 2-byte control register.
 *
 * \param[in] ctx   Pointer to the sensor context containing information on how to access the sensor.
 *
 * \param[in] cfg   Pointer to a structure containing the control value. User can write the value
 *                  either directly or use the bit-field definitions to write each sub-entry
 *                  separately.
 */
int32_t tmp102_configuration_set(tmp102_ctx_t *ctx, tmp102_cfg_t *cfg);

/*
 * \brief Get the control value. The TMP102 sensor exhibits a single 2-byte control register.
 *
 * \param[in]  ctx   Pointer to the sensor context containing information on how to access the sensor.
 *
 * \param[out] cfg   Pointer to a structure containing the control value returned by the driver.
 *                   User can read the value either directly or use the bit-field definitions
 *                   to read each sub-entry separately.
 */
int32_t tmp102_configuration_get(tmp102_ctx_t *ctx, tmp102_cfg_t *cfg);

#endif /* TMP102_REG_H_ */
