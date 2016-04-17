/**
 * \file
 *
 * \brief User board configuration template
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef CONF_BOARD_H
#define CONF_BOARD_H

#define GREEN_LED  LED_GREEN_GPIO
#define YELLOW_LED LED_YELLOW_GPIO

// Slave Select mapping for SPI devices
#define ACCELEROMETER_SS	SENSORS_ACC_CS
#define ECOMPASS_SS			SENSORS_COMP_CS
#define GYRO_SS				SENSORS_GYRO_CS

#define ACCELEROMETER_DRDY	SENSORS_ACC_DRDY
#define ECOMPASS_DRDY       SENSORS_COMP_DRDY
#define GYRO_DRDY			SENSORS_GYRO_DRDY

#endif // CONF_BOARD_H
