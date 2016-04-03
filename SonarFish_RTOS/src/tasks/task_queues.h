/*
 * task_queues.h
 *
 * Created: 2016-02-25 22:43:16
 *  Author: Max
 */ 


#ifndef TASK_QUEUES_H_
#define TASK_QUEUES_H_

#include "queue.h"

extern QueueHandle_t command_queue;
extern QueueHandle_t data_channel_queue;
extern QueueHandle_t sensors_queue;

int create_task_queues(void);


#endif /* TASK_QUEUES_H_ */