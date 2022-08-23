/**
 ****************************************************************************************
 *
 * @file tmp102_reg.c
 *
 * @brief File that includes drivers for the TMP102 temperature sensor
 *
 * Copyright (C) 2020-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include "tmp102_reg.h"

/* Pointer index for the temperature register (read only) */
#define TMP102_TMP_REG_POINTER_IDX       0x00
/* Pointer index for the configuration register (w/r) */
#define TMP102_CFG_REG_POINTER_IDX       0x01
/* Pointer index for the T-low register (w/r) */
#define TMP102_T_LOW_REG_POINTER_IDX     0x02
/* Pointer index for the T-high register (w/r) */
#define TMP102_T_HIGH_REG_POINTER_IDX    0x03

static int32_t tmp102_read_reg(tmp102_ctx_t *ctx, uint8_t reg, uint8_t *data, size_t len)
{
        int32_t ret;
        ret = ctx->read_reg(ctx->handle, reg, data, len);
        return ret;
}

static int32_t tmp102_write_reg(tmp102_ctx_t *ctx, uint8_t reg, uint8_t *data, size_t len)
{
        int32_t ret;
        ret = ctx->write_reg(ctx->handle, reg, data, len);
        return ret;
}

int32_t tmp102_temperature_read(tmp102_ctx_t *ctx, int16_t *temperature)
{
        int32_t ret;
        uint8_t tmp_data[ 2 ];
        uint16_t tmp;
        bool extended_mode;

        /* Get the 2-byte temperature with the MSB received first */
        ret = tmp102_read_reg(ctx, TMP102_TMP_REG_POINTER_IDX, tmp_data, 2);
        /* LSBit of the 2nd byte reflects the extended mode bit value */
        extended_mode = !!(tmp_data[ 1 ] & 0x1);

        tmp = ( ( ((uint16_t)tmp_data[ 0 ] << 8) | (uint16_t)tmp_data[ 1 ] ) >>
                                                                ( extended_mode ? 3 : 4 ) );

        /* Check if the value returned is negative and if so, sign extend the result */
        if (tmp & ((uint16_t)0x1 << ( extended_mode ? 12 : 11 ))) {
                tmp |= (extended_mode ? 0xF000 : 0xF800);
        }

        *temperature = tmp;
        return ret;
}

int32_t tmp102_low_limit_set(tmp102_ctx_t *ctx, int16_t low_limit, bool extended_mode)
{
        int32_t ret;
        uint16_t value = low_limit << ( extended_mode ? 3 : 4 );

        value = SWAP16(value);
        ret = tmp102_write_reg(ctx, TMP102_T_LOW_REG_POINTER_IDX, (uint8_t *)&value, 2);

        return ret;
}

int32_t tmp102_high_limit_set(tmp102_ctx_t *ctx, int16_t high_limit, bool extended_mode)
{
        int32_t ret;
        uint16_t value = high_limit << ( extended_mode ? 3 : 4 );

        value = SWAP16(value);
        ret = tmp102_write_reg(ctx, TMP102_T_HIGH_REG_POINTER_IDX, (uint8_t *)&value, 2);

        return ret;
}

int32_t tmp102_low_limit_get(tmp102_ctx_t *ctx, int16_t *low_limit, bool extended_mode)
{
        int32_t ret;
        uint8_t tmp_data[ 2 ];
        uint16_t tmp;

        /* Get the 2-byte temperature with the MSB received first */
        ret = tmp102_read_reg(ctx, TMP102_T_LOW_REG_POINTER_IDX, tmp_data, 2);

        tmp = ( ( ((uint16_t)tmp_data[ 0 ] << 8) | (uint16_t)tmp_data[ 1 ] ) >>
                                                                ( extended_mode ? 3 : 4 ) );

        /* If negative value sign extend the result */
        if (tmp & ((uint16_t)0x1 << (extended_mode ? 12 : 11))) {
                tmp |= (extended_mode ? 0xF000 : 0xF800);
        }
        *low_limit = tmp;

        return ret;
}

int32_t tmp102_high_limit_get(tmp102_ctx_t *ctx, int16_t *high_limit, bool extended_mode)
{
        int32_t ret;
        uint8_t tmp_data[ 2 ];
        uint16_t tmp;

        ret = tmp102_read_reg(ctx, TMP102_T_HIGH_REG_POINTER_IDX, tmp_data, 2);
        tmp = ( ( ((uint16_t)tmp_data[ 0 ] << 8) | (uint16_t)tmp_data[ 1 ] ) >>
                                                                ( extended_mode ? 3 : 4 ) );

        /* If negative value sign extend the result */
        if (tmp & ((uint16_t)0x1 << (extended_mode ? 12 : 11))) {
                tmp |= (extended_mode ? 0xF000 : 0xF800);
        }

        *high_limit = tmp;

        return ret;
}

int32_t tmp102_configuration_set(tmp102_ctx_t *ctx, tmp102_cfg_t *cfg)
{
        int32_t ret;

        /* Make sure the lowest nibble is zeroed */
        cfg->value &= ~0x000F;
        uint16_t value = SWAP16(cfg->value);

        ret = tmp102_write_reg(ctx, TMP102_CFG_REG_POINTER_IDX, (uint8_t *)&value, 2);
        return ret;
}

int32_t tmp102_configuration_get(tmp102_ctx_t *ctx, tmp102_cfg_t *cfg)
{
        int32_t ret;
        uint16_t value;

        ret = tmp102_read_reg(ctx, TMP102_CFG_REG_POINTER_IDX, (uint8_t *)&value, 2);
        /* Register value is received with the MSB first  */
        cfg->value = SWAP16(value);

        return ret;
}
