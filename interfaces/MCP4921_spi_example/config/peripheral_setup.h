/**
 ****************************************************************************************
 *
 * @file peripheral_setup.h
 *
 * @brief Peripherals setup header file.
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

#ifndef PERIPHERAL_SETUP_H_
#define PERIPHERAL_SETUP_H_

#define MCP4921_DO_PORT      ( HW_GPIO_PORT_0 )
#define MCP4921_DO_PIN       ( HW_GPIO_PIN_9 )

#define MCP4921_DI_PORT      ( HW_GPIO_PORT_0 )
#define MCP4921_DI_PIN       ( HW_GPIO_PIN_15 )

#define MCP4921_CLK_PORT     ( HW_GPIO_PORT_0 )
#define MCP4921_CLK_PIN      ( HW_GPIO_PIN_14 )

#define MCP4921_CS_PORT      ( HW_GPIO_PORT_0 )
#define MCP4921_CS_PIN       ( HW_GPIO_PIN_19 )

#endif /* PERIPHERAL_SETUP_H_ */