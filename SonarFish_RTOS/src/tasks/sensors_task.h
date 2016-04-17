/*
 * sensors_task.h
 *
 * Created: 2016-04-13 21:36:36
 *  Author: Max
 */ 


#ifndef SENSORS_TASK_H_
#define SENSORS_TASK_H_

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "task.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* Task priorities */
#define SENSORS_TASK_PRIORITY		(tskIDLE_PRIORITY + 1)

/* Task stack sizes */
#define SENSORS_TASK_STACK_SIZE		(configMINIMAL_STACK_SIZE * 3)

#define SENSORS_RX_BUFFER_SIZE 100

/* Sensors Task creator */
void create_sensors_task(void);

#endif /* SENSORS_TASK_H_ */