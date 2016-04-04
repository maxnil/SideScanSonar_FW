/*
 * cli_task.c
 *
 * Created: 2016-04-04 00:00:45
 *  Author: Max
 */ 

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* Standard includes. */
#include <stdint.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* FreeRTOS+ includes. */
#include "FreeRTOS_CLI.h"

#include "cli_task.h"
#include "task_queues.h"
#include "CLI-commands.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* Dimensions the buffer into which input characters are placed */
#define MAX_CLI_INPUT_SIZE 40

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL VARIABLES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL FUNCTION DECLARATIONS ~~~~~~~~~~~~~~~~~~~~~~~~~

static void cli_task(void *pvParameters);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*******************************************************************************
 * CLI Task creator
 */
void create_cli_task(void) {
	
	/* Register the default CLI commands. */
	vRegisterCLICommands();

	/* Create the CLI task. */
	xTaskCreate(	cli_task,						/* The task that implements the RS485 data handler */
					(const char *const) "CLI",		/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
					CLI_TASK_STACK_SIZE,			/* The size of the stack allocated to the task. */
					NULL,							/* The parameter is not used. */
					CLI_TASK_PRIORITY,				/* The priority allocated to the task. */
					NULL);							/* Used to store the handle to the created task - in this case the handle is not required. */
}


/*******************************************************************************
 * CLI Task
 * Reads Commands from RS485 UART, and processes them
 */
static void cli_task(void *pvParameters) {
	uint8_t received_char, input_index = 0, *output_string;
	static int8_t input_string[MAX_CLI_INPUT_SIZE];
	portBASE_TYPE returned_value;

	
	for (;;) {
		/* Get the string to write to the UART from the command interpreter. */
		do {
			/* Get the string to write to the UART from the command
			interpreter. */
			returned_value = FreeRTOS_CLIProcessCommand(
					(char *)input_string,
					(char *)output_string,
					configCOMMAND_INT_MAX_OUTPUT_SIZE);

			/* Transmit the generated string. */
//			udi_cdc_multi_write_buf(CLI_USB_PORT, (void *) output_string, strlen(
//					(char *) output_string));
		} while (returned_value != pdFALSE);
	}
}