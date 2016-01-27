/*
 * conf_sonarcom.h
 *
 * Created: 2015-07-01 00:03:35
 *  Author: Max
 */ 


#ifndef CONF_SONARCOM_H_
#define CONF_SONARCOM_H_

#define SW_VERSION "v0.1"

#define DEBUG_L1

// Enabled peripheral functions
#define CONF_SCOM_ENABLE_ANALOG
#define CONF_SCOM_ENABLE_DEBUG_CONSOLE
#define CONF_SCOM_ENABLE_RTCC
#define CONF_SCOM_ENABLE_TWI

/* Debug Console UART */
#define CONF_SCOM_CONSOLE_UART_BAUDRATE   115200

/* Sonar RS485 UART */
//#define CONF_SCOM_RS485_USART_BAUDRATE    921600
#define CONF_SCOM_RS485_USART_BAUDRATE    115200

/* SPI */
#define CONF_SCOM_SPI_BAUDRATE            20000000

#endif /* CONF_SONARCOM_H_ */