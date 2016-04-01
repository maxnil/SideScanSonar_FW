/*
 * task_queues.c
 *
 * Created: 2016-02-25 22:46:33
 *  Author: Max
 */ 

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"

#include "task_queues.h"
#include "scom.h"

QueueHandle_t command_queue;
QueueHandle_t data_channel_queue;
QueueHandle_t response_queue;

int create_task_queues(void) {
	
	/* Create CLI Response and Data Channel queues */
	command_queue = xQueueCreate(10, sizeof(uint8_t*));
	data_channel_queue = xQueueCreate(10, sizeof(uint8_t*));
	response_queue = xQueueCreate(10, sizeof(uint8_t*));

	configASSERT(command_queue);
	configASSERT(response_queue);
	configASSERT(data_channel_queue);
	
	return 0;
}