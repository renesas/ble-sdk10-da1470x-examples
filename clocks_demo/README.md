Description
-----------------------------------------
Using project `clocks_demo` the target is to demonstrate the clock switching between RCHS@32Mhz and PLL or between RCHS@32Mhz and RCHS@96Mhz.

In the demo the clock switching is requested from the three application tasks below:

1. One task is triggered by a timer (HW_TIMER3) with frequency 1Hz. It starts with RCHS@32Mhz clock and after a while it requests for PLL or switch to RCHS@96Mhz.

2. Second task is triggered by a timer (HW_TIMER4) with frequency 11Hz. It starts also with RCHS@32Mhz clock and after a while it requests for PLL or switch to RCHS@96Mhz.

3. Third task is triggered asynchronously when Button1 (S3) is pressed. It requests also for PLL or switch to RCHS@96Mhz.

Each task checks that the system clock runs at the desired frequency.

On OQSPI builds, the system enters sleep mode. Therefore a PDC entry is created for each timer to wakeup the system at its expiration.


Supported build configurations
-----------------------------------------
The `clocks_demo` project contains build configurations for executing from RAM or Octa-QSPI on DA1470x.

- `DA1470x-00-Debug_QSPI`
- `DA1470x-00-Debug_RAM`

This project sets the system in extended sleep. In case RAM builds are used, JTAG will prevent the system to 
enter sleep mode. So it is suggested to use OQSPI builds to see also the clock switch behavior when the system is in extended sleep mode.
 
Requirements
-----------------------------------------
- Dialog DA1470x Development Board
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
A Saleae Logic Analyzer is needed in order to monitor sleep and wakeup states, the button presses, hibernation mode.
The following GPIOs are used in `clocks_demo` project :

GPIO port		Connector(on FPGA)			Connector on silicon	Functionality
---------		-------------------			--------------------	--------------
P0_17			(J28:11)   			        J3-P017 pin			 	Monitors when the system enters sleep mode.
P0_18			(J28:17)					J3-P018 pin				Monitors when the system exits from sleep mode.
P0_24			(J28:15)					J3-P024 pin				Monitors when PLL is used.
P0_22			(J28:18)					J3-P022 pin				Monitors when PLL/RCHS@96MHz is requested by HW_TIMER3 ( referred as timer1 in the demo).
P0_21			(J28:14)					J3-P021 pin				Monitors when PLL/RCHS@96MHz is requested by HW_TIMER4 ( referred as timer2 in the demo).
P0_23			(J28:12)					J3-P023 pin				Monitors when PLL/RCHS@96MHz is requested by Button1 pressing.
P1_22           Button1(S3)					J4-P122 pin				Button which requests clock switching.


Run the `clocks_demo` project
-------------------------------------
Build the project using OQSPI build and burn it to OQSPI flash, or using RAM build to execute it from Debugger.
When program starts to execute, the following is monitored on Saleae logic analyzer :

- At P0_17 and P0_18 sleep enter and sleep exit sequence. These GPIOs are enabled by defining PWR_MGR_USE_TIMING_DEBUG to 1 in configuration file.

- P0_24 is HIGH as long as PLL is requested by any source. 

- P0_22 and P0_21 goes HIGH when PLL/RCHS@96MHz is requested by HW_TIMER3 and HW_TIMER4 respectively.

- P0_23 goes HIGH when PLL/RCHS@96MHz is requested after pressing Button1.

On the console it is shown, if clock switching is successful or not and from which task.







