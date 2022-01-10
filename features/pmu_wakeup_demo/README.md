Description
-----------------------------------------
Using project `pmu_wakeup_demo` following features can be tested :

- Set the system in hibernation pressing a Button and wakeup from hibernation plugging the USB cable on daughterboard.
- By pressing button K1, the app switches to all available clocks configuring fast or normal wakeup mode. Each button press is a different mode.
  Also on each button press GPADC measure the voltage of rails: V30,Vsys,V18,V18F,V18P,V14 and V12. On the console output you can see the configuration 
  and the GPADC measurements.
- Switch between ultra-fast and fast wakeup mode and measure the time that is needed for waking up in each mode.
- Switch on/off a LED every 1 sec. Also a GPIO is switched on/off every 1 second.
- How to configure buttons.
- How to use PMU adapter in order to configure power rails at initialization, when wakeup mode is changed from fast to ultra-fast and vice versa and during switch on and off a LED.


Supported build configurations
-----------------------------------------
The `pmu_wakeup_demo` project contains build configurations for executing from RAM or Octa/Quad-SPI on DA1470x.

- `DA1470x-00-Debug_QSPI`
- `DA1470x-00-Debug_RAM`

This project sets the system in extended sleep or hibernation mode. In case RAM builds are used, JTAG will prevent the system to 
enter sleep mode. So it is suggested to use OQSPI builds. In hibernation mode, RAM cells are not retained, thus when the system is waking up, 
a hardware reset is performed, which results to execute the test from Octal/Quad-SPI flash.

Requirements
-----------------------------------------
- Dialog Development Board with a DA1470X device.

- UART terminal with the following settings is needed to observe the logs.

| Setting      | Value   |
| :---------:  | :-----: |
| Baud rate    | 115200  |
| Data bits    | 8       |
| Stop bits    | 1       |
| Parity       | None    |
| Flow control | None    |


Configuration
-----------------------------------------
A Logic Analyzer can be used in order to monitor sleep and wake-up states, the button presses, hibernation mode.
The following GPIOs are used in `pmu_wakeup_demo` project :

|GPIO port		|Connector on silicon		|Functionality
|---------		|---------------------		|--------------
|P0_17			|J3-P017 pin				|Monitors when the system enters sleep mode.
|P0_18			|J3-P018 pin				|Monitors when the system exits from sleep mode.
|P0_23			|J3-P023 pin				|Toggle this GPIO when Button1 is pressed and system wakes up with fast wkup mode.
|P1_0			|J4-P100 pin				|Toggle this GPIO when Button1 is pressed and system wakes up with ultra-fast wkup mode.
|P1_22			|J4-P122 pin				|Button which switches between fast and ultra-fast wkup mode.

For Hibernation mode on silicon:
Remove jumper (pin3-pin4) from J35. Use a cable from J35:4 to P029 pin of J3 header. This is because specific pins
are able to wake up the system from hibernation mode. (More information can be found in the datasheet). Press Button K2 to get the system out of hibernation mode.

Run the `pmu_wakeup_demo` project
-------------------------------------
Build the project using OQSPI build and burn it to OQSPI flash.
When program starts to execute, the following can be monitored with a logic analyzer :

- At P0_17 and P0_18 sleep enter and sleep exit sequence. These GPIOs are enabled by defining PWR_MGR_USE_TIMING_DEBUG to 1 in configuration file.

- Every second there is a pulse on P0_22, which simulates the LED switch on/off functionality.
  Also the Led on the development kit is switching on and off.

- While K2 button is pressed (hibernation mode), P0_18 (SLEEP_EXIT) goes high, P0_17(SLEEP_ENTER) goes low. To exit from hibernation
  mode plug a USB cable on daughterboard.

- When Button1 is pressed, the program toggles P0_23 at the first press, toggles P1_0 at the second press etc, to switch between one wakeup mode to another.

- Use a probe on Button1 to monitor the press. Using the markers, the time from Button1 press until P0_18 goes high (SLEEP_EXIT) is the wakeup time
  in fast and ulta-fast wakeup mode. 

- The current voltage of each power rail will be printed on the console every second.







