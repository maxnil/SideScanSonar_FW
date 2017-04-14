/*
 * conf_sonarsens.h
 *
 * Created: 2016-04-18 17:56:02
 *  Author: Max
 */ 


#ifndef CONF_SONARSENS_H_
#define CONF_SONARSENS_H_

#define SW_VERSION "v0.2.1"

/* Enabled peripheral functions */
#define CONF_SSENS_ENABLE_UART
#define CONF_SSENS_ENABLE_SPI
#define CONF_SSENS_ENABLE_TIMER
#define CONF_SSENS_ENABLE_TWI

/* Enabled peripherals */
#define CONF_SSENS_ENABLE_HMC5983
#define CONF_SSENS_ENABLE_LIS3DSH
#define CONF_SSENS_ENABLE_L3GD20
#define CONF_SSENS_ENABLE_MAX31725
#define CONF_SSENS_ENABLE_TMP006

/* Sonar Sensor UART */
#define CONF_SSENS_SENSOR_UART_BAUDRATE		115200

/* SPI */
#define CONF_SSENS_SPI_BAUDRATE				20000000

#endif /* CONF_SONARSENS_H_ */