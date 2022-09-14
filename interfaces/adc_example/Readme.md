# Example demonstrating the GPADC adapter

## Overview

This application demonstrates the use of the ADC adapter using the General Purpose ADC (GPADC). The GPADC is set to operate in single-ended, non-continuous mode and measure the output voltage of a voltage divider. The latter is built using the regulated voltage of pin V18P on the J5 socket (V = 1.8V) and a 100 kOhm potentiometer.

## HW and SW configuration

- **Hardware configuration**

  - This example runs on the DA14701, DA14705 and DA14706 Bluetooth Smart SoC devices.
  - The DA1470x Pro Development Kit is needed for this example.
  - Connect the Development Kit to the host computer.
  - Connect pins V18P and GND of socket J5 to the end pins of the potentiometer
  - Connect pin P005 of socket J3 to the middle pin of the potentiometer
  - Connect pin P006 of socket J3 to GND

- **Software configuration**

  - This example requires:
  - Smartsnippets Studio 2.0.18 or higher
  - SDK10.2.44.x
  - **SEGGER J-Link** tools should be downloaded and installed.

## How to run the example

- Import this project into the workspace
- Compile and launch RAM or OQSPI target
- Run from RAM or load in the flash
- Open a serial terminal with settings 115200/8-N-1
- Press key K1 on the Development Kit to trigger a new ADC measurement
- The ADC reading and the voltage measurement in mV are displayed on the terminal
