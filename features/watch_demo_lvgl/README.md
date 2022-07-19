# Watch Demo Instructions

##Example Description

This sample code provides a demonstration of the LVGL graphics framework.

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
2. DA1470x-00-Debug_OQSPI_touch_simulation build configuration is used to get performance metrics for the FPS, the GPU rendering time and the CPU utilization. The touch simulation is used to be able to switch between the current screens and measure the demo perfomance.
  
## Write the Watch Demo resources binary to the QSPI External Flash

1. Open a Windows PowerShell and navigate into binaries folder of the root SDK folder.
2. Copy the file 'WatchDemoColoredResources.bin' to the 'binaries' folder
3. Use the following command to write the resources to the QSPI External flash
	`cli_programmer.exe -i 115200 -s 115200 COMx write_qspi 0x0 WatchDemoColoredResources.bin`
    (Also the gdbserver can be used: Start the Segger GDBserver and use the following command: `cli_programmer.exe gdbserver write_qspi 0x0 WatchDemoColoredResources.bin`)



## Log Messages
The logging and the output of the performance metrics are available in a serial terminal. 
1. Open a serial terminal.
2. Each DA1470x device has two serial ports enabled. The smaller COM port will be used.
3. Set the Speed to 115200.
4. Set the Data to 8bit.
5. Set the Parity to None.
6. Set the Stop Bits to 1bit.
7. Set the Flow Control to None.

## Known Limitations

There are no known limitations for this sample code. 