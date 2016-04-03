/*
 * rs485_task.c
 *
 * Created: 2016-03-01 19:21:04
 *  Author: Max
 */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* Standard includes. */
#include <stdint.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "freertos_usart_serial.h"
#include "task.h"
#include "queue.h"

#include "rs485_task.h"
#include "packets.h"
#include "task_queues.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define MAX_PING_PACKET_LENGTH 2100

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL VARIABLES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* Idle packet (can't be declared as const since the PDC can't read from program flash */
uint8_t idle_packet[] = {START_SYNC_BYTE0, START_SYNC_BYTE1, 8, 0x00, IDLE_PACKET, 0xFF, END_SYNC_BYTE0, END_SYNC_BYTE1};

//~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL FUNCTION DECLARATIONS ~~~~~~~~~~~~~~~~~~~~~~~~~

static void rs485_task(void *pvParameters);
void rs485_send_packet(void);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*******************************************************************************
 * RS485 Task creator
 */
void create_rs485_task(void) {

	/* Create the RS485 USART task. */
	xTaskCreate(	rs485_task,						/* The task that implements the RS485 data handler */
					(const char *const) "RS485",	/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
					RS485_TASK_STACK_SIZE,			/* The size of the stack allocated to the task. */
					NULL,							/* The parameter is not used. */
					RS485_TASK_PRIORITY,			/* The priority allocated to the task. */
					NULL);							/* Used to store the handle to the created task - in this case the handle is not required. */
}


/*******************************************************************************
 * RS485 Task
 * Reads Sonar data/response from RS485 UART, and sends it to the
 * USB Data Channel queue or CLI Response queue depending on packet type
 */
static void rs485_task(void *pvParameters) {
	uint16_t packet_len;
	uint16_t nr_bytes_received;
	uint8_t *packet_ptr;
	uint8_t *data_ptr;
	

	/* Loop forever */
	for (;;) {
	
		/* Allocate new RS485 data packet buffer */
		packet_ptr = (uint8_t*)pvPortMalloc(RS485_RX_BUFFER_SIZE);

	/* Restart point, we reuse the current buffer since it was not put on any queue */
reuse_buffer:

	/* Get pointer to start of  packet */
	data_ptr = packet_ptr;
	printf("Hej 0\n");
	/* Find start of Sonar packet, i.e. first sync byte */
	do {
		/* Get one byte from RS485 UART, timeout after X ms */
		if (freertos_usart_serial_read_packet(CONF_RS485_USART, &packet_ptr[0], 1, RS485_TIMEOUT_MS/portTICK_PERIOD_MS) != 1) {
			rs485_send_packet();					// Rx timeout, transmit any pending Tx packet
			goto reuse_buffer;						// Restart
		}
		printf("hoppla\n");
	} while (*(data_ptr) != START_SYNC_BYTE0);		// Little-endian
	printf("Hej 1\n");
	/* Step to the next byte */
	data_ptr++;
	
	/* Get second byte, timeout after x ms */
	if (freertos_usart_serial_read_packet(CONF_RS485_USART, &packet_ptr[1], 1, RS485_TIMEOUT_MS/portTICK_PERIOD_MS) != 1) {
		rs485_send_packet();						// Rx timeout, transmit any pending Tx packet
		goto reuse_buffer;							// Restart
	}
	printf("Hej 2\n");
	printf("data[0] = 0x%0.2x\n", packet_ptr[0]);
	printf("data[1] = 0x%0.2x\n", packet_ptr[1]);
	/* Check that we got the correct second sync byte */
	if (*(data_ptr) != START_SYNC_BYTE1) {			// Little-endian
		goto reuse_buffer;							// Restart
	}

	printf("Hej 3\n");
	/* Get packet length, timeout after x ms */
	if (freertos_usart_serial_read_packet(CONF_RS485_USART, &packet_ptr[2], 2, RS485_TIMEOUT_MS/portTICK_PERIOD_MS) != 2) {
		printf("Did not receive any Sonar Packet length within 1 sec");
		rs485_send_packet();						// Rx timeout, transmit any pending Tx packet
		goto reuse_buffer;							// Restart
	}
	packet_len = (uint16_t)packet_ptr[2];

	printf("Hej 4 packet_len = %d\n", packet_len);
	/* Get rest of packet, timeout after x ms */
	nr_bytes_received = freertos_usart_serial_read_packet(CONF_RS485_USART, &packet_ptr[4], packet_len - PACKET_HEADER_SIZE + 1, RS485_TIMEOUT_MS/portTICK_PERIOD_MS);
	if (nr_bytes_received != (packet_len - PACKET_HEADER_SIZE + 1)) {
		printf("Did not receive a complete Sonar Packet within 1 sec. (%d:%d)\n", packet_len, nr_bytes_received);
		rs485_send_packet();						// Rx timeout, transmit any pending Tx packet
		goto reuse_buffer;							// Restart
	}
	printf("Hej 5\n");
	/* Check the two end sync bytes */
	if ((packet_ptr[packet_len - PACKET_FOOTER_SIZE + 0] != END_SYNC_BYTE0) ||
	(packet_ptr[packet_len - PACKET_FOOTER_SIZE + 1] != END_SYNC_BYTE1)) {
		printf("Did not receive correct end bytes 0x%.2x 0x%.2x (0x%.2x 0x%.2x)\n",
		packet_ptr[packet_len - PACKET_FOOTER_SIZE + 0], packet_ptr[packet_len - PACKET_FOOTER_SIZE + 1],
		END_SYNC_BYTE0, END_SYNC_BYTE1);
		for (int i = 0; i < packet_len+2; i++) {
			printf("0x%0.2x ", packet_ptr[i]);
		}
		printf("\n");
		rs485_send_packet();						// Rx timeout, transmit any pending Tx packet
		goto reuse_buffer;							// Restart
	}
	printf("Hej 6\n");
	/* Check packet type */
	switch (((struct packet_header_t*)packet_ptr)->type) {
		case COMMAND_PACKET:
			/* Put RS485 packet on the CLI command queue */
			if (xQueueSend(command_queue, &packet_ptr, portMAX_DELAY) != pdPASS) {
				printf("#WARNING: Failed to put RS485 packet on the command_queue\n");
				rs485_send_packet();				// Transmit any pending Tx packet
				goto reuse_buffer;					// Restart
			}
			break;
		case IDLE_PACKET:
			printf("RS485: Got idle packet\n");
			rs485_send_packet();					// Transmit any pending Tx packet
			goto reuse_buffer;						// Restart
		default:
			printf("RS485: Got unknown packet type\n");
			rs485_send_packet();					// Transmit any pending Tx packet
			goto reuse_buffer;						// Restart
		}
printf("Hej 7\n");
		/* If we reached this far, transmit any pending Tx packet */
		rs485_send_packet();							// Transmit any pending Tx packet
	}
}


/*******************************************************************************
 * RS485 Send one packet from the command_queue to the Sonar Fish
 */
void rs485_send_packet(void) {
	uint8_t *packet_ptr;
	uint16_t packet_len;
	status_code_t status;


	/* Check if there are any pending commands to send to the Sonar Fish */	
	if (xQueueReceive(data_channel_queue, &packet_ptr, (TickType_t)0) == pdTRUE) {
		printf("Sending data packet to SonarCom: %s\n", packet_ptr);
		packet_len = ((struct packet_header_t*)packet_ptr)->length;
		status = freertos_usart_write_packet(CONF_RS485_USART, packet_ptr, packet_len, RS485_TIMEOUT_MS/portTICK_PERIOD_MS);
		if (status != STATUS_OK) {
			printf("RS485: rs485_send_packet (command) failed %d\n", status);
		}
		vPortFree(packet_ptr);
	} else {
		printf("Sending Idle packet to SonarCom\n");
		packet_len = ((struct packet_header_t*)idle_packet)->length;
		status = freertos_usart_write_packet(CONF_RS485_USART, idle_packet, packet_len, RS485_TIMEOUT_MS/portTICK_PERIOD_MS);
		if (status != STATUS_OK) {
			printf("RS485: rs485_send_packet (idle) failed %d\n", status);
		}
	}
}