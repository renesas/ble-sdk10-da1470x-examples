# GPIO Pins Handling

The GPIO peripheral block is powered by the MEM power domain (PD_MEM) and thus, it it is always enabled during M33 runtime.
GPIO Pins must be set to latch disabled state before the ARM M33 enters sleep and activated at M33 wakeup. 
Please note that the developer does not have to control the GPIO pins used by adapters (e.g. I2C pins). 
This example demonstrates two possible mechanisms for handling GPIO pins that are not handled by adapters. 
User can switch between the two mechanisms using the `GPIO_HANDLING_DYNAMICALLY` macro in `main.c` file. 

### HW & SW Configurations

- **Hardware Configurations**
    - This example runs on a DA1470x Bluetooth Smart SoC.
    - A DA1470x Pro Development Kit is needed for this example.
- **Software Configurations**
    - Download the latest SDK version for the DA1470x family of devices (10.0.10.x)
    - **SEGGER's J-Link** tools should be downloaded and installed.


## How to run the example

### Initial Setup

- Download the source code from the Support Website
- Import the project into your workspace.
- Connect the target device to your host PC.
- Compile the code (either in Release or Debug mode) and load it into the chip.
- Open a serial terminal (115200/8 - N - 1)
- Press the reset button on DA1470x daughterboard to start executing the application.

- Monitor the GPIO P0_22 to verify that it toggles every one second. 

## Known Limitations

There are no known limitations for this sample code.

## License
**************************************************************************************

 Copyright (c) 2021 Dialog Semiconductor. All rights reserved.

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