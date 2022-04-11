# Watch Demo Instructions

## Port Watch Demo into GA SDK Release
Extract watch_demo-lvgl.zip into sdk root folder. 
1. lvgl library will be extracted at utilities folder.
2. watch_demo_lvgl project will be extracted at projects/dk_apps/demos folder.
3. WatchDemoResources.bin file, containing the required resources for the watch demo, will be extracted at binaries folder.
   

## Write Watch Demo resources' binaries to QSPI Flash
Use SEGGER J-Link GDB Server and cli_programmer.exe to write the resources binary into the QSPI Flash. 
1. Open the SEGGER J-Link GDB Server.
2. Select USB as Connection to J-Link.
3. Select DA1470x as Target Device and Little Endian support.
4. The Target interface is SWD with Auto speed or Fixed at 4000 kHz.
5. Press OK to connect.
6. Select the J-Link emulator with the smaller number.  
7. Open a Windows PowerShell and navigate into binaries folder of the root SDK folder.
8. Type the following command to write the resources to flash
	`.\cli_programmer.exe gdbserver write_oqspi 0x00100000 .\WatchDemoResources.bin`

## Build Configurations
1. DA1470x-00-Debug_OQSPI_demo build configuration. The user can interact with the watch demo application by touching the LCD screen.
2. DA1470x-00-Debug_OQSPI_touch_simulation build configuration is used to get performance metrics for the FPS, the GPU rendering time and the CPU utilization. The touch simulation is used to be able to switch between the current screens and measure the demo perfomance.

## Log Messages
The logging and the output of the performance metrics are available in a serial terminal. 
1. Open a serial terminal.
2. Each DA1470x device has two serial ports enabled. The smaller COM port will be used.
3. Set the Speed to 115200.
4. Set the Data to 8bit.
5. Set the Parity to None.
6. Set the Stop Bits to 1bit.
7. Set the Flow Control to None.

## Definitions
### SDK 

| FILE  | DEFINITION  | DEFAULT VALUE  | VALUES  | NOTES  |
| ------------ | ------------ | ------------ | ------------ | ------------ |
| config/custom_config_oqspi.h  | AD_LCDC_DEFAULT_CLK  | sysclk_RCHS_96 | sysclk_RCHS_96 or sysclk_PLL160  | Sets the system clock. _sysclk_RCHS_96_ sets the system clock at 96 MHz. _sysclk_PLL160_ Sets the system clock at 160 MHz. |
| ui/gdi/include/e120a390qsr.h  | e120a390qsr_drv.hw_init.write_freq  | LCDC_FREQ_48MHz | LCDC_FREQ_48MHz or LCDC_FREQ_40MHz | Sets the LCD interface frequency. _LCDC_FREQ_48MHz_ should be used when system clock is running at 96MHz. _LCDC_FREQ_40MHz_ should be used when system clock is running at 160MHz. |

### DEMO

| FILE  | DEFINITION  | DEFAULT VALUE  | VALUES  | NOTES  |
| ------------ | ------------ | ------------ | ------------ | ------------ |
| Project file | PERFORMANCE_METRICS |  |  | Enables performance metrics. The measurements are stored during the demo execution and are printed when simulation finishes. |
| config/custom_config_oqspi.h | dg_configUSE_TOUCH_SIMULATION | 1 | 1 or 0 | Enables/Disables the touch simulation support. |
| ui/demo/demo.h | FB_COLOR_FORMAT | CF_NATIVE_RGB565 | CF_NATIVE_RGB565 | Sets the frame buffer color format. 16bit color depth is used for the watch demo. CF_NATIVE_RGB565 is the only supported. |
| ui/demo/demo.h | GDI_FB_USE_QSPI_RAM | 0 | 0 or 1 | Set the location of the frame buffers. They can be either reside at SRAM or in PSRAM. _0_: The frame Buffers are in SRAM. _1_: The frame Buffers are in PSRAM. |
| ui/demo/demo.h | SCENARIO | OPTIMAL | OPTIMAL or NO_GPU | _OPTIMAL_: The watch demo is built will performance optimizations enabled. _NO_GPU_: The watch demo is running without GPU acceleration for fill, blit or rotation operations. Only OPTIMAL scenario can be used for now.|
| ui/demo/demo.h | COMPASS_ROTATION_USES_CANVAS | 0 | 0 or 1 | Optimization for the compass disk rotation. _0_: Rotation is performed in Flash where the compass disk is stored. _1_: A canvas is created on SRAM, the compass disk is copied from FLASH to SRAM where the rotation will be finally performed. |
| ui/demo/demo.h | TWO_LAYERS_HORIZONTAL_SLIDING | 1 | 1 or 0 | _1_: Optimization for the horizontal sliding. The 2 Layers of the LCD are used to perform the sliding. The frame buffers are redrawn with the 2 main screens, watch and menu, the refresh timer is paused and the positions of the 2 Layers change according to the received scrolling distance. _0_: LVGL performs the horizontal sliding.|
| ui/demo/demo.h | DEMO_GUI_HEAP_SIZE | 15 * 1024 | (15 * 1024) or (320 * 1024) | The heap that is required for the LVGL library, stored in SRAM. _320 * 1024_: When the COMPASS_ROTATION_USES_CANVAS definition is enabled a new buffer is used in SRAM and the heap size is increased according to the compass disk size. (390 * 390 * 2 bytes) | 


### LVGL
| FILE | DEFINITION | DEFAULT VALUE | VALUES | NOTES |
| ------------ | ------------ | ------------ | ------------ | ------------ |
| Project file | _DEBUG | | | Enables the log module. This definition should be enabled only in development mode, since the performance of the application is reduced. |
| ui/lvgl/config/lvgl_conf.h | LV_COLOR_CHROMA_KEY | lv_color_hex(0x00ff00) | lv_color_hex(0x00ff00) | Images pixels with this color will not be drawn if they are  chroma keyed. Pure green is used as chroma key. | 
| ui/lvgl/config/lvgl_conf.h | LV_DISP_DEF_REFR_PERIOD | 15 | 15 or 30 | Default display refresh period. LVG will redraw changed areas with this period time (in msec)
| ui/lvgl/config/lvgl_conf.h | LV_INDEV_DEF_READ_PERIOD | 15 | 15 or 30 | Input device read period in milliseconds. |







