# Example demonstrating the use of UART Adapter abstraction layer

## Overview

This application demonstrates the use of all three UARTs through the Adapters abstraction layer.
The name of the project is **UART_Adapter_example**.

For **UART1** the echo-back is implemented without flow control since UART1 does not support RTS/CTS functionality. The echo implementation is using blocking calls of the UART adapter API via a single task. The task initiates a UART read for a single character which is transmitted back once received.

For **UART2** the echo-back is using HW flow control with RTS/CTS. The implementation of the echo-back is using two tasks, one receiving a character from PC and one transmitting a character to the UART. The implementation for the UART2 uses an OS Queue for passing the received characters to TX task.

For **UART3** the echo-back is implemented in the same way as the UART1 using HW flow control with RTS/CTS.

The code for the UART tasks implementation is located in the **uart_tasks.c** file.
The **system_init()** function, in the **main.c** file creates and initializes all the tasks needed as well as the OS Queue.

The configurations of all UARTs is located in the **platform_devices.c** file and the UART pin selection is located in the **peripheral_setup.h** file. There is no need to add anything in the **periph_init()** function since all the pin configurations are done from the UART adapter.

**Note** 
The example can be downloaded from [Here](http://lpccs-docs.dialog-semiconductor.com/SW_Example/SDK10/DA1469x_UART_Adapter_Example.zip).


## HW and SW configuration

- **Hardware configuration**

  - This example runs on DA147xx Bluetooth Smart family devices.
  - The Pro Development kit is needed for this example.
  - Connect the Development kit to the host computer.

- **Software configuration**

  - Recommended to employ the latest SmartSnippets Studio version released.
  - Download the latest SDK version
  - SEGGER J-Link tools should be downloaded and installed.

## How to run the example

### Pin Assignments

The pins configured in this example for each UART are shown below:

| UART # |  RX  |  TX  | RTSn | CTSn |
|--------|------|------|------|------|
| **UART-1** | P1_8 | P1_9 |
| **UART-2** | P2_1 | P0_8 | P0_29| P0_11|
| **UART-3** | P0_26| P0_27| P0_28| P1_29|

### Running and testing the application

To run and test the application on a ProDK the following HW and SW is required.

**<u>HW:</u>** Need to have two USB to 3.3V TTL USB dongles with RTS/CTS support.

**<u>SW:</u>** use the Terminal within the SSS or have a terminal application like Putty installed on the PC.

**<u>Testing UART1:</u>**

Connect an external USB to TTL UART (e.g. an FTDI USB to TTL module) to ProDK UART-1 pins as follows:

| ext UART Pin | UART-1 pin |
| ------------ | :--------- |
| RX           | TX         |
| TX           | RX         |

Plug in the USB-to-TTL dongle to a USB port of the PC. There will be a new COM port available in the system. Check the device manager to identify which one it is.

Start the terminal application on the PC (e.g. Putty on Windows or equivalent on other OSs) and configure it to use the COM port as mentioned above. Configure the COM port parameters as follows:

| Parameter    | Value                                   |
| ------------ | --------------------------------------- |
| Baud Rate    | 115200                                  |
| Data Bits    | 8                                       |
| Stop Bits    | 1                                       |
| Parity       | None                                    |
| Flow Control | None |

Do not enable any echo back feature in the terminal app. Start typing in the terminal window. Every    character typed goes to DA147xx UART-1 and returns back so it will appear in the terminal window.

If the user presses 'ESC' (ASCII=27), then the task implementing the echo on  the UART-1 will terminate and the operation will seize on this UART.

**<u>Testing UART2:</u>**

Plug in the ProDK to a USB port of the PC. There will be two COM ports added. Check the Windows Device Manager and check the COM port with the lowest number, this is the one mapped to UART2 of the DA147xx.

Start the terminal application on the PC (e.g. Putty on Windows or equivalent on other OSs) and configure it to use the COM port as mentioned above. Configure the COM port parameters as follows:

| Parameter    | Value                                   |
| ------------ | --------------------------------------- |
| Baud Rate    | 115200                                  |
| Data Bits    | 8                                       |
| Stop Bits    | 1                                       |
| Parity       | None                                    |
| Flow Control | RTS/CTS (or might be called 'hardware') |

Do not enable any echo back feature in the terminal app. Start typing in the terminal window.  Every     character typed goes to DA147xx UART-2 and returns back, so it will appear in the terminal window.

If the user presses 'ESC' (ASCII=27), then the task implementing the echo on  the UART-2 will terminate and the operation will seize on this UART.

The behaviour of the application in case the DA147xx-RTSn or the DA147xx-CTSn gets disconnected, while the user keeps typing, differs: 
 * In the CTS case (assuming that the RTS of the PC gets disconnected from the DA147xx-CTSn, thus da147xx considers its CTSn de-asserted - high due to the internal pull-up). The da147xx will continue receiving characters but not echoing anything back until the internal queue is full. As soon as the queue is depleted the da147xx-RTSn will be de-asserted preventing any additional transmition from the PC terminal. 
* In the RTS disconnection case the transactions will continue but the there will be missing characters.

**Testing UART3:**

Connect an external USB to TTL UART (e.g. an FTDI USB to TTL module) to ProDK UART-3 pins as follows:

| ext UART Pin | UART-3 pin |
| ------------ | :--------- |
| RX           | TX         |
| TX           | RX         |
| CTS          | RTS        |
| RTS          | CTS        |

Plug in the USB-to-TTL dongle to a USB port of the PC. There will be a new COM port available in the system. Check the device manager to identify which one it is.

Start the terminal application on the PC (e.g. Putty on Windows or equivalent on other OSs) and configure it to use the COM port as mentioned above. Configure the COM port parameters as follows:

| Parameter    | Value                                   |
| ------------ | --------------------------------------- |
| Baud Rate    | 115200                                  |
| Data Bits    | 8                                       |
| Stop Bits    | 1                                       |
| Parity       | None                                    |
| Flow Control | RTS/CTS (or might be called 'hardware') |

Do not enable any echo back feature in the terminal app. Start typing in the terminal window.  Every     character typed goes to DA147xx UART-3 and returns back so it will appear in the terminal window.

If the user presses 'ESC' (ASCII=27), then the task implementing the echo on  the UART-3 will terminate and the operation will seize on this UART.

The behaviour of the application in case the DA147xx-RTSn or the DA147xx-CTSn gets disconnected, while the user keeps typing, differs. 
 * In the CTS case (assuming that the RTS of the PC gets disconnected from the DA147xx-CTSn, thus da147xx considers its CTSn de-asserted - high due to the internal pull-up). The device will receive characters up until its TX FIFO level threshhold setting (for every received character a character is transmitted). One extra character received will force one extra character to be pushed to the TX FIFO and since the threshold is allready reached the device will wait until the previous characters are pushed out. This will force the watchdog timer to elapse and trigger an NMI interrupt.
* For RTS case (assuming that the CTS of the PC gets disconnected from the DA147xx-RTSn, thus da147xx cannot signal the PC to stop sending data). In that case the transactions will continue but there will be missing characters.

## Known Limitations
There are no known limitations for this application.

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
