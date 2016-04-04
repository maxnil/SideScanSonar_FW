/*
 * command_task.h
 *
 * Created: 2016-04-04 00:00:27
 *  Author: Max
 */ 


#ifndef COMMAND_TASK_H_
#define COMMAND_TASK_H_

#include "task.h"

/* Task priorities */
#define CLI_TASK_PRIORITY		(tskIDLE_PRIORITY + 1)

/* Task stack sizes */
#define CLI_TASK_STACK_SIZE	(configMINIMAL_STACK_SIZE * 4)

/* CLI Task creator */
void create_cli_task(void);

#endif /* COMMAND_TASK_H_ */