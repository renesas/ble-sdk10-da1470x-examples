/**
 ****************************************************************************************
 *
 * @file misc.h
 *
 * @brief Miscellaneous functionality header file
 *
 * Copyright (c) 2022 Dialog Semiconductor. All rights reserved.
 * 
 * This software ("Software") is owned by Dialog Semiconductor. By using this Software
 * you agree that Dialog Semiconductor retains all intellectual property and proprietary
 * rights in and to this Software and any use, reproduction, disclosure or distribution
 * of the Software without express written permission or a license agreement from Dialog
 * Semiconductor is strictly prohibited. This Software is solely for use on or in
 * conjunction with Dialog Semiconductor products.
 * 
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR AS
 * REQUIRED BY LAW, THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE PROVIDED
 * IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR BY LAW, IN NO EVENT SHALL DIALOG
 * SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE.
 *
 ***************************************************************************************
 */

#ifndef MISC_H_
#define MISC_H_

#include <stdio.h>
#include "hw_gpio.h"

#ifndef DBG_PRINT_ENABLE
#define DBG_PRINT_ENABLE  ( 0 )
#endif

#ifndef DBG_IO_ENABLE
#define DBG_IO_ENABLE     ( 0 )
#endif

#ifndef DBG_CONST_NONE
#define DBG_CONST_NONE    ( 0 )
#endif

#if (DBG_CONST_NONE == 1)
# define __CONST
#else
# define __CONST    const
#endif /* DBG_CONST_NONE */

#if (DBG_PRINT_ENABLE == 1)
# define DBG_PRINTF(_f, args...)   printf((_f), ## args)
#else
# define DBG_PRINTF(_f, args...)
#endif /* DBG_PRINT_ENABLE == 1 */

/*
 * MCP4921 DAC Module Configuration Macros
 *********************************************************************************************************
 */

/* MCP4921 control register bits-masks */
#define MCP4921_AB_Msk           ( 0x8000 )
#define MCP4921_GA_Msk           ( 0x2000 )
#define MCP4921_SHDN_Msk         ( 0x1000 )
#define MCP4921_DATA_Msk         ( 0x0FFF )
#define MCP4921_CTRL_Msk         ( 0xF000 )

#define MCP4921_GET_MSK(x)       MCP4921_ ## x ## _Msk

/* MCP4921 control register bit-fields */
typedef enum {
        MCP4921_AB_CONTROL_BIT_RESET   = 0, /* Select DACA channel */
        MCP4921_GA_CONTROL_BIT_RESET   = 0, /* Output gain 1x */
        MCP4921_SHDN_CONTROL_BIT_RESET = 0, /* Shutdown the selected DAC channel  */
        MCP4921_AB_CONTROL_BIT_SET     = /*MCP4921_GET_MSK(AB)*/0,  /* Select DACB channel */
        MCP4921_GA_CONTROL_BIT_SET     = MCP4921_GET_MSK(GA),  /* Output gain 2x */
        MCP4921_SHDN_CONTROL_BIT_SET   = MCP4921_GET_MSK(SHDN) /* Activate the selected DAC channel */
} MCP4921_CONTROL_BITS;

/*
 * Set the MCP4921 2-byte register:
 *
 *          +------+-----+------+------+------+------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 *  Bit:    |  15  |  14 |  13  |  12  |  11  |  10  |  9  |  8  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
 *          +------+-----+------+------+------+------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 *  Func:   |  AB  |  -  |  GA  | SHDN |  D11 |  D10 |  D9 |  D8 |  D7 |  D6 |  D5 |  D4 |  D3 |  D2 |  D1 |  D0 |
 *          +------+-----+------+------+------+------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 **/
#define MCP4921_SET_REG(_val, _reg)     ( ((_val) & MCP4921_GET_MSK(DATA)) | ((_reg) & MCP4921_GET_MSK(CTRL)) )

#endif /* MISC_H_ */
