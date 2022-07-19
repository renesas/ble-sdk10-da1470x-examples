vad keyword detection template demo application {#vad_keyword_detection}
======================================================

## Overview

This application is a presenting voice activity detector under DA1470x device.
For LCD demo : The demo is detecting a special pattern:"Hello Renesas" and then waits user to say "lights on" or "lights off " to display lighting lump
or lighting off depending on the voice command. 
For LCD less demo : The demo is detecting a special pattern:"Hello Renesas" and then waits user to say "lights on" or "lights off ", here commands
are displayed on the serial client, as long as user speaks the D1 led keeps blinking

## Installation procedure

The project is located in the \b `features\vad_demo_lvgl\projects\dk_apps\templates\vad_keyword_detection` folder.

To install the project follow the [General Installation and Debugging Procedure](@ref install_and_debug_procedure).

Requirements
-----------------------------------------
- Dialog Development Board with a DA1470X device.

- For LCD test only: 1.2 inch Round OLED 390X390 Q-SPI 60Hz PCAP Touch Panel(https://www.panoxdisplay.com/amoled/1-2-inch-round-oled-390-spi-60hz.html)

- to run the vad_keyword_detection project with the LCD GUI we need  to write the required resources into the External Flash(step 3 of features/vad_demo_lvgl/projects/dk_apps/templates/vad_keyword_detection/ui/README.md)
   .\cli_programmer.exe -i 115200 COM8 write_qspi 0x0 ..\projects\dk_apps\templates\vad_keyword_detection\ui\demo\resources\bitmaps\lights_on.bin
   .\cli_programmer.exe -i 115200 COM8 write_qspi 0xF574 ..\projects\dk_apps\templates\vad_keyword_detection\ui\demo\resources\bitmaps\lights_off.bin
- UART terminal with the following settings is needed to observe the logs.


| Setting      | Value   |
| :---------:  | :-----: |
| Baud rate    | 921600  |
| Data bits    | 8       |
| Stop bits    | 1       |
| Parity       | None    |
| Flow control | None    |


## Existing build configurations

The template contains build configurations for executing it from RAM or QSPI. 

- `DA1470x-00-Debug_OQSPI`. Applicable for DA1470x-00. Debug build configuration for executing from OQSPI.
- `DA1470x-00-Debug_OQSPI_LCD`. Applicable for DA1470x-00. Debug build configuration for executing from OQSPI with LCD GUI support.
- `DA1470x-00-Release_OQSPI`. Applicable for DA1470x-00. Release build configuration for executing from OQSPI.
- `DA1470x-00-Release_OQSPI_LCD`. Applicable for DA1470x-00. Release build configuration for executing from OQSPI with LCD GUI support.


