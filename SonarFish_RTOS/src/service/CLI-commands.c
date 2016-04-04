/*
 * CLI-commands.c
 *
 * Created: 2016-03-01 19:21:04
 *  Author: Max
 */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* FreeRTOS+CLI includes. */
#include "FreeRTOS_CLI.h"

#include "CLI-commands.h"

#include "queue.h"
#include "task_queues.h"

#include "packets.h"
#include "conf_sonarfish.h"
#include "led.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL VARIABLES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*******************************************************************************
 * "Get Version" command
 * Returns SW version
 */
static portBASE_TYPE get_version_command(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	configASSERT(pcWriteBuffer);

	sprintf(pcWriteBuffer, "%s (%s, %s)\n", SW_VERSION, __DATE__, __TIME__);

	return pdFALSE;
}

static const CLI_Command_Definition_t get_version_command_definition = {
	"get_version",				/* The command string to type. */
	"get_version:\r\n  Software version\r\n\r\n",
	get_version_command,		/* The function to run. */
	0							/* No parameters are expected. */
};


/*******************************************************************************
 * "ping" command
 * Returns 'pong1' on CLI interface and 'pong3' on data channel
 */
static portBASE_TYPE ping_command(char *pcWriteBuffer, size_t xWriteBufferLen,	const char *pcCommandString) {
	configASSERT(pcWriteBuffer);

	/* Send "ping" back on CLI USB CDC interface */
	sprintf(pcWriteBuffer, "pong\n");

	return pdFALSE;
}

static const CLI_Command_Definition_t get_cli_ping_command_definition = {
	"ping",					/* The command string to type. */
	"ping:\r\n  Returns \'pong\'\r\n\r\n",
	ping_command,			/* The function to run. */
	0							/* No parameters are expected. */
};


/*******************************************************************************
 * "Set FPGA LED" command
 * Sets FPGA LEDs
 */
static portBASE_TYPE set_fpga_led_command(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	const char *parameter_string;
	portBASE_TYPE parameter_string_length;
	uint8_t val;
	int status;

	configASSERT(pcWriteBuffer);
	
	/* Obtain the parameter string. */
	parameter_string = FreeRTOS_CLIGetParameter(pcCommandString, 1,	&parameter_string_length);

	/* Get LED parameter */
	sscanf(parameter_string, "%d", &val);
// status 0 fpga_set_led(val);

	/* Return status */
	sprintf(pcWriteBuffer, "%d\r\n", (status == 0) ? 1 : 0);

	return pdFALSE;
}

static const CLI_Command_Definition_t set_fpga_led_command_definition = {
	"set_fpga_led",				/* The command string to type. */
	"set_fpga_led value':\r\n  Sets the FPGA LEDs\r\n\r\n",
	set_fpga_led_command,		/* The function to run. */
	1						/* 1 parameter are expected. */
};


/*******************************************************************************
 * "Set Red LED" command
 */
static portBASE_TYPE set_red_led_command(char *pcWriteBuffer, size_t xWriteBufferLen,	const char *pcCommandString) {
	const char *parameter_string;	
	portBASE_TYPE parameter_string_length;

	configASSERT(pcWriteBuffer);
	
	/* Obtain the parameter string. */
	parameter_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &parameter_string_length);
	
	/* Check parameter */
	if (parameter_string != NULL && parameter_string[0] == '0') {
		LED_Off(LED_RED);
	} else if (parameter_string != NULL && parameter_string[0] == '1') {
		LED_On(LED_RED);
	}

	/* Return current Sonar Power status */
	sprintf(pcWriteBuffer, "1\r\n");
			
	return pdFALSE;
}

static const CLI_Command_Definition_t set_red_led_command_definition = {
	"set_red_led",		/* The command string to type. */
	"set_red_ledr 0|1:\r\n  0 = off, 1 = on\r\n\r\n",
	set_red_led_command,	/* The function to run. */
	1						/* 1 parameters are expected. */
};


/*******************************************************************************
 * "Set Red LED" command
 */
static portBASE_TYPE set_red_led_command(char *pcWriteBuffer, size_t xWriteBufferLen,	const char *pcCommandString) {
	const char *parameter_string;	
	portBASE_TYPE parameter_string_length;

	configASSERT(pcWriteBuffer);
	
	/* Obtain the parameter string. */
	parameter_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &parameter_string_length);
	
	/* Check parameter */
	if (parameter_string != NULL && parameter_string[0] == '0') {
		LED_Off(LED_RED);
	} else if (parameter_string != NULL && parameter_string[0] == '1') {
		LED_On(LED_RED);
	}

	/* Return current Sonar Power status */
	sprintf(pcWriteBuffer, "1\r\n");
			
	return pdFALSE;
}

static const CLI_Command_Definition_t set_red_led_command_definition = {
	"set_red_led",		/* The command string to type. */
	"set_red_ledr 0|1:\r\n  0 = off, 1 = on\r\n\r\n",
	set_red_led_command,	/* The function to run. */
	1						/* 1 parameters are expected. */
};


/*******************************************************************************
 * "Task Status" command
 */
static portBASE_TYPE task_stats_command(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	const int8_t *const task_table_header = (int8_t *) "Task  State Pri Stack #\r\n*************************************\r\n";

	configASSERT(pcWriteBuffer);

	/* Generate a table of task stats. */
	strcpy((char *) pcWriteBuffer, (char *) task_table_header);
	vTaskList(pcWriteBuffer + strlen((char *) task_table_header));

	/* There is no more data to return after this single string, so return
	pdFALSE. */
	return pdFALSE;
}

static const CLI_Command_Definition_t task_stats_command_definition = {
	"task-stats",		/* The command string to type. */
	"task-stats:\r\n Displays a table showing the state of each FreeRTOS task\r\n\r\n",
	task_stats_command, /* The function to run. */
	0					/* No parameters are expected. */
};


/*******************************************************************************
 * "ps" command
 * Returns task run-time status
 */
static portBASE_TYPE ps_command(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	const int8_t *const stats_table_header = (int8_t *) "Task        Abs Time     % Time\r\n*********************************\r\n";

	configASSERT(pcWriteBuffer);

	/* Generate a table of task stats. */
	strcpy((char *) pcWriteBuffer, (char *)stats_table_header);
	vTaskGetRunTimeStats(pcWriteBuffer + strlen((char *)stats_table_header));

	return pdFALSE;
}

static const CLI_Command_Definition_t ps_command_definition = {
	"ps",					/* The command string to type. */
	"ps:\r\n Displays a table showing how much processing time each FreeRTOS task has used\r\n\r\n",
	ps_command,				/* The function to run. */
	0						/* No parameters are expected. */
};


/*******************************************************************************
 * "mem" command
 * Returns memory status
 */
static portBASE_TYPE mem_command(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	configASSERT(pcWriteBuffer);

	/* Return current Heap memory status */
	sprintf(pcWriteBuffer, "Free heep size: %d bytes\r\n", xPortGetFreeHeapSize());

	return pdFALSE;
}

static const CLI_Command_Definition_t mem_command_definition = {
	"mem",					/* The command string to type. */
	"mem:\r\n Displays memory usage\r\n\r\n",
	mem_command,				/* The function to run. */
	0						/* No parameters are expected. */
};


/*******************************************************************************
 * Registers all CLI commands
 */
void vRegisterCLICommands(void) {
	/* Register all the command line commands defined immediately above. */
	FreeRTOS_CLIRegisterCommand(&get_version_command_definition);
	FreeRTOS_CLIRegisterCommand(&get_cli_ping_command_definition);
	FreeRTOS_CLIRegisterCommand(&set_fpga_led_command_definition);
	FreeRTOS_CLIRegisterCommand(&set_red_led_command_definition);
	
	FreeRTOS_CLIRegisterCommand(&task_stats_command_definition);
	FreeRTOS_CLIRegisterCommand(&ps_command_definition);
	FreeRTOS_CLIRegisterCommand(&mem_command_definition);
}
