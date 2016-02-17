/*
 * rs485.h
 *
 * Created: 2015-01-23 23:43:09
 *  Author: Max
 */ 


#ifndef RS485_H_
#define RS485_H_

/** Constants */
#define RS485_BUFFER_SIZE		256				// Receive and Transmit buffer size
#define RS485_BUFFER_NR			8				// Number of Receive and Transmit buffer
#define RS485_FIFO_SIZE			RS485_BUFFER_NR	// Receive and Transmit FIFO size (max number of buffers)

#define RS485_RX_TIMEOUT		20				// Receive timeout (#bits after last data)


/** State of the RS485 USART link */
typedef enum _rs485_usart_state {
	INITIALIZED,
	LINK_DOWN,
	LINK_UP
} rs485_status_t;


/**
 * Initialize the RS485 controller
 * Return: nothing
  */
void rs485_init(void);

/**
 * Timer initialization
 * Return: nothing
 */
void rs485_timeout_timer_init(uint32_t timeout_val);

/**
 * Get RS485 link status
 * Return: RS485 link status
 */
rs485_status_t rs485_get_status(void);

/**
 * Get buffer and length from RX FIFO
 * Return: Length of buffer, 0 if RX FIFO is empty
 */
uint32_t rs485_rx_fifo_get_buffer(uint8_t **rs485_buf_ptr);

/**
 * Put buffer and length into TX FIFO
 * Return: nothing
 */
void rs485_tx_fifo_put_buffer(uint8_t *rs485_buf_ptr, uint32_t rs485_buf_len);

/**
 * Halt execution
 * Return: nothing
 */
void halt(const char *string);

#endif /* RS485_H_ */