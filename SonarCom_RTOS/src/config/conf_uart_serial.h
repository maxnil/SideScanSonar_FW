/*
 * ?onf_uart_serial.h
 *
 * Configuration file for UART/USART interfaces in SonarCom
 *
 * Created: 2016-04-02 12:37:32
 *  Author: Max
 */

#ifndef CONF_USART_SERIAL_H
#define CONF_USART_SERIAL_H

/** Debug UART setting */
#define CONF_UART				CONSOLE_UART
#define CONSOLE_UART_BAUDRATE   115200
#define CONSOLE_UART_PARITY     UART_MR_PAR_NO

/** GPS UART setting */
#define CONF_GPS_BAUDRATE		CONF_SCOM_GPS_USART_BAUDRATE
#define CONF_GPS_ID_USART		ID_USART0
#define CONF_GPS_USART			USART0

/** RS485 UART setting */
#define CONF_RS485_BAUDRATE		CONF_SCOM_RS485_USART_BAUDRATE
#define CONF_RS485_ID_USART		ID_USART1
#define CONF_RS485_USART		USART1

#endif /* CONF_USART_SERIAL_H_INCLUDED */
