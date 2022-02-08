BLE peripheral application {#ble_peripheral}
===============================

## Overview

This application is a sample implementation of device using GAP peripheral
role. It does not implement any particular profile, instead it registers
couple of different services for testing purposes. It can be also used to
execute PTS test cases against services it exposes. These services can be
controller using debug service (for example, 1st client can interact with
services and 2nd client can control them via debug service).

Features:

- All features enabled in application can be controller via config.h file, all
  parameters are described there.

## Installation procedure

The project is located in the \b `projects/dk_apps/features/ble_peripheral` folder.

To install the project follow the [General Installation and Debugging Procedure](@ref install_and_debug_procedure).

## Manual testing

When enabled in config.h, application will register custom debug service
in ATT database with control point (CP) characteristic as follows:
- service UUID is 6b559111-c4df-4660-818e-234f9e17b290
- CP characteristic UUID is 6b559111-c4df-4660-818e-234f9e17b291
- CP characteristic properties are: WRITE, NOTIFY
- CP descriptor: Client Characteristic Configuration
- CP descriptor: Characteristic Presentation Format

Client should enable notifications on CP in order to receive feedback from
application.

Following commands can be written to CP (string values) to interact with
services registered by application:


~~~{.c}
 - bas set <lvl>         set BAS instance battery level to <lvl>`
 - cts adjust <reason>   notify current time with adjust reason <reason>`
                         possible <reason> values:`
                         - manual (manual update)`
                         - tz (timezone)`
                         - dst (daylight saving time)`
                         - reftime (reference time update)`
~~~
