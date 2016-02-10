/*************************************************************************************************************
 * SonarCom CLI-commands
 */

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

#include "rtcc.h"
#include "sonar_fish.h"
#include "conf_sonarcom.h"

/*************************************************************************************************************
 * "Get Version" command. Returns SW version
 *************************************************************************************************************/
static portBASE_TYPE get_version_command(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	configASSERT(pcWriteBuffer);

	sprintf(pcWriteBuffer, "%s (%s, %s)\n", SW_VERSION, __DATE__, __TIME__);

	return pdFALSE;
}

static const CLI_Command_Definition_t get_version_command_definition = {
	"get_version",				/* The command string to type. */
	"get_version:\r\n  Returns software version\r\n\r\n",
	get_version_command,		/* The function to run. */
	0							/* No parameters are expected. */
};


/*************************************************************************************************************
 * "Get Version" command. Returns SW version
 *************************************************************************************************************/
static portBASE_TYPE sonar_command(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	const char *parameter_string;
	portBASE_TYPE parameter_string_length;
//	int status;
	
	configASSERT(pcWriteBuffer);
	
	/* Obtain the parameter string. */
	parameter_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &parameter_string_length);
	
	/* Get time parameters */
//	sscanf(parameter_string, "%d:%d:%d", &hour, &minute, &second);

	configASSERT(pcWriteBuffer);

	printf("Sending \"%s\" to SonarFish\n", parameter_string);
	sprintf(pcWriteBuffer, "sonarFish response\n");

	return pdFALSE;
}

static const CLI_Command_Definition_t sonar_command_definition = {
	"sonar",			 		/* The command string to type. */
	"sonar:\r\n  Returns software version\r\n\r\n",
	sonar_command,				/* The function to run. */
	-1							/* Variable number of parameters are expected. */
};


/*************************************************************************************************************
 * "Get CLI Ping" command. Returns 'ping'
 *************************************************************************************************************/
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


/*************************************************************************************************************
 * "Get Data Ping" command. Returns 'ping' on the Data port
 *************************************************************************************************************/
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


/*************************************************************************************************************
 * "Get Time" command. Returns RTC time
 *************************************************************************************************************/
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


/*************************************************************************************************************
 * "Set Time" command.
 *************************************************************************************************************/
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


/*************************************************************************************************************
 * "Get Date" command. Returns RTC Date
 *************************************************************************************************************/
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


/*************************************************************************************************************
 * "Set Date" command.
 *************************************************************************************************************/
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


/*************************************************************************************************************
 * "Set Sonar Power Enable" command.
 *************************************************************************************************************/
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


/************************************************************************************************************* 
 * "Task Status" command
 *************************************************************************************************************/
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


/*************************************************************************************************************
 * "Run time status" command.
 *************************************************************************************************************/
static portBASE_TYPE run_time_stats_command(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	const int8_t *const stats_table_header = (int8_t *) "Task            Abs Time      % Time\r\n****************************************\r\n";

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	(void) pcCommandString;
	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	/* Generate a table of task stats. */
	strcpy((char *) pcWriteBuffer, (char *) stats_table_header);
	vTaskGetRunTimeStats(pcWriteBuffer + strlen(
			(char *) stats_table_header));

	/* There is no more data to return after this single string, so return
	pdFALSE. */
	return pdFALSE;
}

static const CLI_Command_Definition_t run_time_stats_command_definition = {
	"run-time-stats",		/* The command string to type. */
	"run-time-stats:\r\n Displays a table showing how much processing time each FreeRTOS task has used\r\n\r\n",
	run_time_stats_command, /* The function to run. */
	0						/* No parameters are expected. */
};


#if 0
/*
 * The task that is created by the create-task command.
 */
void created_task(void *pvParameters);

/*
 * Holds the handle of the task created by the create-task command.
 */
static TaskHandle_t created_task_handle = NULL;

/* Structure that defines the "echo_3_parameters" command line command.  This
takes exactly three parameters that the command simply echos back one at a
time. */
static const CLI_Command_Definition_t three_parameter_echo_command_definition =
{
	(const int8_t *const) "echo-3-parameters",
	(const int8_t *const) "echo-3-parameters <param1> <param2> <param3>:\r\n Expects three parameters, echos each in turn\r\n\r\n",
	three_parameter_echo_command, /* The function to run. */
	3 /* Three parameters are expected, which can take any value. */
};

/* Structure that defines the "echo_parameters" command line command.  This
takes a variable number of parameters that the command simply echos back one at
a time. */
static const CLI_Command_Definition_t multi_parameter_echo_command_definition =
{
	(const int8_t *const) "echo-parameters",
	(const int8_t *const) "echo-parameters <...>:\r\n Take variable number of parameters, echos each in turn\r\n\r\n",
	multi_parameter_echo_command, /* The function to run. */
	-1 /* The user can enter any number of commands. */
};

/* Structure that defines the "create-task" command line command.  This takes a
single parameter that is passed into a newly created task.  The task then
periodically writes to the console.  The parameter must be a numerical value. */
static const CLI_Command_Definition_t create_task_command_definition =
{
	(const int8_t *const) "create-task",
	(const int8_t *const) "create-task <param>:\r\n Creates a new task that periodically writes the parameter to the CLI output\r\n\r\n",
	create_task_command, /* The function to run. */
	1 /* A single parameter should be entered. */
};

/* Structure that defines the "delete-task" command line command.  This deletes
the task that was previously created using the "create-command" command. */
static const CLI_Command_Definition_t delete_task_command_definition =
{
	(const int8_t *const) "delete-task",
	(const int8_t *const) "delete-task:\r\n Deletes the task created by the create-task command\r\n\r\n",
	delete_task_command, /* The function to run. */
	0 /* A single parameter should be entered. */
};
#endif
/*-----------------------------------------------------------*/

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
	FreeRTOS_CLIRegisterCommand(&run_time_stats_command_definition);
#if 0
	FreeRTOS_CLIRegisterCommand(&three_parameter_echo_command_definition);
	FreeRTOS_CLIRegisterCommand(&multi_parameter_echo_command_definition);
	FreeRTOS_CLIRegisterCommand(&create_task_command_definition);
	FreeRTOS_CLIRegisterCommand(&delete_task_command_definition);
#endif
}


#if 0
static portBASE_TYPE three_parameter_echo_command(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString)
{
	int8_t *parameter_string;
	portBASE_TYPE parameter_string_length, return_value;
	static portBASE_TYPE parameter_number = 0;

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	(void) pcCommandString;
	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	if (parameter_number == 0) {
		/* The first time the function is called after the command has been
		entered just a header string is returned. */
		sprintf((char *) pcWriteBuffer,
				"The three parameters were:\r\n");

		/* Next time the function is called the first parameter will be echoed
		back. */
		parameter_number = 1L;

		/* There is more data to be returned as no parameters have been echoed
		back yet. */
		return_value = pdPASS;
	} else {
		/* Obtain the parameter string. */
		parameter_string = (int8_t *) FreeRTOS_CLIGetParameter
									(
										pcCommandString,		/* The command string itself. */
										parameter_number,		/* Return the next parameter. */
										&parameter_string_length	/* Store the parameter string length. */
									);

		/* Sanity check something was returned. */
		configASSERT(parameter_string);

		/* Return the parameter string. */
		memset(pcWriteBuffer, 0x00, xWriteBufferLen);
		sprintf((char *) pcWriteBuffer, "%ld: ", parameter_number);
		strncat((char *) pcWriteBuffer, (const char *) parameter_string,
				parameter_string_length);
		strncat((char *) pcWriteBuffer, "\r\n", strlen("\r\n"));

		/* If this is the last of the three parameters then there are no more
		strings to return after this one. */
		if (parameter_number == 3L) {
			/* If this is the last of the three parameters then there are no more
			strings to return after this one. */
			return_value = pdFALSE;
			parameter_number = 0L;
		} else {
			/* There are more parameters to return after this one. */
			return_value = pdTRUE;
			parameter_number++;
		}
	}

	return return_value;
}

/*-----------------------------------------------------------*/

static portBASE_TYPE multi_parameter_echo_command(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString)
{
	int8_t *parameter_string;
	portBASE_TYPE parameter_string_length, return_value;
	static portBASE_TYPE parameter_number = 0;

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	(void) pcCommandString;
	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	if (parameter_number == 0) {
		/* The first time the function is called after the command has been
		entered just a header string is returned. */
		sprintf((char *) pcWriteBuffer, "The parameters were:\r\n");

		/* Next time the function is called the first parameter will be echoed
		back. */
		parameter_number = 1L;

		/* There is more data to be returned as no parameters have been echoed
		back yet. */
		return_value = pdPASS;
	} else {
		/* Obtain the parameter string. */
		parameter_string = (int8_t *) FreeRTOS_CLIGetParameter
									(
										pcCommandString,		/* The command string itself. */
										parameter_number,		/* Return the next parameter. */
										&parameter_string_length	/* Store the parameter string length. */
									);

		if (parameter_string != NULL) {
			/* Return the parameter string. */
			memset(pcWriteBuffer, 0x00, xWriteBufferLen);
			sprintf((char *) pcWriteBuffer, "%ld: ", parameter_number);
			strncat((char *) pcWriteBuffer, (const char *) parameter_string, parameter_string_length);
			strncat((char *) pcWriteBuffer, "\r\n", strlen("\r\n"));

			/* There might be more parameters to return after this one. */
			return_value = pdTRUE;
			parameter_number++;
		} else {
			/* No more parameters were found.  Make sure the write buffer does
			not contain a valid string. */
			pcWriteBuffer[0] = 0x00;

			/* No more data to return. */
			return_value = pdFALSE;

			/* Start over the next time this command is executed. */
			parameter_number = 0;
		}
	}

	return return_value;
}

/*-----------------------------------------------------------*/

static portBASE_TYPE create_task_command(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString)
{
	int8_t *parameter_string;
	portBASE_TYPE parameter_string_length;
	static const int8_t *success_message = (int8_t *) "Task created\r\n";
	static const int8_t *failure_message = (int8_t *) "Task not created\r\n";
	static const int8_t *task_already_created_message = (int8_t *) "The task has already been created. Execute the delete-task command first.\r\n";
	int32_t parameter_value;

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	/* Obtain the parameter string. */
	parameter_string = (int8_t *) FreeRTOS_CLIGetParameter(
									pcCommandString,		/* The command string itself. */
									1,						/* Return the first parameter. */
									&parameter_string_length	/* Store the parameter string length. */
								);

	/* Turn the parameter into a number. */
	parameter_value = (int32_t) atol((const char *) parameter_string);

	/* Attempt to create the task. */
	if (created_task_handle != NULL) {
		strcpy((char *) pcWriteBuffer,
				(const char *) task_already_created_message);
	} else {
		if (xTaskCreate(created_task, (const signed char *) "Created",
				configMINIMAL_STACK_SIZE,
				(void *) parameter_value, tskIDLE_PRIORITY,
				&created_task_handle) == pdPASS) {
			strcpy((char *) pcWriteBuffer,
					(const char *) success_message);
		} else {
			strcpy((char *) pcWriteBuffer,
					(const char *) failure_message);
		}
	}

	/* There is no more data to return after this single string, so return
	pdFALSE. */
	return pdFALSE;
}

/*-----------------------------------------------------------*/

static portBASE_TYPE delete_task_command(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString)
{
	static const int8_t *success_message = (int8_t *) "Task deleted\r\n";
	static const int8_t *failure_message = (int8_t *) "The task was not running.  Execute the create-task command first.\r\n";

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	(void) pcCommandString;
	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	/* See if the task is running. */
	if (created_task_handle != NULL) {
		vTaskDelete(created_task_handle);
		created_task_handle = NULL;
		strcpy((char *) pcWriteBuffer, (const char *) success_message);
	} else {
		strcpy((char *) pcWriteBuffer, (const char *) failure_message);
	}

	/* There is no more data to return after this single string, so return
	 * pdFALSE. */
	return pdFALSE;
}

/*-----------------------------------------------------------*/

void created_task(void *pvParameters)
{
	int32_t parameter_value;
	static uint8_t local_buffer[60];

	/* Cast the parameter to an appropriate type. */
	parameter_value = (int32_t)pvParameters;

	memset((void *) local_buffer, 0x00, sizeof(local_buffer));
	sprintf((char *) local_buffer,
			"Created task running.  Received parameter %ld\r\n\r\n",
			(long) parameter_value);

	/* Cannot yet tell which CLI interface is in use, but both output functions
	guard check the port is initialised before it is used. */
	uart_cli_output(local_buffer);

	cdc_cli_output(local_buffer);

	for (;;) {
		vTaskDelay(portMAX_DELAY);
	}
}
#endif
