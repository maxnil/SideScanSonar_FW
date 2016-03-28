/*
 * gps_task.h
 *
 * Created: 2016-02-14 15:37:43
 *  Author: Max
 */ 


#ifndef GPS_TASK_H_
#define GPS_TASK_H_

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "task.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* Task priorities */
#define GPS_TASK_PRIORITY		(tskIDLE_PRIORITY + 1)

/* Task stack sizes */
#define GPS_TASK_STACK_SIZE		(configMINIMAL_STACK_SIZE * 3)

#define GPS_RX_BUFFER_SIZE 100

/* GPS Task creator */
void create_gps_task(void);

#endif /* GPS_TASK_H_ */