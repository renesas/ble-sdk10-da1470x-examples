I2C/I3c LSM6DSOX example application. {#lsm6dsox_i2c_i3c_example}
======================================================

## Overview

This demo shows I2C and I3C capabilities of DA1470x device to drive a connected LSM6DSOX accelerometer.
The demo uses STEVAL-MK1197V1 chipset from ST(https://www.st.com/resource/en/data_brief/steval-mki197v1.pdf)
This is an adapter board for LSM6DSOX accelerometer.
The demo supports FIFO to get data from sensor FIFO and also activity detector reporter
LSM6DSOX has 4 operating modes and this demo is using Mode 1.

## Requirements
* **Hardware configuration**

    - This example runs on the DA1470x Bluetooth Smart SoC devices.
    - The DA1470x Pro Development kit is needed for this example.
	- STEVAL-MK1197V1 chipset from ST(https://www.st.com/resource/en/data_brief/steval-mki197v1.pdf)
    - Connect the Development kit to the host computer.

* **UART terminal with the following settings is needed to observe the master task
printouts**

| Setting      | Value    |
|:------------:|:--------:|
| Baudrate     | 115200   |
| Data bits    | 8        |
| Stop bits    | 1        |
| Parity       | None     |


* **The following wiring/connections are needed between The DA1470x Pro Development kit and STEVAL-MK1197V1 chipset(for both I2C and I3C)**

	- SDA(STEVAL) to P111
    - SCL(STEVAL) to P112
	- CS(STEVAL) to VLED
    - VDD(STEVAL) to V30
	- VDDIO(STEVAL) to VSYS
	- GND(STEVAL) to GND
	- SD0(STEVAL) to GND 
	- SCX(STEVAL) to GND 
	- SDX(STEVAL)  to GND.
	- 10 K pull-up resistors need to be soldered to SDA and SCL line of cobalt demo board to drive SCL/SDA high, this is needed only for i2c test not i3c)
	- INT1 pin of STEVAL must be kept floating to let I2C and I3C coexist(for more details please refer to section 5.3 in https://www.st.com/resource/en/datasheet/lsm6dsox.pdf )

## Interface(i3c vs i2c) selection and test(FIFO vs activity detector ) selection

To select I3C interface user should activate dg_configUseI3CHandling and deactivate dg_configUseI2CHandling(and viceversa) in custom_config_ram.h or custom_config_oqspi.h depending
on the configuration selected.
To select the test case user should activate LSM6DSOX_FIFO and deactivate LSM6DSOX_ACTIVITY_DETECT (and viceversa) in custom_config_ram.h or custom_config_oqspi.h depending
on the configuration selected.
For example : below settings configures the example to use i3c communication and activity detector reporter

#define dg_configUseI3CHandling

//#define dg_configUseI2CHandling

#define LSM6DSOX_ACTIVITY_DETECT

//#define  LSM6DSOX_FIFO

* **Software configuration**
  - This example requires:
  - Smartsnippets Studio V2.0.18 or greater.
  - SDK 10.2.44 and onwards
  - **SEGGER J-Link** tools should be downloaded and installed.

## Installation procedure

To install the project follow the [General Installation and Debugging Procedure](@ref install_and_debug_procedure).

## File structure

The following file structure will be created:

* projects/dk_apps/texamples/i2c_example
        * config  
        * sdk
        * startup
        * lsm6dsox_drv
		* lsm6dsox_task.c
		* lsm6dsox_task.h
        * main.c

## Existing build configurations

The template contains build configurations for executing it from RAM or QSPI. 

- `DA1470X-00-Debug_RAM`. The project is built to be run from RAM. The executable is built with debug (-Og) information.
- `DA1470X-00-Debug_OQSPI`. The project is built to be run from QSP. The executable is built with debug (-Og) information.
- `DA1470X-00-Release_RAM`. The project is built to be run from RAM. The executable is built with no debug information and size optimization (-Os).
- `DA1470X-00-Release_OQSPI`. The project is built to be run from QSPI. The executable is built with no debug information and size optimization (-Os).

## Dependencies / Considerations
- This project can not be shared outside Dialog before checking if the customer has signed the SLA. 


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

