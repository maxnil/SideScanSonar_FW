/*
 * CLI-commands.c
 * SonarCom CLI Commands
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

/* Macro to make the lines shorter */
#define CLI_DEF_T static const CLI_Command_Definition_t

//~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL FUNCTION DECLARATIONS ~~~~~~~~~~~~~~~~~~~~~~~~~

static portBASE_TYPE get_date_cmd      (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE get_time_cmd      (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE get_sonar_pwr_cmd (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE get_version_cmd   (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE mem_cmd           (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE ping_cmd          (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE ps_cmd            (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE set_date_cmd      (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE set_sonar_pwr_cmd (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE set_time_cmd      (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE sonar_cmd         (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE task_stats_cmd    (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL VARIABLES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* CLI command definitions */
CLI_DEF_T get_date_cmd_def      = {"get_date",      "get_date\r\n",                    get_date_cmd,      0};
CLI_DEF_T get_sonar_pwr_cmd_def = {"get_sonar_pwr", "get_sonar_pwr\r\n",               get_sonar_pwr_cmd, 0};
CLI_DEF_T get_time_cmd_def      = {"get_time",      "get_time\r\n",                    get_time_cmd,      0};
CLI_DEF_T get_version_cmd_def   = {"get_version",   "get_version\r\n",                 get_version_cmd,   0};
CLI_DEF_T mem_cmd_def           = {"mem",           "mem\r\n",                         mem_cmd,           0};
CLI_DEF_T ping_cmd_def          = {"ping",          "ping\r\n",                        ping_cmd,          0};
CLI_DEF_T ps_cmd_def            = {"ps",            "ps\r\n",                          ps_cmd,            0};
CLI_DEF_T set_date_cmd_def      = {"set_date",      "set_date year-month-day\r\n",     set_date_cmd,      1};
CLI_DEF_T set_sonar_pwr_cmd_def = {"set_sonar_pwr", "set_sonar_pwr 0|1\r\n",           set_sonar_pwr_cmd, 1};
CLI_DEF_T set_time_cmd_def      = {"set_time",      "set_time hour:minute:second\r\n", set_time_cmd,      1};
CLI_DEF_T sonar_cmd_def         = {"sonar",         "sonar \'cmd arg\'\r\n",           sonar_cmd,        -1};
CLI_DEF_T task_stats_cmd_def    = {"task-stats",    "task-stats\r\n",                  task_stats_cmd,    0};

/* Pong packet (can't be declared as const since the PDC can't read from program flash */
struct packet_t pong_packet = {
	.start_sync[0] = START_SYNC_BYTE0,
	.start_sync[1] = START_SYNC_BYTE1,
	.length = 12,
	.type = PONG_PACKET,
	.data[7] = 'p', 'o', 'n', 'g', '2', '\n', 0x00
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*******************************************************************************
 * Registers all CLI commands
 */
void vRegisterCLICommands(void) {
	FreeRTOS_CLIRegisterCommand(&get_date_cmd_def);
	FreeRTOS_CLIRegisterCommand(&get_sonar_pwr_cmd_def);
	FreeRTOS_CLIRegisterCommand(&get_time_cmd_def);
	FreeRTOS_CLIRegisterCommand(&get_version_cmd_def);
	FreeRTOS_CLIRegisterCommand(&mem_cmd_def);
	FreeRTOS_CLIRegisterCommand(&ping_cmd_def);
	FreeRTOS_CLIRegisterCommand(&ps_cmd_def);
	FreeRTOS_CLIRegisterCommand(&set_date_cmd_def);
	FreeRTOS_CLIRegisterCommand(&set_sonar_pwr_cmd_def);
	FreeRTOS_CLIRegisterCommand(&set_time_cmd_def);
	FreeRTOS_CLIRegisterCommand(&sonar_cmd_def);
	FreeRTOS_CLIRegisterCommand(&task_stats_cmd_def);
}


/*******************************************************************************
 * "Get Date" command
 * Returns RTC Date
 */
static portBASE_TYPE get_date_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	uint32_t year, month, day;
	
	configASSERT(pcWriteBuffer);

	/* Get RTC date */
	rtcc_get_date(&year, &month, &day);

	/* Return date */	
	sprintf(pcWriteBuffer, "%d-%.2d-%.2d\r\n", (int)year, (int)month, (int)day);

	return pdFALSE;
}


/*******************************************************************************
 * "Get Sonar Power" command
 */
static portBASE_TYPE get_sonar_pwr_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	configASSERT(pcWriteBuffer);
	
	/* Return current Sonar Power status */
	sprintf(pcWriteBuffer, "%d\r\n", sonar_power_status() ? 1 : 0);
			
	return pdFALSE;
}


/*******************************************************************************
 * "Get Time" command
 * Returns RTC time
 */
static portBASE_TYPE get_time_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	uint32_t hour, minute, second;

	configASSERT(pcWriteBuffer);

	/* Get time from RTC */
	rtcc_get_time(&hour, &minute, &second);

	/* Return RTC time */
	sprintf(pcWriteBuffer, "%d:%.2d:%.2d\r\n", (int)hour, (int)minute, (int)second);

	return pdFALSE;
}


/*******************************************************************************
 * "Get Version" command
 * Returns SW version
 */
static portBASE_TYPE get_version_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	configASSERT(pcWriteBuffer);

	sprintf(pcWriteBuffer, "SonarCom %s (%s, %s)\n", SW_VERSION, __DATE__, __TIME__);

	return pdFALSE;
}


/*******************************************************************************
 * "mem" command
 * Returns memory status
 */
static portBASE_TYPE mem_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	configASSERT(pcWriteBuffer);

	/* Return current Heap memory status */
	sprintf(pcWriteBuffer, "Free heep size: %d bytes\r\n", xPortGetFreeHeapSize());

	return pdFALSE;
}


/*******************************************************************************
 * "ping" command
 * Returns 'pong1' on CLI interface and 'pong2' on data channel
 */
static portBASE_TYPE ping_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	struct packet_t *packet_ptr;
	
	configASSERT(pcWriteBuffer);

	/* Send "ping" back on CLI USB CDC interface */
	sprintf(pcWriteBuffer, "pong1\n");
	
	/* Allocate Pong packet buffer */
	packet_ptr = (struct packet_t*)pvPortMalloc(16);

	/* Copy constant Pong packet */
	memcpy(packet_ptr, &pong_packet, pong_packet.length);

	/* Put Pong packet on the USB CDC data queue */
	if (!xQueueSend(data_channel_queue, &packet_ptr, portMAX_DELAY)) {
		printf("#WARNING: Failed to send Ping packet to data_queue\n");
		vPortFree(packet_ptr);
	}

	return pdFALSE;
}


/*******************************************************************************
 * "ps" command
 * Returns task run-time status
 */
static portBASE_TYPE ps_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	const int8_t *const stats_table_header = (int8_t *) "Task        Abs Time     % Time\r\n*********************************\r\n";

	configASSERT(pcWriteBuffer);

	/* Generate a table of task stats. */
	strcpy((char *) pcWriteBuffer, (char *)stats_table_header);
	vTaskGetRunTimeStats(pcWriteBuffer + strlen((char *)stats_table_header));

	return pdFALSE;
}


/*******************************************************************************
 * "Set Date" command
 * Sets RTC date
 */
static portBASE_TYPE set_date_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
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


/*******************************************************************************
 * "Set Sonar Power Enable" command
 */
static portBASE_TYPE set_sonar_pwr_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
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


/*******************************************************************************
 * "Set Time" command
 * Sets RTC time
 */
static portBASE_TYPE set_time_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
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


/*******************************************************************************
 * "Sonar" command
 * Sends command to SonarFish
 * Returns response from SonarFish
 */
static portBASE_TYPE sonar_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	const char *parameter_string;
	portBASE_TYPE parameter_string_length;
	int parameter_len;
	int packet_len;
	struct packet_t *packet_ptr;
	static int get_response = 0;
	
	configASSERT(pcWriteBuffer);
	
	/* First time around we send the command string */
	if (get_response == 0) {
		/* Obtain the parameter string (don't care about the parameter_string_length */
		parameter_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &parameter_string_length);

		/* Get parameter length (ignore command word) */
		parameter_len = strlen(parameter_string) + 1;			// Add 1 to get the 'end of string' (0x00)
		packet_len = parameter_len + PACKET_HEADER_SIZE;
	
		/* Allocate buffer for SonarFish command */
		packet_ptr = (struct packet_t*)pvPortMalloc(packet_len);
	
		/* Create packet header and footer */
		packet_ptr->start_sync[0] = START_SYNC_BYTE0;
		packet_ptr->start_sync[1] = START_SYNC_BYTE1;
		packet_ptr->length = packet_len;
		packet_ptr->type = COMMAND_PACKET;

		/* Copy parameter string to data packet */
		memcpy(&packet_ptr->data, parameter_string, parameter_len);

	//	printf("Sending \"%s\" to SonarFish (len = %d)\n", parameter_string, parameter_len);

		/* Put Sonar Command packet on command queue */
		if (xQueueSend(command_queue, &packet_ptr, portMAX_DELAY)) {
			get_response = 1;
		} else {
			printf("#WARNING: Failed to send SonarFish Command packet to command_queue\n");
			sprintf(pcWriteBuffer, "Failed to send SonarFish Command packet to command_queue\n");
			vPortFree(packet_ptr);
		}
	}

	if (get_response > 0) {
		/* Get Sonar response */
		if (xQueueReceive(response_queue, &packet_ptr, RESPONSE_TIMEOUT_MS/portTICK_PERIOD_MS) == pdTRUE) {
			printf("Receiving response %d from Sonar: %s\n", get_response, (char*)packet_ptr->data);

			/* Get packet payload */
			sprintf(pcWriteBuffer, (char*)packet_ptr->data);
			vPortFree(packet_ptr);
			if (packet_ptr->type == END_RESPONSE_PACKET) {
				get_response = 0;
				return pdFALSE;
			} else {
				get_response++;
				return pdPASS;
			}
		} else {
			printf("### Sonar command timeout\n");
		}
	}
	
	return pdFALSE;
}


/*******************************************************************************
 * "Task Status" command
 */
static portBASE_TYPE task_stats_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	const int8_t *const task_table_header = (int8_t *) "Task  State Pri Stack #\r\n*************************************\r\n";

	configASSERT(pcWriteBuffer);

	/* Generate a table of task stats. */
	strcpy((char *) pcWriteBuffer, (char *) task_table_header);
	vTaskList(pcWriteBuffer + strlen((char *) task_table_header));

	/* There is no more data to return after this single string, so return
	pdFALSE. */
	return pdFALSE;
}
