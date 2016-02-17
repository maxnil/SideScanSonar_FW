/*
 * rs485.c
 *
 * SonarCom
 *
 * Created: 2015-01-23 23:43:28
 *  Author: Max
 */ 

#if 0

#include <asf.h>
#include <string.h>
#include "conf_timer.h"
#include "led.h"
#include "rs485.h"

#define ALL_INTERRUPT_MASK 0xFFFFFFFF

/* Receive and Transmit data buffers */
uint8_t g_rs485_rx_buffers[RS485_BUFFER_NR][RS485_BUFFER_SIZE];
uint8_t g_rs485_tx_buffers[RS485_BUFFER_NR][RS485_BUFFER_SIZE];

/* Receive and Transmit buffers length */
uint32_t g_rs485_rx_buffer_len[RS485_BUFFER_NR];
uint32_t g_rs485_tx_buffer_len[RS485_BUFFER_NR];

/* Received and Transmit buffer pointers */
uint32_t g_rs485_rx_curr_buf_nr = 0;					// Current Receive buffer
uint32_t g_rs485_tx_curr_buf_nr = 0;					// Current Transmit buffer


/* FIFO elements */
union fifo_buffer_element {
	uint8_t  byte;
	uint16_t halfword;
	uint32_t word;
};

/* RS485 RX and TX FIFOs */
union fifo_buffer_element g_rs485_rx_fifo[RS485_FIFO_SIZE];
union fifo_buffer_element g_rs485_tx_fifo[RS485_FIFO_SIZE];
fifo_desc_t g_rs485_rx_fifo_desc;
fifo_desc_t g_rs485_tx_fifo_desc;

/** RS485 link state */
rs485_status_t g_st_rs485_state;

/* Dummy/Idle package */
uint8_t g_rs485_dummy_data[4] = {0xDE, 0xAD, 0xBE, 0xEF};


/************************************************************************/
/* Local functions                                                      */
/************************************************************************/

/**
 * Set RS485 link status
 * Return: nothing
 */
void rs485_set_status(rs485_status_t rs485_status_t);

/**
 * Configure PDC to transmit data
 * Return: nothing
 */
void rs485_send_packet(uint8_t *buf_ptr, uint32_t buf_len);

/**
 * Configures PDC to receive data
 * Return: nothing
 */
void rs485_receive_packet(uint8_t *buf_ptr, uint32_t buf_len);

/**
 * Put RX buffer into RX FIFO
 * Return: nothing
 */
void rs485_rx_fifo_put_buffer(uint8_t *buf_ptr, uint32_t buf_len);

/**
 * Get TX buffer pointer and length from TX FIFO
 * Return: Length of buffer, 0 if RX FIFO is empty
 */
uint32_t rs485_tx_fifo_get_buffer(uint8_t **rs485_buf_ptr);


//////////////////////////////////////////////////////////////////////////

/**
 * Halt with error
 * Return: nothing
 */
void halt(const char* string) {
	Disable_global_interrupt();
	printf("Halted: %s\n", string);
	while (1);
}


/**
 * Get buffer and length from RX FIFO
 * Return: Length of buffer, 0 if RX FIFO is empty
 */
uint32_t rs485_rx_fifo_get_buffer(uint8_t **buf_ptr) {
	irqflags_t flags;
	uint32_t buf_nr;
	uint32_t buf_len;
	
	/* Disable interrupts */
	flags = cpu_irq_save();

	/* Check if there are any RX buffers in the FIFO */
	if (fifo_pull_uint32(&g_rs485_rx_fifo_desc, &buf_nr) != FIFO_OK) {
		*buf_ptr = NULL;
		buf_len = 0;
	//		printf("# rs485_rx_fifo_get status %d\n", status);
	} else {
		*buf_ptr = (uint8_t*)g_rs485_rx_buffers[buf_nr];
		buf_len = g_rs485_rx_buffer_len[buf_nr];		
	}

	/* Enable interrupts again */
	cpu_irq_restore(flags);
	return buf_len;
}


/**
 * Get buffer and length from TX FIFO
 * Return: Length of buffer, 0 if TX FIFO is empty
 */
uint32_t rs485_tx_fifo_get_buffer(uint8_t **buf_ptr) {
	irqflags_t flags;
	uint32_t buf_nr;
	uint32_t buf_len;
	
	/* Disable interrupts */
	flags = cpu_irq_save();
	
	/* Check if there are any TX buffers in the FIFO */
	if (fifo_pull_uint32(&g_rs485_tx_fifo_desc, &buf_nr) != FIFO_OK) {
		*buf_ptr = (uint8_t*)NULL;
		buf_len = 0;
	//		printf("# rs485_tx_fifo_get status %d\n", status);
	} else {
		*buf_ptr = (uint8_t*)g_rs485_tx_buffers[buf_nr];
		buf_len = g_rs485_tx_buffer_len[buf_nr];		
	}

	/* Enable interrupts again */
	cpu_irq_restore(flags);
	return buf_len;
}


/**
 * Put packet into RX FIFO
 */
void rs485_rx_fifo_put_buffer(uint8_t *buf_ptr, uint32_t buf_len) {
	irqflags_t flags;

	/* Disable interrupts */
	flags = cpu_irq_save();

	/* Copy data into RX buffer */
	memcpy(g_rs485_rx_buffers[g_rs485_rx_curr_buf_nr], buf_ptr, buf_len);

	if (fifo_push_uint32(&g_rs485_rx_fifo_desc, g_rs485_rx_curr_buf_nr) != FIFO_OK) {
		halt("rs485_rx_fifo_put_buffer: fifo_push_uint32 failed");
	}

	/* Step to the next RX buffer */
	g_rs485_rx_curr_buf_nr = (g_rs485_rx_curr_buf_nr + 1) % RS485_BUFFER_SIZE;

	/* Enable interrupts again */
	cpu_irq_restore(flags);
}


/**
 * Put packet into TX FIFO
 */
void rs485_tx_fifo_put_buffer(uint8_t *buf_ptr, uint32_t buf_len) {
	irqflags_t flags;

	/* Disable interrupts */
	flags = cpu_irq_save();

	/* Copy data into TX buffer */
	memcpy(g_rs485_tx_buffers[g_rs485_tx_curr_buf_nr], buf_ptr, buf_len);
	g_rs485_tx_buffer_len[g_rs485_tx_curr_buf_nr] = buf_len;

	if (fifo_push_uint32(&g_rs485_tx_fifo_desc, g_rs485_tx_curr_buf_nr) != FIFO_OK) {
		halt("rs485_tx_fifo_put_buffer: fifo_push_uint32 failed");
	}

	/* Step to the next TX buffer */
	g_rs485_tx_curr_buf_nr = (g_rs485_tx_curr_buf_nr + 1) % RS485_BUFFER_SIZE;

	/* Enable interrupts again */
	cpu_irq_restore(flags);
}


/**
 * Receive packet
 */
void rs485_receive_packet(uint8_t *buf_ptr, uint32_t buf_len) {
	pdc_packet_t pdc_packet;

	pdc_packet.ul_addr = (uint32_t)buf_ptr;
	pdc_packet.ul_size = buf_len;
	pdc_rx_init(CONF_RS485_PDC, &pdc_packet, NULL);
	usart_enable_interrupt(CONF_RS485_USART, US_IER_TIMEOUT);
}


/**
 * Send packet
 */
void rs485_send_packet(uint8_t *buf_ptr, uint32_t buf_len) {
	pdc_packet_t pdc_packet;
	
	pdc_packet.ul_addr = (uint32_t)buf_ptr;
	pdc_packet.ul_size = buf_len;
	pdc_tx_init(CONF_RS485_PDC, &pdc_packet, NULL);
	usart_enable_interrupt(CONF_RS485_USART, US_IER_TXEMPTY);
}


/**
 * Get RS485 link status
 */
rs485_status_t rs485_get_status(void) {
	return g_st_rs485_state;
}


/**
 * Set RS485 link status
 */
void rs485_set_status(rs485_status_t rs485_state) {
#if 0
	switch (rs485_state) {
		case INITIALIZED:
			printf("INITIALIZED\n");
			break;
		case LINK_DOWN:
			printf("LINK_DOWN\n");
			break;
		case LINK_UP:
			printf("LINK_UP\n");
			break;
		default:
			printf("UNKNOWN");
			break;
	}
#endif	
	g_st_rs485_state = rs485_state;
}


/**
 * RX Timeout Timer interrupt handler
 */
void TC_RS485_TIMEOUT_Handler(void)
{
	uint32_t tc_status;
	
	/* Get Timer status */
	tc_status = tc_get_status(TC_RS485, TC_RS485_TIMEOUT_CHANNEL);
	
	/* Check for RX timeout */
	if (tc_status & TC_SR_CPCS) {
//		printf("=> TC_RS485_TIMEOUT\n");
		/* Disable RX */
		usart_disable_rx(CONF_RS485_USART);
		usart_disable_interrupt(CONF_RS485_USART, US_IER_TIMEOUT);
		pdc_rx_clear_cnt(CONF_RS485_PDC);									// Stop RX DMA

		/* Send dummy packet */
		rs485_send_packet(g_rs485_dummy_data, 4);
		rs485_set_status(LINK_DOWN);
//		printf("<= TC_RS485_TIMEOUT\n");
	}
}


/**
 * RS485 USART interrupt handler
 */
void RS485_USART_Handler(void) {
	uint32_t ul_status;
	int rx_len;
	int tx_len;
	uint8_t *tx_buf_ptr;
	int i;
	
	/* Read USART status. */
	ul_status = usart_get_status(CONF_RS485_USART);
	
	/*-----------------------------------------------------------------------*/
	/* Receive done (timeout), switch to transmit							 */
	/*-----------------------------------------------------------------------*/
	if (ul_status & US_CSR_TIMEOUT) {
//		printf("=> RS485_USART_Handler: US_CSR_TIMEOUT\n");
		/* Turn off RX */
		usart_reset_rx(CONF_RS485_USART);	// Disables RX
		usart_enable_interrupt(CONF_RS485_USART, US_IER_TXEMPTY);
#if 1		
		rx_len = RS485_BUFFER_SIZE - pdc_read_rx_counter(CONF_RS485_PDC);
		printf("Received (%3d): ", rx_len);
		for (i = 0; i < rx_len; i++) {
			printf("0x%.2X ", g_rs485_rx_buffers[g_rs485_rx_curr_buf_nr][i]);
		}
		printf("\n");
#endif

		/* Check receive package length */
		if (rx_len > 4) {
			// If length if greater than 4, we move the received buffer to the RX FIFO	
			rs485_rx_fifo_put_buffer(g_rs485_rx_buffers[g_rs485_rx_curr_buf_nr], rx_len);

			/* Step to the next RX buffer */
			g_rs485_rx_curr_buf_nr = (g_rs485_rx_curr_buf_nr + 1) % RS485_BUFFER_NR;
		}
		
		/* Send package from TX FIFO */
		if ((tx_len = rs485_tx_fifo_get_buffer(&tx_buf_ptr)) >= 0) {	// Get next buffer from TX FIFO
			rs485_send_packet(tx_buf_ptr, tx_len);
		} else {
		/* Send dummy packet */
			rs485_send_packet(g_rs485_dummy_data, 4);
		}
		
		rs485_set_status(LINK_UP);

	/*-----------------------------------------------------------------------*/
	/* Transmitter done, switch to receive									 */
	/*-----------------------------------------------------------------------*/	
	} else if (ul_status & US_CSR_TXEMPTY) {
//		printf("=> RS485_USART_Handler: US_CSR_TXEMPTY\n");
		/* Stop listening for TXEMPTY */
		usart_disable_interrupt(CONF_RS485_USART, US_IER_TXEMPTY);

		/* Turn on RX */
		usart_enable_rx(CONF_RS485_USART);
		
		/* Initialize the receiver */
		rs485_receive_packet(g_rs485_rx_buffers[g_rs485_rx_curr_buf_nr], RS485_BUFFER_SIZE);

		/* Start RX Timeout counters */
		usart_start_rx_timeout(CONF_RS485_USART);				// End of RX packet timeout
		tc_start(TC_RS485, TC_RS485_TIMEOUT_CHANNEL);			// Missing RX packet timeout
//		printf("<= RS485_USART_Handler: US_CSR_TXEMPTY\n");
	}

}


/**
 *  Initialize RS485 UART
 */
void rs485_init(void)
{
	const sam_usart_opt_t usart_console_settings = {
		CONF_RS485_BAUDRATE,
		US_MR_CHRL_8_BIT,
		US_MR_PAR_NO,
		US_MR_NBSTOP_1_BIT,
		US_MR_CHMODE_NORMAL,
		/* This field is only used in IrDA mode. */
		0
	};

	/* Initialize FIFOs */
	fifo_init(&g_rs485_rx_fifo_desc, g_rs485_rx_fifo, RS485_FIFO_SIZE);
	fifo_init(&g_rs485_tx_fifo_desc, g_rs485_tx_fifo, RS485_FIFO_SIZE);

	/* Enable the peripheral clock in the PMC. */
	sysclk_enable_peripheral_clock(CONF_RS485_ID_USART);

	/* Configure USART in RS485 mode. */
	if (usart_init_rs485(CONF_RS485_USART, &usart_console_settings, sysclk_get_cpu_hz())) {
		halt("rs485_init: usart_init_485 failed\n");
	}

	/* Disable all the interrupts */
	usart_disable_interrupt(CONF_RS485_USART, ALL_INTERRUPT_MASK);

	/* Enable transmitter and receiver DMA */
	pdc_enable_transfer(CONF_RS485_PDC, PERIPH_PTCR_RXTEN | PERIPH_PTCR_TXTEN);

	/* Reset transmitter and receiver */
	usart_reset_tx(CONF_RS485_USART);
	usart_reset_rx(CONF_RS485_USART);

	/* Enable transmitter and receiver */
//	usart_enable_rx(CONF_RS485_USART);
	usart_enable_tx(CONF_RS485_USART);

	/* Enable TX Empty and RX Timeout interrupts */
//	usart_enable_interrupt(CONF_RS485_USART, US_IER_TXEMPTY);
	usart_enable_interrupt(CONF_RS485_USART, US_IER_TIMEOUT);

	usart_set_rx_timeout(CONF_RS485_USART, RS485_RX_TIMEOUT);

	/* Initialize the receiver */
	rs485_receive_packet(g_rs485_rx_buffers[g_rs485_rx_curr_buf_nr], RS485_BUFFER_SIZE);
		
	/* Configure and enable interrupt of USART */
	NVIC_EnableIRQ(CONF_RS485_IRQn);
	
	rs485_set_status(INITIALIZED);
}


/**
 * Initialize RS485 link timeout Timer
 */
void rs485_timeout_timer_init(uint32_t timeout_val) {

	/* Configure the PMC to enable the TC module. */
	sysclk_enable_peripheral_clock(ID_TC_RS485_TIMEOUT);

	/** Configure TC interrupts for TC WAVE only */
	NVIC_DisableIRQ(TC_RS485_TIMEOUT_IRQn);
	NVIC_ClearPendingIRQ(TC_RS485_TIMEOUT_IRQn);
	NVIC_SetPriority(TC_RS485_TIMEOUT_IRQn, 0);
	NVIC_EnableIRQ(TC_RS485_TIMEOUT_IRQn);
	
	/* Initialize TC to waveform mode. */
	tc_init(TC_RS485, TC_RS485_TIMEOUT_CHANNEL, TC_RS485_TIMEOUT_CLOCK | TC_CMR_CPCDIS | TC_CMR_WAVE | TC_CMR_WAVSEL_UP);

	/* Configure waveform frequency and duty cycle. */
	tc_write_rc(TC_RS485, TC_RS485_TIMEOUT_CHANNEL, timeout_val);

	tc_enable_interrupt(TC_RS485, TC_RS485_TIMEOUT_CHANNEL, TC_IER_CPCS);

	/* Start Timeout counter */
	tc_start(TC_RS485, TC_RS485_TIMEOUT_CHANNEL);
}

#endif