# I2C Adapter Demonstration Example In SYSCPU Context

## Example Description

This example demonstrates using the I2C adapter layer employing the I2C1 block in master mode. The DA1470x family of devices comprises three separate I2C blocks instances. In this example, all of the I2C operations are executed in the SYSCPU context using the FreeRTOS preemptive scheduling. To make life easy, the I2C- and microBUS-compatible [`THERMO3` click board](https://www.mikroe.com/thermo-3-click) is utilized. The board comes with the `TMP102` temperature sensor which can trigger an external signal once the temperature exceeds or falls within a configurable temperature range. In this demonstration, the sensor is configured in comparator mode in which the alert pin is asserted once the temperature equals or exceeds the T<sub>high</sub> threshold and for as long as the temperature falls below the T<sub>low</sub> threshold. The alert signal is connected to the WKUP controller which is triggered once the signal is asserted. A single task, namely `termo3_task`,  is created and is responsible for configurating the sensor and reading the temperature for as long as the alert signal is asserted. The sample code comes with full drivers of the TMP102 temperature sensor which can be found in the [tmp102_drv/header/tmp102_reg.h](tmp102_drv/header/tmp102_reg.h) file.

## HW and SW configuration

### Hardware configuration

- This example runs on DA1470x Bluetooth Smart SoC devices. One DA1470x Pro Development kit and one THERMO3 click board are needed for this example. For more information on the temperature sensor demonstrated click [here]( https://www.mikroe.com/thermo-3-click).
- Connect the development kit to the host computer via the USB1 connector mounted on the motherboard and open a serial terminal. Two serial ports should be displayed; select the one with the lower number and then configure the terminal as depicted in the following table:

|  Setting  | Value  |
| :-------: | :----: |
| Baudrate  | 115200 |
| Data bits |   8    |
| Stop bits |   1    |
|  Parity   |  None  |

- Connect the click board to mikroBUS #2. It's a prerequisite that the expansion headers have been soldered before attaching the click board. Alternatively, the user can occupy mikroBUS #1. If this is the case, the alert pin should be changed. To do so, go to the `config/custom_config_osqpsi.h` or `config/custom_config_ram.h` depending on the selected build configuration and defien the following statement:

  ```c
  #define MIKRO_BUS           ( 1 )
  ```

### Software configuration

- Recommended to employ the latest SmartSnippets Studio version released.
- Download the latest SDK version
- SEGGER J-Link tools should be downloaded and installed.

## How to run the example

**The RAM build configurations should be used for debugging purposes only.**

### Initial Setup

- Download the source code from the Support Website.

- Import the `i2c_master_m33_sample_code` project into your workspace (there are no path dependencies).

- Define the temperature boundaries that force the alert pin to be asserted. In this example, the alert range defined is [25, 26]. This means that the alert signal will be asserted once the temperature equals or exceeds the 26 degrees of Celsius and for as long the temperature falls bellow the 25 degrees of Celsius. The temperature value should be displayed every one second and for as long as the alert signal is asserted. To change the alert boundaries go to the `config/app_common.h` file and change the corresponding configuration macros, that is:

  ```c
  /* The high temperature value (in Celsius) that defines the boundaries of the alter signaling. */
  #define THERMO3_ALERT_T_HIGH       ( 26 )
  /* The low temperature value (in Celsius) that defines the boundaries of the alter signaling. */
  #define THERMO3_ALERT_T_LOW        ( 25 )
  ```

- Build the project by selecting either the debug or release OQSPI build configuration.

- Download the firmware image into the XiP (eXecution-in Place) flash memory used by selecting the `program_oqspi_jtag` or  `program_oqspi_serial` launcher. If more actions are required follow the instructions given in the Console window of the Eclipse environment.

  - The project is compiled with the flash memory autodetect feature enabled meaning that any supported OQSPI flash memory can be used without modifying the `custom_config_oqspi.h` file, that is:

    ```c
    #define dg_configFLASH_AUTODETECT               ( 1 )
    #define dg_configOQSPI_FLASH_AUTODETECT         ( 1 )
    ```

- Press the `B1(RSTn)` push button mounted on the daughterboard, so the device starts executing its firmware.

- The current temperature value should be displayed as long as the alert signal is asserted. For istance:

```bash
  > Temperature: +26.0
  >
  > Temperature: +26.1
  >
  > Temperature: +25.6
  >
  > Temperature: +25.2
  >
  > Temperature: +25.0
```

## Known Limitations

There are no known limitations for this example.
