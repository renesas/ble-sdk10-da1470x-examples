Description
-----------------------------------------
Using project `pdc_demo` following feature can be tested :

- How to configure a PDC entry, which will trigger PDC to wakeup the system and to take specific actions after waking up.

In this demo the following PDC triggers are implemented :

1.  Create a PDC entry using HW_TIMER5 as peripheral trigger. PDC will wakeup the system when HW_TIMER5 expires, every 1 second.
    It will also enable XTAL32M after waking up.
  
2.  Create a PDC entry using HW_TIMER6 as peripheral trigger. PDC will wakeup the system when HW_TIMER6 expires, every 510 milliseconds.
    It will also enable PD_SNC after waking up.
  
3.  Create a PDC entry using GPIO Port 1 as peripheral trigger. That means that if any pin of Port1 changes, a PDC trigger will wakeup the system.
    In the pdc_demo Button1 (P1_22) and Button2 (P1_23) are connected on GPIO Port1. When one of these Buttons is pressed, PDC will wakeup the system.    
  
    After each wakeup event there is also a verification mechanism, which checks if the system is waking up from the configured source, 
    and	if it has enabled the correct power domains and clocks upon wakeup.


Supported build configurations
-----------------------------------------
The `pdc_demo` project contains build configurations for executing from RAM or Octa/Quad-SPI flash on DA1470x.

- `DA1470x-00-Debug_QSPI`
- `DA1470x-00-Debug_RAM`

This project sets the system in extended sleep mode. In case RAM builds are used, JTAG will prevent the system to 
enter sleep mode. So it is suggested to use OQSPI builds in order to be able to observe the wakeup mechanism through PDC. 

Requirements
-----------------------------------------
- Dialog DA1470x Development Board.

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
A Saleae Logic Analyzer can be used in order to monitor sleep mode, the button presses, HW_TIMER5 and HW_TIMER6.
The following GPIOs are used in `pdc_demo` project :

| GPIO port | Connector on silicon  | Functionality                                                   |
|-----------|-----------------------|-----------------------------------------------------------------|
| P0_17     | J3-P0_17 pin          | Monitors when the system enters sleep mode.                     |
| P0_18     | J3-P0_18 pin          | Monitors when the system exits from sleep mode.                 |
| P0_21     | J3-P0_21 pin          | Toggle this GPIO when HW_TIMER5 expires.                        |
| P0_23     | J3-P0_23 pin          | Toggle this GPIO when HW_TIMER6 expires.                        |
| P0_22     | J3-P0_22 pin          | Toggle this GPIO either when Button1 or when Button2 is pressed.|
| P1_22     | J4-P1_22 pin          | Button which will trigger a wakeup event, when it is pressed.   |
| P1_23     | J4-P1_23 pin          | Button which will trigger a wakeup event, when it is pressed.   |

Run the `pdc_demo` project
-------------------------------------
Build the project using OQSPI build and burn it to O/QSPI flash.
When program starts to execute, the following can be monitored on a logic analyzer :


- At P0_17 and P0_18 sleep enter and sleep exit sequence. These GPIOs are enabled by defining PWR_MGR_USE_TIMING_DEBUG to 1 in configuration file.

- Every second there is a pulse on P0_21, which is the expiration of HW_TIMER5. At the same time at P0_18 there is a toggling, that indicates 
  that HW_TIMER5 has woken up the system.

- Every 510 milliseconds there is a pulse on P0_23, which is the expiration of HW_TIMER6. At the same time at P0_18 there is a toggling, that indicates 
  that HW_TIMER6 has woken up the system.

- On P0_22 a toggling occurs when Button1 or Button2 is pressed. At the same time at P0_18 there is a toggling that indicates, that one of the 
  Buttons has woken up the system.   

- On the console there is information about the wakeup source and if the configured peripherals switched on.







