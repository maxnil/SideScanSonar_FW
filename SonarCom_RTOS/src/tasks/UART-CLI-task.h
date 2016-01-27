/*
 * UART_CLI_task.h
 *
 * Created: 2015-06-21 22:14:49
 *  Author: Max
 */ 


#ifndef UART-CLI-TASK_H_
#define UART-CLI-TASK_H_

#include "uart.h"

void create_uart_cli_task(Uart *uart_base, uint16_t stack_depth_words, unsigned portBASE_TYPE task_priority);

void uart_cli_output(const uint8_t *message_string);

#endif /* UART-CLI-TASK_H_ */