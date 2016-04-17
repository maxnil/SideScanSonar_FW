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
struct packet_t idle_packet = {
	.start_sync[0] = START_SYNC_BYTE0,
	.start_sync[1] = START_SYNC_BYTE1,
	.length = 6,
	.type = IDLE_PACKET,
	.data[0] = 0x00		// At least one data bytes needs to be sent
};

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
	struct packet_t *packet_ptr;
	int idle_cnt = 0;
	int cmd_cnt = 0;

	/* Loop forever */
	for (;;) {
	
		/* Allocate new RS485 data packet buffer */
		packet_ptr = (struct packet_t*)pvPortMalloc(RS485_RX_BUFFER_SIZE);

		/* Restart point, we reuse the current buffer since it was not put on any queue */
	reuse_buffer:

		/* Find start of Sonar packet, i.e. first sync byte */
		do {
			/* Get one byte from RS485 UART, timeout after X ms */
			if (freertos_usart_serial_read_packet(CONF_RS485_USART, &(packet_ptr->start_sync[0]), 1, RS485_TIMEOUT_MS/portTICK_PERIOD_MS) != 1) {
				rs485_send_packet();					// Rx timeout, transmit any pending Tx packet
				goto reuse_buffer;						// Restart
			}
		} while ((packet_ptr->start_sync[0]) != START_SYNC_BYTE0);
	
		/* Get second byte, timeout after x ms */
		if (freertos_usart_serial_read_packet(CONF_RS485_USART, &(packet_ptr->start_sync[1]), 1, RS485_TIMEOUT_MS/portTICK_PERIOD_MS) != 1) {
			rs485_send_packet();						// Rx timeout, transmit any pending Tx packet
			goto reuse_buffer;							// Restart
		}

		/* Check that we got the correct second sync byte */
		if ((packet_ptr->start_sync[1]) != START_SYNC_BYTE1) {
			goto reuse_buffer;							// Restart
		}

		/* Get packet length, timeout after x ms */
		if (freertos_usart_serial_read_packet(CONF_RS485_USART, (uint8_t*)&(packet_ptr->length), 2, RS485_TIMEOUT_MS/portTICK_PERIOD_MS) != 2) {
			printf("Did not receive any Sonar Packet before timeout\n");
			rs485_send_packet();						// Rx timeout, transmit any pending Tx packet
			goto reuse_buffer;							// Restart
		}
		
		/* Check packet length */
		packet_len = packet_ptr->length;
		if (packet_len > MAX_PING_PACKET_LENGTH) {
			printf("Length of packet is longer than allowed\n");
			rs485_send_packet();						// Transmit any pending Tx packet
			goto reuse_buffer;							// Restart
			
		}

		/* Get packet type, timeout after x ms */
		if (freertos_usart_serial_read_packet(CONF_RS485_USART, &(packet_ptr->type), 1, RS485_TIMEOUT_MS/portTICK_PERIOD_MS) != 1) {
			rs485_send_packet();						// Rx timeout, transmit any pending Tx packet
			goto reuse_buffer;							// Restart
		}
		
		/* Check packet type */
		if (packet_ptr->type >= UNKNOWN_PACKET) {
			goto reuse_buffer;			
		}

		/* Get rest of packet, timeout after x ms */
		nr_bytes_received = freertos_usart_serial_read_packet(CONF_RS485_USART, (uint8_t*)&(packet_ptr->data), packet_len - PACKET_HEADER_SIZE, RS485_TIMEOUT_MS/portTICK_PERIOD_MS);
		if (nr_bytes_received != (packet_len - PACKET_HEADER_SIZE)) {
			printf("Did not receive a complete Sonar Packet before timeout (%d:%d)\n", packet_len, nr_bytes_received);
			rs485_send_packet();						// Rx timeout, transmit any pending Tx packet
			goto reuse_buffer;							// Restart
		}

		/* Check packet type */
		switch (packet_ptr->type) {
			case COMMAND_PACKET:
				cmd_cnt++;
				/* Put RS485 packet on the CLI command queue */
				printf("RS485: Got command packet\n");
				if (xQueueSend(command_queue, &packet_ptr, portMAX_DELAY) != pdPASS) {
					printf("#WARNING: Failed to put RS485 packet on the command_queue\n");
					rs485_send_packet();				// Transmit any pending Tx packet
					goto reuse_buffer;					// Restart
				}
				break;
			case IDLE_PACKET:
//				printf("RS485: Got idle packet (%d, %d)\n", idle_cnt++, cmd_cnt);
				rs485_send_packet();					// Transmit any pending Tx packet
				goto reuse_buffer;						// Restart
			default:
				printf("RS485: Got unsupported packet type: 0x%.2x\n", packet_ptr->type);
				rs485_send_packet();					// Transmit any pending Tx packet
				goto reuse_buffer;						// Restart
		}

		/* If we reached this far, transmit any pending Tx packet */
		rs485_send_packet();							// Transmit any pending Tx packet
	}
}


/*******************************************************************************
 * RS485 Send one packet from the command_queue to the Sonar Fish
 */
void rs485_send_packet(void) {
	struct packet_t *packet_ptr;
	status_code_t status;

	/* Check if there are any pending data packets to send to the Sonar Fish */	
	if (xQueueReceive(data_channel_queue, &packet_ptr, (TickType_t)0) == pdTRUE) {
		printf("Sending data packet to SonarCom: %s\n", (char*)packet_ptr->data);
		status = freertos_usart_write_packet(CONF_RS485_USART, (uint8_t*)packet_ptr, packet_ptr->length, RS485_TIMEOUT_MS/portTICK_PERIOD_MS);
		if (status != STATUS_OK) {
			printf("RS485: rs485_send_packet (command) failed %d\n", status);
		}
		vPortFree(packet_ptr);
	} else {
//		printf("Sending Idle packet to SonarCom\n");
		status = freertos_usart_write_packet(CONF_RS485_USART, (uint8_t*)&idle_packet, idle_packet.length, RS485_TIMEOUT_MS/portTICK_PERIOD_MS);
		if (status != STATUS_OK) {
			printf("RS485: rs485_send_packet (idle) failed %d\n", status);
		}
	}
}