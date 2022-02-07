I2C EEPROM sample program
======================================================

## Overview

This application is based on the implementation of FreeRTOS Retarget template application. It's intended to show writing and reading to/from an EEPROM.

The EEPROM used is a standard Click Board with a 1kx8 EEPROM BR24G08.
This particular EEPROM can either have device address 0x50 or 0x54, depending on address pin A2. (Default is 0x50).
Address pins A1 and A0 are not used. 

(Software) Device Addressing
(1) Slave address comes after start condition from master.
(2) The significant 4 bits of slave address are used for recognizing a device type.
The device code of this IC is fixed to '1010'.
(3) Next slave addresses (A2 --- device address) are for selecting devices, and multiple devices can be used on a same bus according to the number of device addresses.
(4) The most insignificant bit ( W/R --- READ / WRITE ) of slave address is used for designating write or read operation, and is as shown below.
Setting W/R to 0 ------- write (setting 0 to word address setting of random read)
Setting W/R to 1 ------- read

| 1 | 0 | 1 | 0 | A2 | P1 | P0 | R/W |
-|

> Note: P0 and P1 are page select bits.

Because of this mechanism, this particular EEPROM looks like 4 addressable EEPROMs of 256 bytes each with 4 different addresses : 0x50, 0x51, 0x52 and 0x53

## Installation procedure

The project is delivered as a small software example.

To install the project follow the [General Installation and Debugging Procedure](@ref install_and_debug_procedure).

## File structure

The following file structure will be created:

* projects/dk_apps/templates/freertos_retarget
        * config
                * custom_config_oqspi.h
                * custom_config_ram.h
        * sdk
        * startup
        * main.c

## Existing build configurations

The template contains build configurations for executing it from RAM or (O)QSPI. 

- `DA1470X-00-Debug_RAM`. The project is built to be run from RAM. The executable is built with debug (-Og) information.
- `DA1470X-00-Debug_OQSPI`. The project is built to be run from (O)QSPI. The executable is built with debug (-Og) information.
- `DA1470X-00-Release_RAM`. The project is built to be run from RAM. The executable is built with no debug information and size optimization (-Os).
- `DA1470X-00-Release_OQSPI`. The project is built to be run from (O)QSPI. The executable is built with no debug information and size optimization (-Os).

## RTT ENABLE

The template gives the user the option to redirect the output either to RTT or Retarget by defining the CONFIG_RETARGET pre-processor macro.

> Note: The template uses the retarget option by default.
> Note2: By default asynchronous access is used

