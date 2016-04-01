/*
 * gps_task.c
 *
 * Created: 2016-02-14 15:37:32
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

#include "gps_task.h"
#include "packets.h"
#include "task_queues.h"

//~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL FUNCTION DECLARATIONS ~~~~~~~~~~~~~~~~~~~~~~~~~

static void gps_task(void *pvParameters);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/*******************************************************************************
 * GPS Task creator
 */
void create_gps_task(void) {
	
	/* Create the USART CLI task. */
	xTaskCreate(	gps_task,						/* The task that implements the GPS data handler */
					(const char *const) "GPS",		/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
					GPS_TASK_STACK_SIZE,			/* The size of the stack allocated to the task. */
					NULL,							/* The parameter is not used. */
					GPS_TASK_PRIORITY,				/* The priority allocated to the task. */
					NULL);							/* Used to store the handle to the created task - in this case the handle is not required. */
}


/*******************************************************************************
 * GPS Task
 * Reads GPS data from GPS UART, and sends it to the USB Data Channel
 * (via the data channel queue)
 */
static void gps_task(void *pvParameters) {
	int packet_len;
	uint8_t *packet_ptr;
	uint8_t *data_ptr;

	/* Loop forever */
	for (;;) {
		/* Allocate new GPS data packet buffer */
		packet_ptr = (uint8_t*)pvPortMalloc(GPS_RX_BUFFER_SIZE);

	reuse_buffer:
	
		/* Get pointer to start of data area in packet */
		data_ptr = &((struct packet_header_t*)packet_ptr)->data;
		
		/* Find start of GPS sentence ('$') */
		do {
			/* Get one character from GPS UART, ignore timeout */
			freertos_usart_serial_read_packet(CONF_GPS_USART, data_ptr, 1, portMAX_DELAY);
		} while (*(data_ptr) != '$');
		
		/* Step to the next byte */
		data_ptr++;
		
		/* Find end of GPS sentence (0x0A) */
		for (packet_len = PACKET_HEADER_SIZE + 1 + PACKET_FOOTER_SIZE + 1; packet_len < GPS_RX_BUFFER_SIZE; packet_len++) {
			/* Get one character from GPS UART */
			freertos_usart_serial_read_packet(CONF_GPS_USART, data_ptr, 1, 1000/portTICK_PERIOD_MS);
			if (*(data_ptr++) == 0x0A) {
				goto end_char_found;						// Stop when we reached the end of GPS sentence
			}
		}
		
		/* The for-loop should never complete unless the end character was not found */
//		printf("GPS: packet buffer overrun\n");
		goto reuse_buffer;		// Restart
		
	end_char_found:

		/* Making sure last byte is 0x00 so we can print it with printf */
		*(data_ptr++) = 0x00;
		packet_len++;
		
		/* Create packet header and footer */
		((struct packet_header_t*)packet_ptr)->start_sync[0] = START_SYNC_BYTE0;
		((struct packet_header_t*)packet_ptr)->start_sync[1] = START_SYNC_BYTE1;
		((struct packet_header_t*)packet_ptr)->length = packet_len;
		((struct packet_header_t*)packet_ptr)->type = GPS_PACKET;
		((struct packet_footer_t*)data_ptr)->end_sync[0] = END_SYNC_BYTE0;
		((struct packet_footer_t*)data_ptr)->end_sync[1] = END_SYNC_BYTE1;

		/* The GPS Sentence has end of line in it, so we do not need to add one here */
//		printf("GPS: %s", &((struct packet_header_t*)packet_ptr)->data);

		/* Put GPS packet on the USB CDC data queue */
		if (!xQueueSend(data_channel_queue, &packet_ptr, portMAX_DELAY)) {
//			printf("#WARNING: Failed to send GPS packet to data_queue\n");
			goto reuse_buffer;		// Restart
		}
	}
}
