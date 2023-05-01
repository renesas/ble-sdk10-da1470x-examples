SPI Adapter Demonstration Example In SNC/SYSCPU Context.
======================================================

## Example Description

   This example exhibits a simple producer - consumer communication scheme over the SPI interface using the SPI adapter layer. All SPI operations are executed in the SNC context using the DG (dialog) co-operative scheduling. In this scheduler, a single-stack environment is used for all co-routines in contrast to the FreeRTOS environment where each task is assigned a separate stack instance. This imposes few restrictions when composing the co-routines; relevant comments can be found throughout the source code. In this demonstration, two DG co-routines are created; a task that employs the SPI1 block instance in master mode and a task that utilizes the SPI2 interface in slave mode. 

   User is free to employ one development kit and connect the two SPI interfaces in a loopback fashion or connect two DevKits together. The producer task is triggered periodically with the help of a SW timer initiating non-blocking transmit operations. The consumer task initiates asynchronous read operations waiting for incoming data. Once a predefined number of bytes is received, the consumer task copies them into a shared memory space and the remote master (SYSCPU) is notified to further process them. All three supported Inter-Process Communication (IPC) schemes are demonstrated, that is a simple SNC2SYS interrupt scheme, the mailbox service and the RPMsg-Lite framework.  The DA1470x family of devices comprises three SPI block instances with the SPI1 and SPI2 blocks being powered by the PD_SNC power domain and SPI3 being powered by PD_SYS. As per the SDK architecture, the SNC master is allowed to access blocks powered by the `PD_SNC` power domain and so, the SPI3 block is not permitted to be occupied  while in SNC context.

## HW and SW configuration
**Hardware configuration**

- This example runs on DA1470x Bluetooth Smart SoC devices. One DA1470x Pro Development kit is needed for this example.
- Connect the development kit to the host computer via the USB1 connector mounted on the motherboard and open a serial terminal. Two serial ports should be displayed; select the one with the lower number and then configure the terminal as depicted in the following table. 

|  Setting  | Value  |
| :-------: | :----: |
| Baudrate  | 115200 |
| Data bits |   8    |
| Stop bits |   1    |
|  Parity   |  None  |

- Connect the SPI master and slave blocks in a loopback fashion based on the following table.

|   Master    |    Slave    |
| :---------: | :---------: |
| P1_10 (D0)  | P0_26 (DI)  |
| P1_11 (CLK) | P0_25 (CLK) |

**Software configuration**
  - Recommended to employ the latest SmartSnippets Studio version released.
  - Download the latest SDK version
  - SEGGER J-Link tools should be downloaded and installed.

## How to run the example
**The RAM build configurations should be used for debugging purposes only**

### Initial Setup

- Download the source code from the Support Website.

- Import the `spi_master_slave_m33_sample_code` and `spi_master_slave_snc_sample_code` projects into your workspace (there are no path dependencies).

- The default IPC scheme is the RPMsg-Lite framework. Users can select any of the three available IPC schemes by setting the corresponding configuration macros in the `interface\app_common.h` file.  There are three valid combinations:

  - Simple SNC2SYS interrupt scheme:

  ```c
  #define dg_configUSE_MAILBOX            ( 0 )
  #define dg_configUSE_RPMSG_LITE         ( 0 )
  ```

  - Mailbox service

  ```c
  #define dg_configUSE_MAILBOX             ( 1 )
  #define dg_configUSE_RPMSG_LITE          ( 0 )
  ```

  - RPMSG-Lite framework

  ```c
  #define dg_configUSE_MAILBOX             ( 0 )
  #define dg_configUSE_RPMSG_LITE          ( 1 )
  ```

- The demonstration example is composed of two projects; one project compiled and run on the SYSCPU master and one project compiled and run on the SNC master. 

  - Open the `sdk\mailbox\include\mailbox.h` file of either project and modify the `MAILBOX_INT_MAIN` and `MAILBOX_INT_SNC` enumeration structures as follow:

    ```c
    typedef enum {
            /* RPMsg-Lite mailbox interrupt */
    #if dg_configUSE_RPMSG_LITE
            MAILBOX_INT_SNC_RPMSG_LITE,
    #endif /* dg_configUSE_RPMSG_LITE */
    
            /* Add more mailbox interrupts */
            MAILBOX_INT_SNC_APP,                    /* Index associated to the shared space that hosts the raw data */
    
            MAILBOX_INT_SNC_MAX,                    /* Must not exceed 32 */
    } MAILBOX_INT_SNC;
    ```

    ```c
    typedef enum {
            /* RPMsg-Lite mailbox interrupt */
    #if dg_configUSE_RPMSG_LITE
            MAILBOX_INT_MAIN_RPMSG_LITE,
    #endif /* dg_configUSE_RPMSG_LITE */
    
            /* Add more mailbox interrupts */
            MAILBOX_INT_MAIN_APP,                   /* Index associated to the shared space that hosts the raw data */
    
            MAILBOX_INT_MAIN_MAX,                   /* Must not exceed 32 */
    } MAILBOX_INT_MAIN;
    ```

  - Go to the SNC project and select either the debug or release build configuration. Once the the project is built go to the build folder, that should be `DA1470x-00-SNC-SPI-Debug` or`DA1470x-00-SNC-SPI-Release` respectively, and copy the `snc_files/snc_fw_embed.h `  file. This file contains the SNC binary along with symbols that reflect objects accessible by both the SNC and SYSCPU masters.

  - Go to the SYSCPU project, and paste the previously copied file in the `snc` folder. Then build the project by selecting either the OQSPI debug or release build configuration. To facilitate users, the SNC binary file is already copied under the mentioned folder.

- Download the firmware image into the XiP (eXecution-in Place) flash memory used by selecting the `program_oqspi_jtag` or  `program_oqspi_serial` launcher. Read the `Console` window of the Eclipse environment in case more steps are required to be executed. 

  - The SYSCPU project is compiled with the flash memory autodetect feature enabled meaning that any supported OQSPI flash memory can be used without modifying the custom configuration file, that is `custom_config_oqspi.h`. 

    ```c
    #define dg_configFLASH_AUTODETECT               ( 1 )
    #define dg_configOQSPI_FLASH_AUTODETECT         ( 1 )
    ```

- Press the `B1(RSTn)` push button mounted on the daughterboard, so the device starts executing its firmware. 

- A series of messages should be displayed on the terminal; In this example both SYSCPU and SNC masters print log messages using retarget operations. To distinguish which master prints which message,  the corresponding master name is appended before a retarget message, that is `[SNC]:` or `[M33]:`. This message extension is performed automatically by the retarget feature. Here is the message sequence:

  1. The producer task performs a predefined number of SPI transmissions by sending sequential numbers. 
  
     > [SNC]: SPI Producer TX OK!
     >
     > [SNC]: SPI Producer TX OK!
     >
     > [SNC]: SPI Producer TX OK!
     >
     > [SNC]: SPI Producer TX OK!
     >
     > [SNC]: SPI Producer TX OK!
     >
     > [SNC]: SPI Producer TX OK!
  
  2. At the same time the consumer task receives these bytes. Once a predefined number of bytes is received the consumer task copies them into a shared memory space and notifies the remote master (SYSCPU) to further process them.
  
     > [SNC]: Successfully received [60] bytes
  
  3. The remote master (SYSPCU) gets notified, access the shared memory space and prints them on the serial console:
  
     > [M33]: Successfully retrieved [60] bytes from the shared space. EP source address is: [48]
  
     > [M33]: 0 [M33]: 1 ...
     >
     > ...
     >

## Known Limitations 

There are no known limitations for this example. 

## License

------

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