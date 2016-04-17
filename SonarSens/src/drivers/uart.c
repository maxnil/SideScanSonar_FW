/*
 * uart.c
 *
 * Created: 2013-12-17 22:19:36
 *  Author: Max
 */ 

#include <asf.h>
#include <board.h>
#include "uart.h"

/**
 * \def BUFFER_SIZE
 * \brief The size of the UART buffer
 */
#define BUFFER_SIZE 128

// set the correct BAUD and F_CPU defines before including setbaud.h
#include "conf_clock.h"
#include "conf_uart.h"

/**
 * \name avr_libc_inc avr libc include files
 * @{
 */
#include <util/setbaud.h>
#include <avr/interrupt.h>
//! @}

#include "ring_buffer.h"

// buffers for use with the ring buffer (belong to the UART)
uint8_t out_buffer[BUFFER_SIZE];
uint8_t in_buffer[BUFFER_SIZE];

//! ring buffer to use for the UART transmission
struct ring_buffer ring_buffer_out;
//! ring buffer to use for the UART reception
struct ring_buffer ring_buffer_in;

/**
 * \brief UART data register empty interrupt handler
 *
 * This handler is called each time the UART data register is available for
 * sending data.
 */
ISR(UART0_DATA_EMPTY_IRQ)
{
	// if there is data in the ring buffer, fetch it and send it
	if (!ring_buffer_is_empty(&ring_buffer_out)) {
		UDR0 = ring_buffer_get(&ring_buffer_out);
	}
	else {
		// no more data to send, turn off data ready interrupt
		UCSR0B &= ~(1 << UDRIE0);
	}
}

/**
 * \brief Data RX interrupt handler
 *
 * This is the handler for UART receive data
 */
ISR(UART0_RX_IRQ)
{
	ring_buffer_put(&ring_buffer_in, UDR0);
}

/**
 * \brief Initialize the UART with correct baud rate settings
 *
 * This function will initialize the UART baud rate registers with the correct
 * values using the AVR libc setbaud utility. In addition set the UART to
 * 8-bit, 1 stop and no parity.
 */
void uart_init(void)
{
	// Enable the UART clock
	sysclk_enable_peripheral_clock(&UCSR0A);
	
#if defined UBRR0H
	// get the values from the setbaud tool
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
#else
#error "Device is not supported by the driver"
#endif

#if USE_2X
	UCSR0A |= (1 << U2X0);
#endif

	// enable RX and TX and set interrupts on rx complete
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);

	// 8-bit, 1 stop bit, no parity, asynchronous UART
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00) | (0 << USBS0) |
			(0 << UPM01) | (0 << UPM00) | (0 << UMSEL01) |
			(0 << UMSEL00);

	// initialize the in and out buffer for the UART
	ring_buffer_out = ring_buffer_init(out_buffer, BUFFER_SIZE);
	ring_buffer_in = ring_buffer_init(in_buffer, BUFFER_SIZE);
}

/**
 * \brief Function for putting a char in the UART buffer
 *
 * \param data the data to add to the UART buffer and send
 *
 */
inline void uart_putchar(uint8_t data)
{
	// Disable interrupts to get exclusive access to ring_buffer_out.
	cli();
	if (ring_buffer_is_empty(&ring_buffer_out)) {
		// First data in buffer, enable data ready interrupt
		UCSR0B |=  (1 << UDRIE0);
	}
	// Put data in buffer
	ring_buffer_put(&ring_buffer_out, data);

	// Re-enable interrupts
	sei();
}

/**
 * \brief Function for getting a char from the UART receive buffer
 *
 * \retval Next data byte in receive buffer
 */
inline uint8_t uart_getchar(void)
{
	return ring_buffer_get(&ring_buffer_in);
}


/**
 * \brief Function to check if we have a char waiting in the UART receive buffer
 *
 * \retval true if data is waiting
 * \retval false if no data is waiting
 */
inline bool uart_char_waiting(void)
{
	return !ring_buffer_is_empty(&ring_buffer_in);
}
