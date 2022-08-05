# Factory Firmware Instructions

## Factory Firmware Description

The Smartbond DA1470X Development Kit-Pro is delivered preloaded with an example application showcasing a wearable UI on the display board, using the open-source LVGL framework, a Mass Storage Device (MSD) application containing a Readme.html file and an implementation of a Bluetooth® Low Energy proximity reporter. 

**Operation of the LVGL demo software**

1/ Connect the DA1470X Kit to a computer (USB1 on the motherboard)
2/ A watch face with 3 clock hands will appear on the display
3/ Slide from left to right to get the menu screen
4/ Sliding up/down will show more menu items
5/ Press a menu item for further content (only a few menu items have consecutive screens)

**Operation of the Proximity Reporter**

1/ Once the Development Kit-Pro is connected to USB, the proximity reporter starts advertising
2/ Download one of the available Bluetooth® Low Energy connectivity apps for Android or iOS.
3/ Browse for a device named Renesas PX ReporterXXXX. The last 4 numbers being the end of the device address
4/ Connect and change the alert level between 0, 1 and 2
5/ The red LED blinks at a faster or slower pace depending on the alert level

**Operation of SmartMSD**

1/ Connect the DA1470X Kit to a computer (USB1 on the motherboard)
2/ Connect the daughter board USB connector (J8) to the computer
3/ Browse the Host system for Mass Storage Device (External Disk)
4/ No special driver is required
5/ Confirm a portable storage disk is attached to Host
6/ One file (Readme.html) is shown on the portable storage in computer file browser.

### HW & SW Configurations

- **Hardware Configurations**

  - This example runs on a DA1470x Bluetooth Smart SoC.
  - A DA14706 Pro Development Kit is needed for this example.
  - For more information on the HW setup please refer to [UM-B-153: DA1470x Getting Started User Manual ](https://www.dialog-semiconductor.com/DA1470x_Getting_Started_manual "UM-B-153: DA1470x Getting Started User Manual ")
  
- **Software Configurations**

  - Download the latest SDK version for the DA1470x family of devices
  - **SEGGER's J-Link** tools should be downloaded and installed.


## Build Configurations
1. DA1470x-00-Debug_OQSPI_demo build configuration. The user can interact with the watch demo application by touching the LCD screen.
2. DA1470x-00-Release_OQSPI_demo

**Write the build binary file into the (O)QSPI flash on the DA1470x daughterboard using the SmartSnippetsStudio**

## Write the Watch Demo resources binary to the QSPI External Flash

1. The cli_programmer.exe, uartboot.bin and mkimage.exe applications should be already built in order to be able to write the resources at the Flash.
2. Open a Windows PowerShell and navigate into binaries folder of the root SDK folder.
3. Locate the graphics resource file (Can be found in ..\factory_firmware\ui\demo\resources\bitmaps\WatchDemoColoredResources.bin)
4. Use the following command to write the resources to the QSPI External flash
	`./cli_programmer.exe -i 115200 -s 115200 COM8 write_qspi 0x0 ..\...  ..\WatchDemoColoredResources.bin`
(Also the gdbserver can be used: Start the Segger GDBserver and use the following command: `cli_programmer.exe gdbserver write_qspi 0x0 ..\... ...\WatchDemoColoredResources.bin`)



5. (re)Start the application by pressing the Reset button on the DA1470x daughterboard.

## Log Messages
The logging and the output of the performance metrics are available in a serial terminal. 
1. Open a serial terminal.
2. Each DA1470x device has two serial ports enabled. The smaller COM port will be used.
3. Set the Speed to 115200.
4. Set the Data to 8bit.
5. Set the Parity to None.
6. Set the Stop Bits to 1bit.
7. Set the Flow Control to None.

