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
#include "rtcc.h"
#include "sonar_fish.h"
#include "conf_sonarcom.h"

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
 * "Sonar" command
 * Sends command to SonarFish
 * Returns response from SonarFish
 */
static portBASE_TYPE sonar_command(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	const char *parameter_string;
	portBASE_TYPE parameter_string_length;
	int parameter_len;
	int packet_len;
	int response_len;
	uint8_t *packet_ptr;
	uint8_t *data_ptr;
	
	configASSERT(pcWriteBuffer);
	
	/* Obtain the parameter string. */
	parameter_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &parameter_string_length);

	/* Get parameter length (ignore command word) */
	parameter_len = strlen(parameter_string);
	packet_len = parameter_len + PACKET_HEADER_SIZE + PACKET_FOOTER_SIZE;
	
	/* Allocate buffer for SonarFish command */
	packet_ptr = (uint8_t*)pvPortMalloc(packet_len);
	
	data_ptr = &((struct packet_header_t*)packet_ptr)->data;
	
	/* Copy parameter string to data packet */
	memcpy(data_ptr, parameter_string, parameter_len);
	data_ptr += parameter_len;

	/* Create packet header and footer */
	((struct packet_header_t*)packet_ptr)->start_sync[0] = START_SYNC_BYTE0;
	((struct packet_header_t*)packet_ptr)->start_sync[1] = START_SYNC_BYTE1;
	((struct packet_header_t*)packet_ptr)->length = packet_len;
	((struct packet_header_t*)packet_ptr)->type = COMMAND_PACKET;
	((struct packet_footer_t*)data_ptr)->end_sync[0] = END_SYNC_BYTE0;
	((struct packet_footer_t*)data_ptr)->end_sync[1] = END_SYNC_BYTE1;

//	printf("Sending \"%s\" to SonarFish (len = %d)\n", parameter_string, parameter_len);

	if (!xQueueSend(command_queue, &packet_ptr, portMAX_DELAY)) {
		printf("#WARNING: Failed to send SonarFish Command packet to command_queue\n");
		vPortFree(packet_ptr);
		sprintf(pcWriteBuffer, "Failed to send SonarFish Command packet to command_queue\n");
	} else {
		if (xQueueReceive(response_queue, &packet_ptr, RESPONSE_TIMEOUT_MS/portTICK_PERIOD_MS) == pdTRUE) {
//			printf("Receiving response from Sonar: %s\n", packet_ptr);
			packet_len = ((struct packet_header_t*)packet_ptr)->length;
			response_len = packet_len - PACKET_HEADER_SIZE - PACKET_FOOTER_SIZE;
//Bort			for (int i = 0; i < packet_len; i++) {
//Bort				printf("0x%.2x ", packet_ptr[i]);
//Bort			}
//Bort			printf("\n");
			/* Get packet payload */
			data_ptr = &((struct packet_header_t*)packet_ptr)->data;
			sprintf(pcWriteBuffer, data_ptr);
			vPortFree(packet_ptr);
		} else {
			printf("### Sonar command timeout\n");
		}
	}
	
	return pdFALSE;
}

static const CLI_Command_Definition_t sonar_command_definition = {
	"sonar",			 		/* The command string to type. */
	"sonar x y:\r\n  Send command 'x' with argument 'y' to SonarFish\r\n\r\n",
	sonar_command,				/* The function to run. */
	-1							/* Variable number of parameters are expected. */
};


/*******************************************************************************
 * "Get CLI Ping" command
 * Returns 'ping'
 */
static portBASE_TYPE get_cli_ping_command(char *pcWriteBuffer, size_t xWriteBufferLen,	const char *pcCommandString) {
	configASSERT(pcWriteBuffer);
	
	sprintf(pcWriteBuffer, "ping\n");
	
	return pdFALSE;
}

static const CLI_Command_Definition_t get_cli_ping_command_definition = {
	"get_cli_ping",				/* The command string to type. */
	"get_cli_ping:\r\n  Returns ping\r\n\r\n",
	get_cli_ping_command,		/* The function to run. */
	0							/* No parameters are expected. */
};


/*******************************************************************************
 * "Get Data Ping" command
 * Returns 'ping' on the Data port
 */
static portBASE_TYPE get_data_ping_command(char *pcWriteBuffer, size_t xWriteBufferLen,	const char *pcCommandString) {
	configASSERT(pcWriteBuffer);
	
	sprintf(pcWriteBuffer, "1 (and ping to the Data port)\n");
	
	return pdFALSE;
}

static const CLI_Command_Definition_t get_data_ping_command_definition = {
	"get_data_ping",			/* The command string to type. */
	"get_data_ping:\r\n  Returns ping on the data port\r\n\r\n",
	get_data_ping_command,		/* The function to run. */
	0							/* No parameters are expected. */
};


/*******************************************************************************
 * "Get Time" command
 * Returns RTC time
 */
static portBASE_TYPE get_time_command(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	uint32_t hour, minute, second;

	configASSERT(pcWriteBuffer);

	/* Get time from RTC */
	rtcc_get_time(&hour, &minute, &second);

	/* Return RTC time */
	sprintf(pcWriteBuffer, "%d:%.2d:%.2d\r\n", (int)hour, (int)minute, (int)second);

	return pdFALSE;
}

static const CLI_Command_Definition_t get_time_command_definition = {
	"get_time",				/* The command string to type. */
	"get_time:\r\n  Displays the current time\r\n\r\n",
	get_time_command,		/* The function to run. */
	0						/* No parameters are expected. */
};


/*******************************************************************************
 * "Set Time" command
 * Sets RTC time
 */
static portBASE_TYPE set_time_command(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	int hour, minute, second;
	const char *parameter_string;
	portBASE_TYPE parameter_string_length;
	int status;
	
	configASSERT(pcWriteBuffer);
	
	/* Obtain the parameter string. */
	parameter_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &parameter_string_length);
	
	/* Check that parameter length is correct */
	if (parameter_string_length != 8) {
		sprintf(pcWriteBuffer, "ARG ERROR\r\n");
		return pdFALSE;
	}

	/* Get time parameters */
	sscanf(parameter_string, "%d:%d:%d", &hour, &minute, &second);

	/* Set time in RTC */
	status = rtcc_set_time(hour, minute, second);
	
	/* Return RTC status */
	sprintf(pcWriteBuffer, "%d\r\n", (status == 0) ? 1 : 0);

	return pdFALSE;
}

static const CLI_Command_Definition_t set_time_command_definition = {
	"set_time",				/* The command string to type. */
	"set_time hour:minute:second:\r\n  Sets the current time\r\n\r\n",
	set_time_command,		/* The function to run. */
	1						/* 1 parameter are expected. */
};


/*******************************************************************************
 * "Get Date" command
 * Returns RTC Date
 */
static portBASE_TYPE get_date_command(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	uint32_t year, month, day;
	
	configASSERT(pcWriteBuffer);

	/* Get RTC date */
	rtcc_get_date(&year, &month, &day);

	/* Return date */	
	sprintf(pcWriteBuffer, "%d-%.2d-%.2d\r\n", (int)year, (int)month, (int)day);

	return pdFALSE;
}

static const CLI_Command_Definition_t get_date_command_definition = {
	"get_date",				/* The command string to type. */
	"get_date:\r\n  Displays the current date\r\n\r\n",
	get_date_command,		/* The function to run. */
	0						/* No parameters are expected. */
};


/*******************************************************************************
 * "Set Date" command
 * Sets RTC date
 */
static portBASE_TYPE set_date_command(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	const char *parameter_string;
	portBASE_TYPE parameter_string_length;
	int year, month, day;
	int status = 0;

	configASSERT(pcWriteBuffer);
	
	/* Obtain the parameter string. */
	parameter_string = FreeRTOS_CLIGetParameter(pcCommandString, 1,	&parameter_string_length);
	
	/* Check that parameter length is correct */
	if (parameter_string_length != 8) {
		sprintf(pcWriteBuffer, "ARG ERROR\r\n");
		return pdFALSE;
	}
	
	/* Get date parameters */
	sscanf(parameter_string, "%d-%d-%d", &year, &month, &day);

	/* Set date in RTC */
	status = rtcc_set_date(year, month, day);

	/* Return RTC status */
	sprintf(pcWriteBuffer, "%d\r\n", (status == 0) ? 1 : 0);

	return pdFALSE;
}

static const CLI_Command_Definition_t set_date_command_definition = {
	"set_date",				/* The command string to type. */
	"set_date year-month-day:\r\n  Sets the current date\r\n\r\n",
	set_date_command,		/* The function to run. */
	1						/* 1 parameter are expected. */
};


/*******************************************************************************
 * "Set Sonar Power Enable" command
 */
static portBASE_TYPE set_sonar_pwr_command(char *pcWriteBuffer, size_t xWriteBufferLen,	const char *pcCommandString) {
	const char *parameter_string;	
	portBASE_TYPE parameter_string_length;

	configASSERT(pcWriteBuffer);
	
	/* Obtain the parameter string. */
	parameter_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &parameter_string_length);
	
	/* Check parameter */
	if (parameter_string != NULL && parameter_string[0] == '0') {
		sonar_power_enable(false);
	} else if (parameter_string != NULL && parameter_string[0] == '1') {
		sonar_power_enable(true);
	}

	/* Return current Sonar Power status */
	sprintf(pcWriteBuffer, "%d\r\n", sonar_power_status() ? 1 : 0);
			
	return pdFALSE;
}

static const CLI_Command_Definition_t set_sonar_pwr_command_definition = {
	"set_sonar_pwr",		/* The command string to type. */
	"set_sonar_pwr 0|1|?:\r\n  0 = off, 1 = on, ? = check status of the Sonar Fish power\r\n\r\n",
	set_sonar_pwr_command,	/* The function to run. */
	1						/* 1 parameters are expected. */
};


/*******************************************************************************
 * "Task Status" command
 */
static portBASE_TYPE task_stats_command(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	const int8_t *const task_table_header = (int8_t *) "Task          State  Priority  Stack	#\r\n************************************************\r\n";

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	(void) pcCommandString;
	(void) xWriteBufferLen;
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
	const int8_t *const stats_table_header = (int8_t *) "Task            Abs Time      % Time\r\n****************************************\r\n";

	(void) pcCommandString;
	(void) xWriteBufferLen;
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
 * Registers all CLI commands
 */
void vRegisterCLICommands(void)
{
	/* Register all the command line commands defined immediately above. */
	FreeRTOS_CLIRegisterCommand(&sonar_command_definition);
	FreeRTOS_CLIRegisterCommand(&get_version_command_definition);
	FreeRTOS_CLIRegisterCommand(&get_cli_ping_command_definition);
	FreeRTOS_CLIRegisterCommand(&get_data_ping_command_definition);
	FreeRTOS_CLIRegisterCommand(&get_time_command_definition);
	FreeRTOS_CLIRegisterCommand(&set_time_command_definition);
	FreeRTOS_CLIRegisterCommand(&get_date_command_definition);
	FreeRTOS_CLIRegisterCommand(&set_date_command_definition);
	FreeRTOS_CLIRegisterCommand(&set_sonar_pwr_command_definition);
	
	FreeRTOS_CLIRegisterCommand(&task_stats_command_definition);
	FreeRTOS_CLIRegisterCommand(&ps_command_definition);
}
