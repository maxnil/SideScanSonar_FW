/*
 * conf_sonarfish.h
 *
 * Created: 2016-04-03 12:58:07
 *  Author: Max
 */ 


#ifndef CONF_SONARFISH_H_
#define CONF_SONARFISH_H_

#define SW_VERSION "v0.1.2"

#define DEBUG_L1

/* Enabled peripheral functions */
#define CONF_SFISH_ENABLE_ANALOG
#define CONF_SFISH_ENABLE_DEBUG_CONSOLE
//#define CONF_SFISH_ENABLE_TEMP
#define CONF_SFISH_ENABLE_FREERTOS_SENSOR_UART
#define CONF_SFISH_ENABLE_FREERTOS_RS485_USART
#define CONF_SFISH_ENABLE_FREERTOS_SPI
#define CONF_SFISH_ENABLE_FREERTOS_TWI

//#define CONF_SFISH_ENABLE_FPGA

/* Enable tasks */
#define CONF_SFISH_ENABLE_CLI_TASK
#define CONF_SFISH_ENABLE_RS485_TASK
#define CONF_SFISH_ENABLE_SENSORS_TASK
#define CONF_SFISH_ENABLE_TIMER_TASK

/* Debug Console UART */
#define CONF_SFISH_CONSOLE_UART_BAUDRATE	115200

/* Sonar Sensors UART */
#define CONF_SFISH_SENSOR_UART_BAUDRATE		115200

/* Sonar RS485 UART */
//#define CONF_SFISH_RS485_USART_BAUDRATE    921600
#define CONF_SFISH_RS485_USART_BAUDRATE		115200

/* SPI */
#define CONF_SFISH_SPI_BAUDRATE				20000000

#endif /* CONF_SONARFISH_H_ */