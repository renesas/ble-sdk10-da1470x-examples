/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief FreeRTOS template application with retarget
 *
 * Copyright (C) 2015-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "osal.h"
#include "resmgmt.h"
#include "hw_cpm.h"
#include "hw_gpio.h"
#include "hw_watchdog.h"
#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"

#include "hw_pdc.h"
#include "ad_i2c.h"
#include "hw_wkup.h"
#include "hw_sys.h"
#include "peripheral_setup.h"
#include "platform_devices.h"

/* Task priorities */
#define  mainI2C_TASK_PRIORITY                  ( OS_TASK_PRIORITY_NORMAL )

/* The rate at which data is template task counter is incremented. */
#define mainCOUNTER_FREQUENCY_MS                ( OS_MS_2_TICKS(200) )

/* Retained symbols */
__RETAINED static OS_EVENT signal_i2c_eeprom;
__RETAINED static OS_EVENT signal_i2c_eeprom_async;

/*
 * Error code returned after an I2C operation. It can be used
 * to identify the reason of a failure.
 */
__RETAINED static HW_I2C_ABORT_SOURCE I2C_error_code;

/*
 * Perform any application specific hardware configuration.  The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware( void );
/*
 * Task functions .
 */
static OS_TASK_FUNCTION(prvI2CTask_EEPROM, pvParameters);      //static void prvI2CTask_EEPROM(void *pvParameters);

/* Task handle */
__RETAINED static OS_TASK prvI2CTask_h;
static OS_TASK xHandle;

uint32_t pdc_wkup_combo_id  __attribute__((unused));

static OS_TASK_FUNCTION(system_init, pvParameters)        //static void system_init(void *pvParameters)
{
        //OS_BASE_TYPE status;

        REG_SETF(GPREG, DEBUG_REG, SYS_CPU_FREEZE_EN, 0);

#if defined CONFIG_RETARGET
        extern void retarget_init(void);
#endif /* CONFIG_RETARGET */

        /*
         * Prepare clocks. Note: cm_cpu_clk_set() and cm_sys_clk_set() can be called only
         * from a task since they will suspend the task until the XTAL32M has settled and,
         * maybe, the PLL is locked.
         */
        cm_sys_clk_init(sysclk_XTAL32M);
        cm_apb_set_clock_divider(apb_div1);
        cm_ahb_set_clock_divider(ahb_div1);
        cm_lp_clk_init();

        /* Prepare the hardware to run this demo */
        prvSetupHardware();

#if defined CONFIG_RETARGET
        retarget_init();
#endif /* CONFIG_RETARGET */


        OS_EVENT_CREATE(signal_i2c_eeprom);
        OS_EVENT_CREATE(signal_i2c_eeprom_async);

        /*
         * Upon a wakeup cycle, wait for the XTAL32M crystal to settle.
         * BLE, USB and UART blocks require the XTAL32M to be up and
         * running to work properly.
         */
        pm_set_wakeup_mode(true);


        /* Set the desired sleep mode. */
        pm_sleep_mode_set(pm_mode_active);

        /*
         * Set the desired wakeup mode.
         *
         * \warning When set is Ultra-Fast wakeup mode, sleep voltage should be 0.9V
         *          and not less than that.
         *
         **/
        pm_set_sys_wakeup_mode(pm_sys_wakeup_mode_normal);

        /* I2C task  */
        OS_TASK_CREATE("I2C",    /* The text name assigned to the task, for
                                             debug only; not used by the kernel. */
                        prvI2CTask_EEPROM, /* The function that implements the task. */
                        NULL,              /* The parameter passed to the task. */
                        1024 * OS_STACK_WORD_SIZE, /* Stack size allocated for the task
                                                      in bytes. */
                        mainI2C_TASK_PRIORITY,   /* The priority assigned to the task. */
                        prvI2CTask_h );             /* The task handle. */
        OS_ASSERT(prvI2CTask_h);

        /* The work of the SysInit task is done */
        OS_TASK_DELETE(xHandle);
}

#define EEPROM_MAX_WRITE_CYCLE_DURATION_MS   5

/* Macro used after an I2C write operation */
#define MEM_24C08_write_cycle_delay()  OS_DELAY_MS(EEPROM_MAX_WRITE_CYCLE_DURATION_MS+1)

/* Enable/disable asynchronous I2C operations */
#define I2C_ASYNC_EN                (0)

/* EEPROM device-specific parameters */
#define EEPROM_TEST_DATA_SIZE       (128)
#define EEPROM_24C08_PAGE_SIZE    (16)

/* The source test buffer that contains the Lorem Ipsum text */
const uint8_t e_src_buff[EEPROM_TEST_DATA_SIZE] = {
        'L', 'o', 'r', 'e', 'm', ' ', 'i', 'p', 's', 'u', 'm', ' ', 'd', 'o', 'l', 'o',
        'r', ' ', 's', 'i', 't', ' ', 'a', 'm', 'e', 't', ',', ' ', 'c', 'o', 'n', 's',
        'e', 'c', 't', 'e', 't', 'u', 'r', ' ', 'a', 'd', 'i', 'p', 'i', 's', 'c', 'i',
        'n', 'g', ' ', 'e', 'l', 'i', 't', '.', ' ', 'D', 'o', 'n', 'e', 'c', ' ', 'd',
        'i', 'g', 'n', 'i', 's', 's', 'i', 'm', ' ', 'l', 'a', 'c', 'u', 's', ' ', 'a',
        ' ', 'p', 'o', 's', 'u', 'e', 'r', 'e', ' ', 'm', 'a', 'x', 'i', 'm', 'u', 's',
        '.', ' ', 'A', 'l', 'i', 'q', 'u', 'a', 'm', ' ', 'e', 'r', 'a', 't', ' ', 'v',
        'o', 'l', 'u', 't', 'p', 'a', 't', '.', ' ', 'P', 'r', 'o', 'i', 'n', '.', ' ',
};



/**
 * @brief Prints an EEPROM page
 *
 * \param [in] pageBuff A pointer to the buffer which contains the page data
 * \param [in] len      The data length
 *
 */
static void printout_eeprom_page(uint8_t *pageBuff, uint16_t len)
{
        uint16_t i;
        for (i = 0; i < len; i++) {
                //if (!(i % 8)) printf("\r\n");
                printf("%c", pageBuff[i]);
        }
        printf("\r\n");
}


/* Perform an I2C write operation */
static void eeprom_data_writer(i2c_device dev, uint16_t pageAddr)
{
     /* The provided address must be multiple of page size */
     ASSERT_WARNING(((pageAddr % EEPROM_24C08_PAGE_SIZE) == 0));

     uint8_t e_data_buff[EEPROM_24C08_PAGE_SIZE + 1];
     uint8_t *pSrc;

     /* Open the device */
     ad_i2c_handle_t dev_hdr = ad_i2c_open((ad_i2c_controller_conf_t *)dev);


     /* Write arbitrary data in EEPROM */
     for (int i = 0; i < (EEPROM_TEST_DATA_SIZE / EEPROM_24C08_PAGE_SIZE); i++) {

             /* The first two bytes sent are the target slave address */
             //e_data_buff[0] = pageAddr >> 8;
             e_data_buff[0] = pageAddr;

             /* Point to the right next EEPROM page */
             pSrc = (uint8_t *)e_src_buff + pageAddr;

             /* Prepare the data to be sent */
             memcpy((void *)&e_data_buff[1], (void *)pSrc, EEPROM_24C08_PAGE_SIZE);


             /* Write one page size in EEPROM */
             I2C_error_code = ad_i2c_write(dev_hdr, (const uint8_t *)e_data_buff,
                                             sizeof(e_data_buff), HW_I2C_F_ADD_STOP);


             /* Print on the serial console the status of the I2C operation */
             if (I2C_error_code == 0) {
                    /* Wait until EEPROM performs its actual write cycle */
                    MEM_24C08_write_cycle_delay();

                    /* Move forward to the next EEPROM page */
                    pageAddr += EEPROM_24C08_PAGE_SIZE;
             } else {
                    printf("\n\rUnsuccessful I2C write operation with error code: %d\n\r",
                                                                          I2C_error_code);
                    break; /* Exit from loop */
             }

     }

     /* Close the device */
     ad_i2c_close(dev_hdr, false);
     //printf("\n\rEEPROM succesfully written.\n\r");
}



#if I2C_ASYNC_EN == 1
/*
 * Callback function for the I2C asynchronous transactions:
 *
 * \param [in] error      Error code returned at the end of an I2C transaction.
 * \param [in] user_data  User data that can be passed and used within the function.
 */
void i2c_eeprom_cb(void *user_data, HW_I2C_ABORT_SOURCE error)
{
        /* Read the error status code */
        I2C_error_code = error;

        /* Signal the [prvI2CTask_EEPROM] task that time for resuming has elapsed. */
        OS_EVENT_SIGNAL_FROM_ISR(signal_i2c_eeprom_async);
}
#endif


/* Perform an I2C read operation */
static uint16_t eeprom_data_reader(i2c_device dev, uint16_t pageAddr, uint8_t *buff)
{
       uint8 e_addr_buff[1];

       /* Fill in the address bytes */
       //e_addr_buff[0] = pageAddr >> 8;
       e_addr_buff[0] = pageAddr;


       /* Open the device */
       ad_i2c_handle_t dev_hdr = ad_i2c_open((ad_i2c_controller_conf_t *)dev);

#if (I2C_ASYNC_EN)
       /* Read one page from EEPROM */
       I2C_error_code = ad_i2c_write_read_async(dev_hdr, (const uint8_t *)e_addr_buff,
                                 sizeof(e_addr_buff), buff, EEPROM_24C08_PAGE_SIZE,
                                 i2c_eeprom_cb, NULL, HW_I2C_F_ADD_STOP);

      /*
       * In the meantime and while I2C transactions are performed in the background,
       * application task can proceed to other operations/calculation.
       * It is essential that, the new operations do not involve I2C transactions
       * on the already occupied bus!!!
       */

       /* Wait here until the current asynchronous I2C operation is done. */
       OS_EVENT_WAIT(signal_i2c_eeprom_async, OS_EVENT_FOREVER);
#else
       /* Read one page from EEPROM */
       I2C_error_code = ad_i2c_write_read(dev_hdr, (const uint8_t *)e_addr_buff,
                               sizeof(e_addr_buff), buff, EEPROM_24C08_PAGE_SIZE,
                               HW_I2C_F_ADD_STOP);
#endif

       /* Print on the serial console the status of the I2C operation */
       if (I2C_error_code == 0) {
               /*
                * When maximum data size has been reached, roll over to the starting
                * address.
                */
               pageAddr = (pageAddr + EEPROM_24C08_PAGE_SIZE) % EEPROM_TEST_DATA_SIZE;
       } else {
               printf("\n\rUnsuccessful I2C read operation with error code: %d\n\r",
                                                                        I2C_error_code);
       }

       /* Close the device */
       ad_i2c_close(dev_hdr, false);


       /* Return the next EEPROM page */
       return pageAddr;
}



/**
 * @brief Task responsible for performing I2C related operations
 */
static OS_TASK_FUNCTION(prvI2CTask_EEPROM, pvParameters)     //static void prvI2CTask_EEPROM(void *pvParameters)
{
        uint8_t page_buff[EEPROM_24C08_PAGE_SIZE];
        uint16_t nxt_page_addr = 0;

        printf("\n\r***I2C Demonstration Example***\n\r");

        /*
         * I2C adapter initialization should be done once at the beginning. Alternatively,
         * this function could be called during system initialization in system_init().
         */
        //ad_i2c_init();

        /* Perform a write I2C operation */
        eeprom_data_writer(EEPROM_24C08, 0x0);

        for (;;) {

                /*
                 * Suspend task execution - As soon as WKUP callback function
                 * is triggered, the task resumes its execution.
                 */
                OS_EVENT_WAIT(signal_i2c_eeprom, OS_EVENT_FOREVER);

                /* Perform a read I2C operation */
                nxt_page_addr = eeprom_data_reader(EEPROM_24C08, nxt_page_addr,
                                                                         page_buff);

                //printf("\n\rNext EEPROM page 0x%04X\r\n", nxt_page_addr);
                /* Print on the serial console the currently read data */
                printout_eeprom_page(page_buff, EEPROM_24C08_PAGE_SIZE);
                printf("\n\rNext EEPROM page 0x%04X\r\n", nxt_page_addr);
        }
}

/* WKUP KEY interrupt handler */
static void wkup_cb(void)
{

        /* Clear the WKUP interrupt flag!!! */

        //hw_wkup_reset_interrupt();
        hw_wkup_reset_key_interrupt();

        /*
         * Avoid using printf() within ISR context!!! It may crash your code.
         * Instead, use notifications to notify a task to perform an action!!
         */

        /*
         * Notify [prvI2CTask_EEPROM] that time for
         * performing I2C operations has elapsed.
         */
        OS_EVENT_SIGNAL_FROM_ISR(signal_i2c_eeprom);

}


/* Initialize the WKUP controller */
static void wkup_init(void)
{

        /* Initialize the WKUP controller */
        hw_wkup_init(NULL);

        /*
         * Set debounce time expressed in ms. Maximum allowable value is 63 ms.
         * A value set to 0 disables the debounce functionality.
         */

        //hw_wkup_set_debounce_time(10);
        hw_wkup_set_key_debounce_time(10);

        /*
         * Enable interrupts produced by the KEY block of the wakeup controller (debounce
         * circuitry) and register a callback function to hit following a KEY event.
         */

        //hw_wkup_register_key_interrupt(wkup_cb, 1);
        hw_wkup_register_key_interrupt(wkup_cb, 1);

        /*
         * Set the polarity (rising/falling edge) that triggers the WKUP controller.
         *
         * \note The polarity is applied both to KEY and GPIO blocks of the controller
         *
         */

        //hw_wkup_configure_pin(KEY1_PORT, KEY1_PIN, 1, HW_WKUP_PIN_STATE_LOW);
        hw_wkup_set_trigger(KEY1_PORT, KEY1_PIN, HW_WKUP_TRIG_LEVEL_HI_DEB);

        /* Enable interrupts of WKUP controller */

        //hw_wkup_enable_irq();
        hw_wkup_enable_key_irq();
}

/**
 * @brief Template main creates a SysInit task, which creates a Template task
 */
int main( void )
{
        //OS_BASE_TYPE status;


        /* Start the two tasks as described in the comments at the top of this
        file. */
        OS_TASK_CREATE("SysInit",              /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                        system_init,                    /* The System Initialization task. */
                        ( void * ) 0,                   /* The parameter passed to the task. */
                        configMINIMAL_STACK_SIZE * OS_STACK_WORD_SIZE,
                                                        /* The number of bytes to allocate to the
                                                           stack of the task. */
                        OS_TASK_PRIORITY_HIGHEST,       /* The priority assigned to the task. */
                        xHandle );                      /* The task handle */
        OS_ASSERT(xHandle);



        /* Start the tasks and timer running. */
        OS_TASK_SCHEDULER_RUN();             //vTaskStartScheduler();

        /* If all is well, the scheduler will now be running, and the following
        line will never be reached.  If the following line does execute, then
        there was insufficient FreeRTOS heap memory available for the idle and/or
        timer tasks to be created.  See the memory management section on the
        FreeRTOS web site for more details. */
        for ( ;; );

}

/**
 * @brief Initialize the peripherals domain after power-up.
 *
 */
static void periph_init(void)
{
        #define DA7218_PWR_PIN  HW_GPIO_PORT_0, HW_GPIO_PIN_28

        hw_gpio_configure_pin_power(DA7218_PWR_PIN, HW_GPIO_POWER_VDD1V8P);
        hw_gpio_configure_pin(DA7218_PWR_PIN, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true);
        hw_gpio_pad_latch_enable(DA7218_PWR_PIN);
}

/**
 * @brief Hardware Initialization
 */
static void prvSetupHardware(void)
{
        /*
         * The IRQ produced by the KEY sub block of the wakeup controller
         * (debounced IO IRQ) is multiplexed with other trigger sources
         * (VBUS IRQ, SYS2CMAC IRQ, JTAG present) in a single PDC peripheral
         * trigger ID (HW_PDC_PERIPH_TRIG_ID_COMBO).
         */
#if !defined(CONFIG_USE_BLE) && (!dg_configENABLE_DEBUGGER) && (!dg_configUSE_SYS_CHARGER)

        pdc_wkup_combo_id = hw_pdc_add_entry(HW_PDC_LUT_ENTRY_VAL(
                                                            HW_PDC_TRIG_SELECT_PERIPHERAL,
                                                            HW_PDC_PERIPH_TRIG_ID_COMBO,
                                                            HW_PDC_MASTER_CM33, 0));
        OS_ASSERT(pdc_wkup_combo_id != HW_PDC_INVALID_LUT_INDEX);

        /*  */
        hw_pdc_set_pending(pdc_wkup_combo_id);
        hw_pdc_acknowledge(pdc_wkup_combo_id);
#endif

        wkup_init();

        /* Init hardware */
        pm_system_init(periph_init);

        /* Enable the COM power domain before handling any GPIO pin */
        //hw_sys_pd_com_enable();

        ad_i2c_io_config(((ad_i2c_controller_conf_t *)EEPROM_24C08)->id,
                ((ad_i2c_controller_conf_t *)EEPROM_24C08)->io, AD_IO_CONF_OFF);

        /* Configure the KEY1 push button on Pro DevKit */
        HW_GPIO_SET_PIN_FUNCTION(KEY1);
        HW_GPIO_PAD_LATCH_ENABLE(KEY1);

        /* Lock the mode of the target GPIO pin */
        HW_GPIO_PAD_LATCH_DISABLE(KEY1);

        /* Disable the COM power domain after handling the GPIO pins */
        //hw_sys_pd_com_disable();
}

/**
 * @brief Malloc fail hook
 */
void vApplicationMallocFailedHook( void )
{
        /* vApplicationMallocFailedHook() will only be called if
        configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
        function that will get called if a call to OS_MALLOC() fails.
        OS_MALLOC() is called internally by the kernel whenever a task, queue,
        timer or semaphore is created.  It is also called by various parts of the
        demo application.  If heap_1.c or heap_2.c are used, then the size of the
        heap available to OS_MALLOC() is defined by configTOTAL_HEAP_SIZE in
        FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
        to query the size of free heap space that remains (although it does not
        provide information on how the remaining heap might be fragmented). */
        ASSERT_ERROR(0);
}

/**
 * @brief Application idle task hook
 */
void vApplicationIdleHook( void )
{
        /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
        to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
        task.  It is essential that code added to this hook function never attempts
        to block in any way (for example, call OS_QUEUE_GET() with a block time
        specified, or call OS_DELAY()).  If the application makes use of the
        OS_TASK_DELETE() API function (as this demo application does) then it is also
        important that vApplicationIdleHook() is permitted to return to its calling
        function, because it is the responsibility of the idle task to clean up
        memory allocated by the kernel to any task that has since been deleted. */
}

/**
 * @brief Application stack overflow hook
 */
void vApplicationStackOverflowHook( OS_TASK pxTask, char *pcTaskName )
{
        ( void ) pcTaskName;
        ( void ) pxTask;

        /* Run time stack overflow checking is performed if
        configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
        function is called if a stack overflow is detected. */
        ASSERT_ERROR(0);
}

/**
 * @brief Application tick hook
 */
void vApplicationTickHook( void )
{
}


