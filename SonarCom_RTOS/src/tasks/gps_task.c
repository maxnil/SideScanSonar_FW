/*
 * gps_task.c
 *
 * Created: 2016-02-14 15:37:32
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

#include "gps_task.h"
#include "packets.h"
#include "task_queues.h"
#include "scom.h"

/* Just to keep the compiler happy */
static void gps_task(void *pvParameters);

/**
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


/**
 * GPS Task
 * Reads GPS data from GPS UART, and sends it to the USB Data Channel (via the data channel queue)
 */
static void gps_task(void *pvParameters) {
	int sentence_len;
	uint8_t *packet_ptr;
	uint8_t data_byte;
	int packet_len;
	uint8_t *tmp_data_ptr;

	/* Loop forever */
	for (;;) {
		/* Allocate new GPS data packet buffer */
		packet_ptr = (uint8_t*)pvPortMalloc(GPS_MAX_SENTENCE_LENGTH + PACKET_HEADER_SIZE + PACKET_FOOTER_SIZE);

	timeout:
		/* Start of data_packet buffer */
		tmp_data_ptr = &((struct packet_header_t*)packet_ptr)->data;
		
		/* Find start of GPS sentence ('$') */
		do {
			/* Get one character from GPS UART, ignore timeout */
			freertos_usart_serial_read_packet(CONF_GPS_USART, &data_byte, 1, 1000/portTICK_PERIOD_MS);
		} while (data_byte != '$');
		
		*(tmp_data_ptr++) = data_byte;		// Should be the '$'
		
		/* Find end of GPS sentence (0x0A) */
		for (sentence_len = 1; sentence_len < GPS_MAX_SENTENCE_LENGTH; sentence_len++) {
			/* Get one character from GPS UART, restart if timeout */
			if (freertos_usart_serial_read_packet(CONF_GPS_USART, tmp_data_ptr, 1, 1000/portTICK_PERIOD_MS) == 0) {
//				printf("GPS; Timeout\n");
				goto timeout;
			}
			if (*(tmp_data_ptr++) == 0x0A) {
				break;
			}
		}
		
		
#if 0
		do {
			/* Get one character from GPS UART, restart if timeout */
			if (freertos_usart_serial_read_packet(CONF_GPS_USART, &data_byte, 1, 1000/portTICK_PERIOD_MS) == 0) {
//				printf("GPS; Timeout\n");
				goto timeout;
			}
			*(tmp_data_ptr++) = data_byte;
		} while (data_byte != 0x0A && (tmp_data_ptr < (packet_ptr + GPS_MAX_SENTENCE_LENGHT + PACKET_HEADER_SIZE)));
#endif
					
		/* Check that we got end of sentence */
//		if (*tmp_data_ptr != 0x0A) {
		if (sentence_len == GPS_MAX_SENTENCE_LENGTH) {
//			printf("GPS: GPS Sentence longer that allowed\n");
			goto timeout;
		}

		/* Making sure last byte is 0x00 so we can print it with printf */
		*(tmp_data_ptr++) = 0x00;
		
		/* Create packet header and footer */
		packet_len = (tmp_data_ptr - packet_ptr) + PACKET_FOOTER_SIZE;
		printf("packet_len   = %d\n", packet_len);
		printf("Sentence len = %d\n", sentence_len + PACKET_HEADER_SIZE + PACKET_FOOTER_SIZE + 2);
		((struct packet_header_t*)packet_ptr)->start_sync = START_SYNC;
		((struct packet_header_t*)packet_ptr)->length = packet_len;
		((struct packet_header_t*)packet_ptr)->type = GPS_PACKET;
		((struct packet_footer_t*)tmp_data_ptr)->end_sync = END_SYNC;

		/* The GPS Sentence has end of line in it, so we do not need to add one here */
//		printf("GPS: %s", data_packet.ptr + PACKET_HEADER_SIZE);

		/* Send GPS packet to the data queue */
		if (!xQueueSend(data_channel_queue, &packet_ptr, (TickType_t)1000)) {
			printf("#WARNING: Failed to send GPS packet to data_queue\n");
			goto timeout;
		}
	}
}
