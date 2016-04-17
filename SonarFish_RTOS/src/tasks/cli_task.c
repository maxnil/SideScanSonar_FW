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

/* End of response packet (can't be declared as const since the PDC can't read from program flash */
struct packet_t end_reponse_packet = {
	.start_sync[0] = START_SYNC_BYTE0,
	.start_sync[1] = START_SYNC_BYTE1,
	.length = 6,
	.type = END_RESPONSE_PACKET,
	.data[0] = 0x00		// At least one data bytes needs to be sent
};

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
	char *output_string;
	char *input_string;
	struct packet_t *cmd_packet_ptr;
	struct packet_t *resp_packet_ptr;
	int string_len;
	portBASE_TYPE returned_value;

	/* Obtain the address of the output buffer */
	output_string = FreeRTOS_CLIGetOutputBuffer();

	/* Loop forever */
	for (;;) {
		/* Check if there are any pending commands */
		if (xQueueReceive(command_queue, &cmd_packet_ptr, (TickType_t)0) == pdTRUE) {
			input_string = (char*)cmd_packet_ptr->data;
			printf("CLI Task: Command string = %s\n", input_string);
			do {
				/* Get the response string from the command interpreter */
				returned_value = FreeRTOS_CLIProcessCommand(
						input_string,
						output_string,
						configCOMMAND_INT_MAX_OUTPUT_SIZE);

				/* Get response string length */
				string_len = strlen(output_string) + 1;		// Add 1 to get the 'end of string'

				/* Allocate packet buffer */
				resp_packet_ptr = (struct packet_t*)pvPortMalloc(PACKET_HEADER_SIZE + string_len);

				resp_packet_ptr->start_sync[0] = START_SYNC_BYTE0;
				resp_packet_ptr->start_sync[1] = START_SYNC_BYTE1;
				resp_packet_ptr->length = PACKET_HEADER_SIZE + string_len;
				resp_packet_ptr->type = RESPONSE_PACKET;
				memcpy(resp_packet_ptr->data, output_string, string_len);

				/* Transmit the generated string. */
				if (xQueueSend(data_channel_queue, &resp_packet_ptr, portMAX_DELAY) != pdPASS) {
					printf("#WARNING: Failed to put response packet on the reponse_queue\n");
					vPortFree(resp_packet_ptr);
					break;
				}				
			} while (returned_value != pdFALSE);

			/* Allocate end of response packet buffer */
			resp_packet_ptr = (struct packet_t*)pvPortMalloc(end_reponse_packet.length);

			memcpy(resp_packet_ptr, (uint8_t*)&end_reponse_packet, end_reponse_packet.length);

			/* Transmit the generated string. */
			if (xQueueSend(data_channel_queue, &resp_packet_ptr, portMAX_DELAY) != pdPASS) {
				printf("#WARNING: Failed to put response packet on the reponse_queue\n");
				vPortFree(resp_packet_ptr);
				break;
			}
			
			/* Release the command string buffer */
			vPortFree(cmd_packet_ptr);
		}
	}
}