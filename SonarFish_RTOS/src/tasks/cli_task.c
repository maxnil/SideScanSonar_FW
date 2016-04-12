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
#include "packets.h"

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
	uint8_t *output_string;
	uint8_t *input_string;
	uint8_t *packet_ptr;
	int string_len;
	portBASE_TYPE returned_value;

	/* Obtain the address of the output buffer */
	output_string = (uint8_t *) FreeRTOS_CLIGetOutputBuffer();

	/* Loop forever */
	for (;;) {
		/* Check if there are any pending commands */
		if (xQueueReceive(command_queue, &input_string, (TickType_t)0) == pdTRUE) {

			do {
				/* Get the response string from the command interpreter */
				returned_value = FreeRTOS_CLIProcessCommand(
						(char *)input_string,
						(char *)output_string,
						configCOMMAND_INT_MAX_OUTPUT_SIZE);

				/* Get response string length */
				string_len = strlen(output_string); 

				/* Allocate output buffer */
				packet_ptr = (uint8_t*)pvPortMalloc(string_len + PACKET_HEADER_SIZE + PACKET_FOOTER_SIZE);

				((struct packet_header_t*)packet_ptr)->start_sync[0] = START_SYNC_BYTE0;
				((struct packet_header_t*)packet_ptr)->start_sync[1] = START_SYNC_BYTE1;
				((struct packet_header_t*)packet_ptr)->length = string_len + PACKET_HEADER_SIZE + PACKET_FOOTER_SIZE;
				memcpy(((struct packet_header_t*)packet_ptr)->data, output_string, string_len);
				packet_ptr[((struct packet_header_t*)packet_ptr)->length - 2] = END_SYNC_BYTE0;
				packet_ptr[((struct packet_header_t*)packet_ptr)->length - 1] = END_SYNC_BYTE1;

				/* Transmit the generated string. */
				if (xQueueSend(data_channel_queue, &packet_ptr, portMAX_DELAY) != pdPASS) {
					printf("#WARNING: Failed to put response packet on the reponse_queue\n");
					vPortFree(output_string);
					break;
				}				
			} while (returned_value != pdFALSE);
		
		/* Release the command string buffer */
		vPortFree(input_string);
		}
	}
}