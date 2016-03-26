/*
 * USB_CDC_CLI_task.h
 *
 * Created: 2015-06-26 21:22:02
 *  Author: Max
 */ 


#ifndef USB_CDC_CLI_TASK_H_
#define USB_CDC_CLI_TASK_H_

#include "task.h"

/* Task priorities */
#define CDC_CLI_TASK_PRIORITY				(tskIDLE_PRIORITY + 1)
#define CDC_DATA_CHANNEL_TASK_PRIORITY		(tskIDLE_PRIORITY + 1)

/* Task stack sizes */
#define CDC_CLI_TASK_STACK_SIZE				(configMINIMAL_STACK_SIZE * 2)
#define CDC_DATA_CHANNEL_TASK_STACK_SIZE	(configMINIMAL_STACK_SIZE * 4)

/* USB CDC Task creator */
void create_usb_cdc_tasks(void);

void cdc_cli_output(const uint8_t const *message_string);

#endif /* USB_CDC_CLI_TASK_H_ */