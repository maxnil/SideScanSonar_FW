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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define COMMAND_FUNCTION_ARGS char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString

//~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL FUNCTION DECLARATIONS ~~~~~~~~~~~~~~~~~~~~~~~~~

static portBASE_TYPE get_version_command(COMMAND_FUNCTION_ARGS);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL VARIABLES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

static const CLI_Command_Definition_t get_version_command_definition = {
	"get_version",
	"get_version:\r\n  Software version\r\n\r\n",
	get_version_command,
	0};

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
	uint8_t *packet_ptr;
	uint8_t *data_ptr;
	
	configASSERT(pcWriteBuffer);
	
	/* Obtain the parameter string (don't care about the parameter_string_length */
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

	/* Put Sonar Command packet on command queue */
	if (xQueueSend(command_queue, &packet_ptr, portMAX_DELAY)) {
		/* Get Sonar response */
		if (xQueueReceive(response_queue, &packet_ptr, RESPONSE_TIMEOUT_MS/portTICK_PERIOD_MS) == pdTRUE) {
			//			printf("Receiving response from Sonar: %s\n", packet_ptr);
			//			packet_len = ((struct packet_header_t*)packet_ptr)->length;
			//			response_len = packet_len - PACKET_HEADER_SIZE - PACKET_FOOTER_SIZE;

			/* Get packet payload */
			data_ptr = &((struct packet_header_t*)packet_ptr)->data;
			sprintf(pcWriteBuffer, (char*)data_ptr);
			vPortFree(packet_ptr);
		} else {
			printf("### Sonar command timeout\n");
		}
	} else {
		printf("#WARNING: Failed to send SonarFish Command packet to command_queue\n");
		sprintf(pcWriteBuffer, "Failed to send SonarFish Command packet to command_queue\n");
		vPortFree(packet_ptr);
	}
	
	return pdFALSE;
}

static const CLI_Command_Definition_t sonar_command_definition = {
	"sonar",
	"sonar \'cmd arg\' :\r\n  Send command 'x' with argument 'y' to SonarFish\r\n\r\n",
	sonar_command,
	-1
};


/*******************************************************************************
 * "ping" command
 * Returns 'pong1' on CLI interface and 'pong2' on data channel
 */
static portBASE_TYPE ping_command(char *pcWriteBuffer, size_t xWriteBufferLen,	const char *pcCommandString) {
	const uint8_t pong_packet[14] = {START_SYNC_BYTE0, START_SYNC_BYTE1, 0x0E, 0x00, PONG_PACKET, 'p', 'o', 'n', 'g', '2', '\n', 0x00, END_SYNC_BYTE0, END_SYNC_BYTE1};
	uint8_t *packet_ptr;

	configASSERT(pcWriteBuffer);

	/* Send "ping" back on CLI USB CDC interface */
	sprintf(pcWriteBuffer, "pong1\n");
	
	/* Allocate Pong packet buffer */
	packet_ptr = (uint8_t*)pvPortMalloc(16);

	/* Copy constant Pong packet */
	memcpy(packet_ptr, pong_packet, 14);

	/* Put Pong packet on the USB CDC data queue */
	if (!xQueueSend(data_channel_queue, &packet_ptr, portMAX_DELAY)) {
		printf("#WARNING: Failed to send Ping packet to data_queue\n");
		vPortFree(packet_ptr);
	}

	return pdFALSE;
}

static const CLI_Command_Definition_t get_cli_ping_command_definition = {
	"ping",
	"ping:\r\n  Returns \'pong\' on both USB CDC interfaces\r\n\r\n",
	ping_command,
	0
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
	"get_time",
	"get_time:\r\n  Displays the current time\r\n\r\n",
	get_time_command,
	0
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
	"set_time",
	"set_time hour:minute:second:\r\n  Sets the current time\r\n\r\n",
	set_time_command,
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
	"get_date",
	"get_date:\r\n  Displays the current date\r\n\r\n",
	get_date_command,
	0
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
	"set_date",
	"set_date year-month-day:\r\n  Sets the current date\r\n\r\n",
	set_date_command,
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
	"set_sonar_pwr",
	"set_sonar_pwr 0|1|?:\r\n  0 = off, 1 = on, ? = check status of the Sonar Fish power\r\n\r\n",
	set_sonar_pwr_command,
	1
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
	"task-stats",
	"task-stats:\r\n Displays a table showing the state of each FreeRTOS task\r\n\r\n",
	task_stats_command,
	0
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
	"ps",
	"ps:\r\n Displays a table showing how much processing time each FreeRTOS task has used\r\n\r\n",
	ps_command,
	0
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
	"mem",
	"mem:\r\n Displays memory usage\r\n\r\n",
	mem_command,
	0
};


/*******************************************************************************
 * Registers all CLI commands
 */
void vRegisterCLICommands(void) {
	/* Register all the command line commands defined immediately above. */
	FreeRTOS_CLIRegisterCommand(&sonar_command_definition);
	FreeRTOS_CLIRegisterCommand(&get_version_command_definition);
	FreeRTOS_CLIRegisterCommand(&get_cli_ping_command_definition);
	FreeRTOS_CLIRegisterCommand(&get_time_command_definition);
	FreeRTOS_CLIRegisterCommand(&set_time_command_definition);
	FreeRTOS_CLIRegisterCommand(&get_date_command_definition);
	FreeRTOS_CLIRegisterCommand(&set_date_command_definition);
	FreeRTOS_CLIRegisterCommand(&set_sonar_pwr_command_definition);
	
	FreeRTOS_CLIRegisterCommand(&task_stats_command_definition);
	FreeRTOS_CLIRegisterCommand(&ps_command_definition);
	FreeRTOS_CLIRegisterCommand(&mem_command_definition);
}
