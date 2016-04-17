/*
 * uart.h
 *
 * Created: 2013-12-17 22:19:24
 *  Author: Max
 */ 


#ifndef UART_H_
#define UART_H_

void uart_init(void);

/**
 * \brief Function for putting a char in the UART buffer
 *
 * \param data the data to add to the UART buffer and send
 *
 */
void uart_putchar(uint8_t data);

/**
 * \brief Function for getting a char from the UART receive buffer
 *
 * \retval Next data byte in receive buffer
 */
uint8_t uart_getchar(void);

/**
 * \brief Function to check if we have a char waiting in the UART receive buffer
 *
 * \retval true if data is waiting
 * \retval false if no data is waiting
 */
bool uart_char_waiting(void);

#endif /* UART_H_ */