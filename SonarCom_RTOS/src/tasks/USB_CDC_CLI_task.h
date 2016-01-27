/*
 * USB_CDC_CLI_task.h
 *
 * Created: 2015-06-26 21:22:02
 *  Author: Max
 */ 


#ifndef USB_CDC_CLI_TASK_H_
#define USB_CDC_CLI_TASK_H_

void create_usb_cdc_cli_task(uint16_t stack_depth_words, unsigned portBASE_TYPE task_priority);

void cdc_cli_output(const uint8_t const *message_string);

#endif /* USB_CDC_CLI_TASK_H_ */