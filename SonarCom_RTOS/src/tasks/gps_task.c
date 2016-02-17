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
#include "semphr.h"

#include "gps_task.h"

static void gps_task(void *pvParameters);

void create_gps_task(uint16_t stack_depth_words, unsigned portBASE_TYPE task_priority) {
	/* Create the USART CLI task. */
	xTaskCreate(	gps_task,						/* The task that implements the GPS data handler */
					(const char *const) "GPS",		/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
					stack_depth_words,				/* The size of the stack allocated to the task. */
					NULL,							/* The parameter is not used. */
					task_priority,					/* The priority allocated to the task. */
					NULL);							/* Used to store the handle to the created task - in this case the handle is not required. */
}

/*-----------------------------------------------------------*/

static void gps_task(void *pvParameters) {
	int len;
	uint8_t bytes[GPS_RX_BUFFER_SIZE+1];
	
	for (;;) {
		len = freertos_usart_serial_read_packet(CONF_GPS_USART, bytes, 16, 1000);
		bytes[len] = 0x00;
		if (len > 0) {
			printf("GPS: %s\n", bytes);
		}
		bytes[0] = 'M';
		bytes[1] = 'a';
		bytes[2] = 'x';
//		printf("GPS write packet = %d\n", freertos_usart_write_packet(CONF_GPS_USART, bytes, 3, 1000));
	}
}
