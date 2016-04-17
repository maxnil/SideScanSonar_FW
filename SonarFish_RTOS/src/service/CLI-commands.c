/*
 * CLI-commands.c
 * SonarFish CLI Commands
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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* Macro to make the lines shorter */
#define CLI_DEF_T static const CLI_Command_Definition_t

//~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL FUNCTION DECLARATIONS ~~~~~~~~~~~~~~~~~~~~~~~~~

static portBASE_TYPE get_version_cmd        (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE mem_cmd                (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE ping_cmd               (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE ps_cmd                 (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE set_acc_en_cmd         (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE set_cv_en_cmd          (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE set_comp_en_cmd        (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE set_press_en_cmd       (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE set_pwr_en_cmd         (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE set_rx_deadzone_cmd    (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE set_rx_en_cmd          (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE set_rx_gain_cmd        (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE set_rx_gain_offset_cmd (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE set_rx_gain_slope_cmd  (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE set_rx_range_cmd       (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE set_rx_rec_en_cmd      (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE set_sens_rec_en_cmd    (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE set_temp_en_cmd        (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE set_tx_en_cmd          (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE set_tx_mod_cmd         (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE set_tx_pow_cmd         (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE set_tx_pulse_len_cmd   (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE task_stats_cmd         (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL VARIABLES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* CLI command definitions */
CLI_DEF_T get_version_cmd_def        = {"get_version",        "get_version\r\n",		get_version_cmd,        0};
CLI_DEF_T mem_cmd_def                = {"mem",                "mem\r\n",				mem_cmd,                0};
CLI_DEF_T ping_cmd_def               = {"ping",               "ping\r\n",				ping_cmd,               0};
CLI_DEF_T ps_cmd_def                 = {"ps",                 "ps\r\n",					ps_cmd,                 0};
CLI_DEF_T set_acc_en_cmd_def         = {"set_acc_en",         "set_acc_en\r\n",         set_acc_en_cmd,         1};
CLI_DEF_T set_cv_en_cmd_def          = {"set_cv_en",          "set_cv_en\r\n",          set_cv_en_cmd,          1};
CLI_DEF_T set_comp_en_cmd_def        = {"set_comp_en",        "set_comp_en\r\n",        set_comp_en_cmd,        1};
CLI_DEF_T set_press_en_cmd_def       = {"set_press_en",       "set_press_en\r\n",       set_press_en_cmd,       1};
CLI_DEF_T set_pwr_en_cmd_def         = {"set_pwr_en",         "set_pwr_en\r\n",         set_pwr_en_cmd,         1};
CLI_DEF_T set_rx_deadzone_cmd_def    = {"set_rx_deadzone",    "set_rx_deadzone\r\n",    set_rx_deadzone_cmd,    1};
CLI_DEF_T set_rx_en_cmd_def          = {"set_rx_en",          "set_rx_en\r\n",          set_rx_en_cmd,          1};
CLI_DEF_T set_rx_gain_cmd_def        = {"set_rx_gain",        "set_rx_gain\r\n",        set_rx_gain_cmd,        1};
CLI_DEF_T set_rx_gain_offset_cmd_def = {"set_rx_gain_offset", "set_rx_gain_offset\r\n", set_rx_gain_offset_cmd, 1};
CLI_DEF_T set_rx_gain_slope_cmd_def  = {"set_rx_gain_slope",  "set_rx_gain_slope\r\n",  set_rx_gain_slope_cmd,  1};
CLI_DEF_T set_rx_range_cmd_def       = {"set_rx_range",       "set_rx_range\r\n",       set_rx_range_cmd,       1};
CLI_DEF_T set_rx_rec_en_cmd_def      = {"set_rx_rec_en",      "set_rx_rec_en\r\n",      set_rx_rec_en_cmd,      1};
CLI_DEF_T set_sens_rec_en_cmd_def    = {"set_sens_rec_en",    "set_sens_rec_en\r\n",    set_sens_rec_en_cmd,    1};
CLI_DEF_T set_temp_en_cmd_def        = {"set_temp_en",        "set_temp_en\r\n",        set_temp_en_cmd,        1};
CLI_DEF_T set_tx_en_cmd_def          = {"set_tx_en",          "set_tx_en\r\n",          set_tx_en_cmd,          1};
CLI_DEF_T set_tx_mod_cmd_def         = {"set_tx_mod",         "set_tx_mod\r\n",         set_tx_mod_cmd,         1};
CLI_DEF_T set_tx_pow_cmd_def         = {"set_tx_pow",         "set_tx_pow\r\n",         set_tx_pow_cmd,         1};
CLI_DEF_T set_tx_pulse_len_cmd_def   = {"set_tx_pulse_len",   "set_tx_pulse_len\r\n",   set_tx_pulse_len_cmd,   1};
CLI_DEF_T task_stats_cmd_def         = {"task-stats",         "task-stats\r\n",			task_stats_cmd,         0};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*******************************************************************************
 * Registers all CLI commands
 */
void vRegisterCLICommands(void) {
	FreeRTOS_CLIRegisterCommand(&get_version_cmd_def);
	FreeRTOS_CLIRegisterCommand(&mem_cmd_def);
	FreeRTOS_CLIRegisterCommand(&ping_cmd_def);
	FreeRTOS_CLIRegisterCommand(&ps_cmd_def);
	FreeRTOS_CLIRegisterCommand(&set_acc_en_cmd_def);
	FreeRTOS_CLIRegisterCommand(&set_cv_en_cmd_def);
	FreeRTOS_CLIRegisterCommand(&set_comp_en_cmd_def);
	FreeRTOS_CLIRegisterCommand(&set_press_en_cmd_def);
	FreeRTOS_CLIRegisterCommand(&set_pwr_en_cmd_def);
	FreeRTOS_CLIRegisterCommand(&set_rx_deadzone_cmd_def);
	FreeRTOS_CLIRegisterCommand(&set_rx_en_cmd_def);
	FreeRTOS_CLIRegisterCommand(&set_rx_gain_cmd_def);
	FreeRTOS_CLIRegisterCommand(&set_rx_gain_offset_cmd_def);
	FreeRTOS_CLIRegisterCommand(&set_rx_gain_slope_cmd_def);
	FreeRTOS_CLIRegisterCommand(&set_rx_range_cmd_def);
	FreeRTOS_CLIRegisterCommand(&set_rx_rec_en_cmd_def);
	FreeRTOS_CLIRegisterCommand(&set_sens_rec_en_cmd_def);
	FreeRTOS_CLIRegisterCommand(&set_temp_en_cmd_def);
	FreeRTOS_CLIRegisterCommand(&set_tx_en_cmd_def);
	FreeRTOS_CLIRegisterCommand(&set_tx_mod_cmd_def);
	FreeRTOS_CLIRegisterCommand(&set_tx_pow_cmd_def);
	FreeRTOS_CLIRegisterCommand(&set_tx_pulse_len_cmd_def);
	FreeRTOS_CLIRegisterCommand(&task_stats_cmd_def);
}


/*******************************************************************************
 * "Get Version" command
 * Returns SW version
 */
static portBASE_TYPE get_version_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	configASSERT(pcWriteBuffer);

	sprintf(pcWriteBuffer, "SonarFish %s (%s, %s)\r\n", SW_VERSION, __DATE__, __TIME__);

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
 * Returns 'pong'
 */
static portBASE_TYPE ping_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	const uint8_t pong_packet[12] = {START_SYNC_BYTE0, START_SYNC_BYTE1, 0x0E, 0x00, PONG_PACKET, 'p', 'o', 'n', 'g', '2', '\n', 0x00};
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
 * "" command
 * Returns 
 */
static portBASE_TYPE set_acc_en_cmd         (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	return pdFALSE;	
}


/*******************************************************************************
 * "" command
 * Returns 
 */
static portBASE_TYPE set_cv_en_cmd          (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	return pdFALSE;
}


/*******************************************************************************
 * "" command
 * Returns 
 */
static portBASE_TYPE set_comp_en_cmd        (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	return pdFALSE;
}


/*******************************************************************************
 * "" command
 * Returns 
 */
static portBASE_TYPE set_press_en_cmd       (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	return pdFALSE;
}


/*******************************************************************************
 * "Set Power Enable" command
 * Returns 1 if success
 */
static portBASE_TYPE set_pwr_en_cmd         (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	const char *parameter_string;
	portBASE_TYPE parameter_string_length;

	configASSERT(pcWriteBuffer);
	
	/* Obtain the parameter string. */
	parameter_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &parameter_string_length);
	
	/* Check parameter */
	if (parameter_string != NULL && parameter_string[0] == '0') {
		printf("Disable Blue LED\n");
		LED_Off(LED_BLUE);
	} else if (parameter_string != NULL && parameter_string[0] == '1') {
		LED_On(LED_BLUE);
		printf("Enable Blue LED\n");
	}

	/* Return */
	sprintf(pcWriteBuffer, "1\r\n");
	
	return pdFALSE;
}


/*******************************************************************************
 * "" command
 * Returns 
 */
static portBASE_TYPE set_rx_deadzone_cmd    (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	return pdFALSE;
}


/*******************************************************************************
 * "" command
 * Returns 
 */
static portBASE_TYPE set_rx_en_cmd          (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	return pdFALSE;
}


/*******************************************************************************
 * "" command
 * Returns 
 */
static portBASE_TYPE set_rx_gain_cmd        (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	return pdFALSE;
}


/*******************************************************************************
 * "" command
 * Returns 
 */
static portBASE_TYPE set_rx_gain_offset_cmd (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	return pdFALSE;
}


/*******************************************************************************
 * "" command
 * Returns 
 */
static portBASE_TYPE set_rx_gain_slope_cmd  (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	return pdFALSE;
}


/*******************************************************************************
 * "" command
 * Returns 
 */
static portBASE_TYPE set_rx_range_cmd       (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	return pdFALSE;
}


/*******************************************************************************
 * "" command
 * Returns 
 */
static portBASE_TYPE set_rx_rec_en_cmd      (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	return pdFALSE;
}


/*******************************************************************************
 * "" command
 * Returns 
 */
static portBASE_TYPE set_sens_rec_en_cmd    (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	return pdFALSE;
}


/*******************************************************************************
 * "" command
 * Returns 
 */
static portBASE_TYPE set_temp_en_cmd        (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	return pdFALSE;
}


/*******************************************************************************
 * "" command
 * Returns 
 */
static portBASE_TYPE set_tx_en_cmd          (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	return pdFALSE;
}


/*******************************************************************************
 * "" command
 * Returns 
 */
static portBASE_TYPE set_tx_mod_cmd         (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	return pdFALSE;
}


/*******************************************************************************
 * "" command
 * Returns 
 */
static portBASE_TYPE set_tx_pow_cmd         (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	return pdFALSE;
}


/*******************************************************************************
 * "" command
 * Returns 
 */
static portBASE_TYPE set_tx_pulse_len_cmd   (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
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
