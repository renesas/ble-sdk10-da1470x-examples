BLE Custom Service Example {#ble_custom_service_example}
===============================

## Overview

This application is a sample implementation of device using GAP peripheral
role. It does not implement any particular profile, instead it registers
couple of different services for testing purposes. It can be also used to
execute PTS test cases against services it exposes. These services can be
controlled using debug service (for example, 1st client can interact with
services and 2nd client can control them via debug service).

Features:

- All features enabled in application can be controlled via config.h file, all
  parameters are described there.

## HW and SW configuration

- **Hardware configuration**

  - This example runs on the DA14701, DA14705 and DA14706 Bluetooth Smart SoC devices.
  - The DA1470x Pro Development kit is needed for this example.
  - Connect the Development kit to the host computer.

- **Software configuration**

  - This example requires:
  - Smartsnippets Studio 2.0.18 or higher
  - SDK10.3.1.x
  - **SEGGER J-Link** tools should be downloaded and installed.

## How to run the example

**Initial Setup**
- Import this project into the workspace
- Compile and launch RAM or OQSPI target
- Run from RAM or load in the flash

## License
**************************************************************************************

 Copyright (c) 2022 Dialog Semiconductor. All rights reserved.

 This software ("Software") is owned by Dialog Semiconductor. By using this Software
 you agree that Dialog Semiconductor retains all intellectual property and proprietary
 rights in and to this Software and any use, reproduction, disclosure or distribution
 of the Software without express written permission or a license agreement from Dialog
 Semiconductor is strictly prohibited. This Software is solely for use on or in
 conjunction with Dialog Semiconductor products.

 EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR AS
 REQUIRED BY LAW, THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE PROVIDED
 IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR BY LAW, IN NO EVENT SHALL DIALOG
 SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR
 CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE.

**************************************************************************************
