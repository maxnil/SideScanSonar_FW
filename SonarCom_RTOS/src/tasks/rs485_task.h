/*
 * rs485_task.h
 *
 * Created: 2016-03-01 19:20:49
 *  Author: Max
 */ 


#ifndef RS485_TASK_H_
#define RS485_TASK_H_

#include "task.h"

/* Task priorities */
#define RS485_TASK_PRIORITY		(tskIDLE_PRIORITY + 1)

/* Task stack sizes */
#define RS485_TASK_STACK_SIZE	(configMINIMAL_STACK_SIZE * 4)

#define RS485_RX_BUFFER_SIZE	2048

/* RS485 Task creator */
void create_rs485_task(void);

#endif /* RS485_TASK_H_ */