/*
 * rs485_task.c
 *
 * Created: 2016-03-01 19:21:04
 *  Author: Max
 */ 

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
#include "scom.h"
#include "sonar_data.h"


/* Just to keep the compiler happy */
static void rs485_task(void *pvParameters);


/**
 * RS485 Task creator
 */
void create_rs485_task(void) {
	/* Create the RS485 USART task. */
	xTaskCreate(	rs485_task,						/* The task that implements the GPS data handler */
					(const char *const) "RS485",	/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
					RS485_TASK_STACK_SIZE,			/* The size of the stack allocated to the task. */
					NULL,							/* The parameter is not used. */
					RS485_TASK_PRIORITY,			/* The priority allocated to the task. */
					NULL);							/* Used to store the handle to the created task - in this case the handle is not required. */
}

uint8_t idle_packet[] = {0xF5, 0xF5, 0xF5, 0xF5, 0x00, 0x08, 0x50, 0x50};

#define MAX_PING_PACKET_LENGTH 2100

/**
 * RS485 Task
 * Reads Sonar data/response from RS485 UART, and sends it to the USB Data Channel queue or CLI Response queue
 */
static void rs485_task(void *pvParameters) {
	uint8_t *packet_ptr;
	int start_cnt;
	int nr_bytes_received;
	uint8_t data_byte;
	uint16_t packet_len;

	/* Loop forever */
	for (;;) {
		/* Find the start of sonar packet (0xAF, 0xAF, 0xAF, 0xAF) */
		start_cnt = 0;
		while (start_cnt++ < RS485_NR_SYNCBYTES) {
			/* Get one byte */
			if (freertos_usart_serial_read_packet(CONF_RS485_USART, &data_byte, 1, 1000/portTICK_PERIOD_MS) != 1) {
				printf("Did not receive any Sonar Packet start byte within 1 sec.\n");
				goto send_packet;
			}
			if (data_byte != RS485_SYNCBYTE) {
				start_cnt = 0;
			}
		}
		
		/* Get packet length */
		nr_bytes_received = freertos_usart_serial_read_packet(CONF_RS485_USART, (uint8_t*)&packet_len, 2, 1000/portTICK_PERIOD_MS);
		if (nr_bytes_received != 2) {
			printf("Did not receive any Sonar Packet length within 1 sec. (%d:2)\n", nr_bytes_received);
			goto send_packet;
		}
		
		/* Remove Sync Bytes and Packet Length Bytes */
		packet_len -= (RS485_NR_SYNCBYTES + 2);
		
		/* Allocate buffer for entire packet */
		configASSERT(packet_ptr = (uint8_t*)pvPortMalloc(packet_len));
		
		/* Get packet including end bytes */
		nr_bytes_received = freertos_usart_serial_read_packet(CONF_RS485_USART, packet_ptr, packet_len, 1000/portTICK_PERIOD_MS);
		if (nr_bytes_received != packet_len) {
			printf("Did not receive a complete Sonar Packet within 1 sec. (%d:%d)\n", nr_bytes_received, packet_len);
			goto send_packet;
		}
		
		/* Check the two last bytes */
		if (packet_ptr[packet_len - RS485_NR_ENDBYTES] != RS485_ENDBYTE || packet_ptr[packet_len - 1] != RS485_ENDBYTE) {
			printf("Did not receive correct end bytes (0x%.2x%.2x)\n", packet_ptr[packet_len - 2], packet_ptr[packet_len - 1]);
			goto send_packet;
		}
		
		/* Remove the end bytes */
//		data_packet.len = packet_len - RS485_NR_ENDBYTES;

		/* Send Sonar packet to the data queue */
		if (!xQueueSend(data_channel_queue, (void *)&packet_ptr, 1000/portTICK_PERIOD_MS)) {
			printf("#WARNING: Failed to send RS485 packet to data_queue\n");
			vPortFree(packet_ptr);
		}
		
	/* Send packet to Sonar */ 
	send_packet:
		if (xQueueReceive(command_queue, (void *)&packet_ptr, 0) == pdTRUE) {
			printf("Sending Command packet to Sonar\n");
		} else {
			printf("Sending Idle packet to Sonar\n");
		}

#if 0
		/* Allocate new RS485 data packet buffer */	
		data_packet.ptr = (uint8_t*)pvPortMalloc(MAX_PING_PACKET_LENGTH);

		/* Create a 'fake' PING packet */
		data_packet.len = create_ping_packet(data_packet.ptr, MAX_PING_PACKET_LENGTH);

		/* Send Sonar packet to the data queue */
		if (!xQueueSend(data_channel_queue, (void *)&data_packet, (TickType_t)1000)) {
			printf("#WARNING: Failed to send RS485 packet to data_queue\n");
			vPortFree(data_packet.ptr);
		}
	
		/* Sleep for 1 second */
		vTaskDelay(1000 / portTICK_PERIOD_MS);
#endif
	}
}
