/*
 * sensor_task.c
 *
 * Created: 2016-04-13 21:36:49
 *  Author: Max
 */ 

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* Standard includes. */
#include <stdint.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "freertos_uart_serial.h"
#include "task.h"
#include "queue.h"

#include "sensor_task.h"
#include "packets.h"
#include "task_queues.h"

//~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL FUNCTION DECLARATIONS ~~~~~~~~~~~~~~~~~~~~~~~~~

static void sensor_task(void *pvParameters);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/*******************************************************************************
 * Sensor Task creator
 */
void create_sensor_task(void) {
	
	/* Create the UART CLI task. */
	xTaskCreate(	sensor_task,					/* The task that implements the Sensor data handler */
					(const char *const) "SENSOR",	/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
					SENSOR_TASK_STACK_SIZE,			/* The size of the stack allocated to the task. */
					NULL,							/* The parameter is not used. */
					SENSOR_TASK_PRIORITY,			/* The priority allocated to the task. */
					NULL);							/* Used to store the handle to the created task - in this case the handle is not required. */
}


/*******************************************************************************
 * Sensor Task
 * Reads Sensor data from Sensor UART, and sends it to the RS485 Data Channel
 * (via the data channel queue)
 */
static void sensor_task(void *pvParameters) {
	int packet_len;
	struct packet_t *packet_ptr;
	uint8_t *data_ptr;
	
	uint8_t data;
	int len;

	int pb2 = 0;

	/* Loop forever */
	for (;;) {

//uart_enable_rx(CONF_SENSOR_UART);
//uart_enable(CONF_SENSOR_UART);
		/* Allocate new GPS data packet buffer */
		packet_ptr = (struct packet_t*)pvPortMalloc(SENSOR_RX_BUFFER_SIZE);

	reuse_buffer:
	
		/* Get pointer to start of data area in packet */
		data_ptr = (uint8_t*)&(packet_ptr->data);
		
		/* Find start of Sensor sentence ('$') */
		do {
			/* Get one character from Sensor UART, ignore timeout */
			freertos_uart_serial_read_packet(CONF_SENSOR_UART, data_ptr, 1, 1000/portTICK_PERIOD_MS);
//			printf("Sensor char: 0x%.2x\n", *data_ptr);
//			len = uart_read(CONF_SENSOR_UART, &data);
//			printf("uart_read = %d 0x%.2x\n", len, data);
		} while (*(data_ptr) != '$');
		
		/* Step to the next byte */
		data_ptr++;
		
		/* Find end of Sensor sentence (0x0A) */
		/* packet_len includes first data received (+1) */
		for (packet_len = PACKET_HEADER_SIZE + 1; packet_len < SENSOR_RX_BUFFER_SIZE; packet_len++) {
			/* Get one character from Sensor UART */
			freertos_uart_serial_read_packet(CONF_SENSOR_UART, data_ptr, 1, 1000/portTICK_PERIOD_MS);
			if (*(data_ptr++) == 0x0A) {
				goto end_char_found;						// Stop when we reached the end of Sensor sentence
			}
		}
		
		/* The for-loop should never complete unless the end character was not found */
		goto reuse_buffer;		// Restart
		
	end_char_found:
		packet_len++;			// Include the last data received
		
		/* Making sure last byte is 0x00 so we can print it with printf */
		*(data_ptr++) = 0x00;
		packet_len++;

		
		/* Create packet header and footer */
		packet_ptr->start_sync[0] = START_SYNC_BYTE0;
		packet_ptr->start_sync[1] = START_SYNC_BYTE1;
		packet_ptr->length = packet_len;
		packet_ptr->type = SENSOR_PACKET;

		/* The Sensor Sentence has end of line in it, so we do not need to add one here */
//		printf("Length %d ", packet_ptr->length);
//		printf("Sensor: %s", packet_ptr->data);

		/* Put Sensor packet on the data queue */
		if (!xQueueSend(data_channel_queue, &packet_ptr, portMAX_DELAY)) {
			printf("#WARNING: Failed to send GPS packet to data_queue\n");
			goto reuse_buffer;		// Restart
		}
	}
}
