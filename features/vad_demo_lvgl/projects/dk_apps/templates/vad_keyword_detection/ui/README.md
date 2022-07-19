# How to run the vad_keyword_detection project with the LCD GUI.

## Use the LVGL graphics library and the GDI interface.
1. Extract the LVGL-DA1470x-v8.1.0-dev-7.zip into the SDK root directory in order to place lvgl and gdi folders under utilities folder.

## Write the required resources into the External Flash. 
1. Navigate to the binaries folder of the SDK root directory.
2. Use the cli_programmer.exe to write the resources.
3. Open a Windows PowerShell terminal and type:
.\cli_programmer.exe -i 115200 COM8 write_qspi 0x0 ..\projects\dk_apps\templates\vad_keyword_detection\ui\demo\resources\bitmaps\lights_on.bin
.\cli_programmer.exe -i 115200 COM8 write_qspi 0xF574 ..\projects\dk_apps\templates\vad_keyword_detection\ui\demo\resources\bitmaps\lights_off.bin
NOTE: 
* the offset of each image can be found at projects\dk_apps\templates\vad_keyword_detection\ui\demo\resources\resources.c file
* the resource images are stored in the external QSPI Flash. Set the desired location by changing the address of the RESOURCES_BASE_ADDRESS definition.

## Build the GPU driver
1. Import the libd2_driver project.
2. Build the DA1470x-00-Release configuration.

## Build the vad_keyword_detection project 
1. Use the DA1470x-00-Debug_OQSPI_LCD build configuration.

## How to change the displayed image and text of the GUI.
1. Use the lights_on boolean variable to change the displayed image according to the application needs: 
* set the lights_on variable to true, in order to use the lights on image source, and set the lights_on variable to false, in order to use the lights off image source. Do not forget to change the value inside a critical section,
* send a UI_CHANGE_IMG_EVT notification to the gui_task_h handler:

eg. OS_TASK_NOTIFY(gui_task_h, UI_CHANGE_IMG_EVT, OS_NOTIFY_SET_BITS);
    OS_ENTER_CRITICAL_SECTION();
    lights_on = 0;
    OS_LEAVE_CRITICAL_SECTION();
    
2. Use the ui_txt char pointer variable to change the displayed text according to the application needs:
* set the text that needs to be displayed in the LCD. Do not forget to change the value inside a critical section,
* send a UI_CHANGE_TEXT_EVT notification to the gui_task_h handler:

eg. OS_TASK_NOTIFY(gui_task_h, UI_CHANGE_TEXT_EVT, OS_NOTIFY_SET_BITS);
    OS_ENTER_CRITICAL_SECTION();
    ui_txt = "Hello";
    OS_LEAVE_CRITICAL_SECTION();
 