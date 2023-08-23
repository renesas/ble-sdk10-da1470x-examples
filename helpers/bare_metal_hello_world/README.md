# Bare Metal Hello World Example

## Example Description

This demonstration example is a simple implementation of a bare metal retarget application. 

## HW and SW configuration

### Hardware configuration

- This example runs on DA1470x Bluetooth Smart SoC devices. 
- Connect the development kit to the host computer via the USB1 connector mounted on the motherboard. Then, open a serial terminal of your choice. Two serial ports should be displayed; select the one with the lower number and configure the terminal with the following settings:

|  Setting  | Value  |
| :-------: | :----: |
| Baud rate | 115200 |
| Data bits |   8    |
| Stop bits |   1    |
|  Parity   |  None  |

### Software configuration

- Recommended to [download](https://www.renesas.com/eu/en/software-tool/smartbond-development-tools) the latest Smart Snippets Studio (SSS) version. 
- [Download](https://www.renesas.com/us/en/document/sws/da1470x-sdk-102649) the latest SDK version.
- SEGGER J-Link tools should be downloaded and installed.

## How to run the example

**The RAM build configurations should be used for debugging purposes only.**

### Initial Setup

- [Download](https://github.com/dialog-semiconductor) the demonstration example. 

- Import the `hello_world_no_os` sample code into your workspace (there should be no path dependencies).

- Build the project by selecting either a OQSPI or RAM build configuration.

- Download the firmware image into the device. For more info refer to the [getting started](https://lpccs-docs.renesas.com/um-b-153-da1470x_getting_started/9-First_DA1470x_application/First_DA1470x_application.html) document, section 9.3

- Press the `B1(RSTn)` push button mounted on the daughterboard, so the device starts executing its firmware.

- `Hello world!` should be displayed every one second.

```bash
  > Hellow World!
  >
  > Hellow World!
  >
  > Hellow World!
  >
```

## Known Limitations

There are no known limitations for this example.