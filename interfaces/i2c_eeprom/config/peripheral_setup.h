/*
 * peripheral_setup.h
 *
 *  Created on: Aug 25, 2021
 *      Author: akleinen
 */

#ifndef CONFIG_PERIPHERAL_SETUP_H_
#define CONFIG_PERIPHERAL_SETUP_H_

#define EEPROM_24C08_I2C_ADDRESS    ( 0x50 )

/**
 * I2C 1 configuration
 */
#define EEPROM_24C08_SCL_PORT       ( HW_GPIO_PORT_1 )
#define EEPROM_24C08_SCL_PIN        ( HW_GPIO_PIN_12 )

#define EEPROM_24C08_SDA_PORT       ( HW_GPIO_PORT_1 )
#define EEPROM_24C08_SDA_PIN        ( HW_GPIO_PIN_11 )


/**
 * I2C 2 configuration
 */

#endif /* CONFIG_PERIPHERAL_SETUP_H_ */
