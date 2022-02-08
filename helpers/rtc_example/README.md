RTC example demo application {#rtc_example}
======================================================

## Overview

This application is an example implementation of an RTC application. The main.c creates a template task 
which implements the following tests:

- Trigger time roll over interrupt
Initializes RTC, sets time to 11:59:59:98 and date (not important).
Registers interrupt handler callback function and enables hundredth of second, second, minute and hour roll over interrupts.
In 20 milliseconds the interrupt is triggered. Checks in the callback function that the following events are set:
  HW_RTC_EVENT_HOUR
  HW_RTC_EVENT_MIN
  HW_RTC_EVENT_SEC
  HW_RTC_EVENT_HSEC 
Prints over UART the RTC time and date and the event mask.

- Trigger date roll over interrupt
Initializes RTC, sets format to 24 hour clock format and sets time to 23:59:59:00 and date to 31st of January.
Registers interrupt handler callback function and enables day of the month and month roll over interrupts.
In one second the interrupt is triggered. Checks in the interrupt handler callback function that the following events are set:
  HW_RTC_EVENT_MDAY
  HW_RTC_EVENT_MONTH
Prints over UART the RTC time and date and the event mask.

- Trigger time alarm interrupt
Initializes RTC, sets time to 11:59:59:00 and date (not important).
Sets alarm time to 12:00:00:00 and enable hundredth of second, second, minute and hour alarm interrupts
Registers alarm interrupt handler callback function.
In 1 seconds the callback function is called and the event HW_RTC_EVENT_ALRM is set.
Prints over UART the RTC time and date and the event mask.

- Trigger date alarm interrupt
Initializes RTC, sets format to 24 hour clock format and sets time to 23:59:59:00 and date to 31st of January.
Sets alarm date to 1st of February and enables day of the month and month alarm interrupts.
Registers alarm interrupt handler callback function.
In 1 seconds the callback function is called and the event HW_RTC_EVENT_ALRM is set.
Prints over UART the RTC time and date and the event mask.

- Event wake up (PDC)
Initializes RTC.
Enables RTC_EVENT interrupt.
Sets PDC event period to 2 secs.
Enables RTC PDC event counter.
Creates PDC entry to wake up M33 from HW_PDC_PERIPH_TRIG_ID_RTC_TIMER.
Task is blocked and waits for any event. M33 enters extended sleep mode.
M33 wakes up from RTC timer periodic event and prints the time.

- Alarm wake up (PDC)
Initializes RTC.
Sets RTC alarm to fire every second.
Enables RTC alarm interrupt.
Creates PDC entry to wake up M33 from HW_PDC_PERIPH_TRIG_ID_RTC_ALARM.
Set M33 to sleep. RCX is set as the LP CLK
Task is blocked and waits for any event. M33 enters extended sleep mode.
M33 wakes up from RTC alarm and prints the time.

- Keep registers after reset 
User will have to press reset button of the DUT and observe the RTC clock and calender values printed on UART after boot.


## Installation procedure

The project is located in the `projects/dk_apps/demos/rtc_example` folder.

To install the project follow the [General Installation and Debugging Procedure](@ref install_and_debug_procedure).

## File structure

The following file structure will be created:

* projects/dk_apps/demos/rtc_example
        * config
                * custom_config_qspi.h
                * custom_config_ram.h
        * sdk
        * startup
        * main.c

## Existing build configurations

The project contains build configurations for executing it from RAM or QSPI. 

## RTT ENABLE

The template gives the user the option to redirect the output either to RTT or Retarget by defining the CONFIG_RETARGET pre-processor macro.

> Note: The template uses the retarget option by default.


