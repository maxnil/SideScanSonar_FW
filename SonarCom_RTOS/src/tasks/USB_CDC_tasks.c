/*
 * USB_CDC_tasks.c
 *
 * Created: 2016-02-14 15:37:32
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
#include "semphr.h"

/* FreeRTOS+ includes. */
#include "FreeRTOS_CLI.h"

/* Atmel library includes. */
#include <udi_cdc.h>
#include <udc.h>

#include "USB_CDC_tasks.h"
#include "task_queues.h"
#include "CLI-commands.h"
#include "packets.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* USB CDC ports */
#define CLI_USB_PORT 0
#define SONAR_USB_PORT 1

/* Dimensions the buffer into which input characters are placed */
#define MAX_CLI_INPUT_SIZE 40

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL VARIABLES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* Semaphore used to signal the arrival of new data */
static SemaphoreHandle_t cdc_new_cli_data_semaphore = NULL;
static SemaphoreHandle_t cdc_new_data_channel_semaphore = NULL;

/* Mutex used to get access to the CDC port for transmitting */
static SemaphoreHandle_t access_mutex = NULL;

/* Flag used to indicated whether or not the CDC port is connected or not */
static volatile bool cdc_connected = false;

//~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL FUNCTION DECLARATIONS ~~~~~~~~~~~~~~~~~~~~~~~~~

static void usb_cdc_command_console_task(void *pvParameters);
static void usb_cdc_data_channel_task(void *pvParameters);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/*******************************************************************************
 * USB CDC Task creator
 * Creates both the CLI and Data Channel USB Task
 */
void create_usb_cdc_tasks(void) {
	/* Register the default CLI commands. */
	vRegisterCLICommands();

	/* Create the semaphore used to signal the arrival of new data on CLI USB port */
	vSemaphoreCreateBinary(cdc_new_cli_data_semaphore);
	configASSERT(cdc_new_cli_data_semaphore);
	
	/* Create the semaphore used to signal the arrival of new data on RAW USB port */
	vSemaphoreCreateBinary(cdc_new_data_channel_semaphore);
	configASSERT(cdc_new_data_channel_semaphore);

	/* Create the semaphore used to access the CDC port as it is written to from
	more than one task. */
	access_mutex = xSemaphoreCreateMutex();
	configASSERT(access_mutex);
	
	/* Start USB */
	if (!udc_include_vbus_monitoring()) {
		/* VBUS monitoring is not available on this product.  Assume VBUS is present. */
		usb_vbus_event(true);
	}

	/* Start USB */
	udc_start();

	/* Create the USART CLI task. */
	xTaskCreate(	usb_cdc_command_console_task,	/* The task that implements the command console. */
					(const char *const) "CDC_CLI",	/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
					CDC_CLI_TASK_STACK_SIZE,		/* The size of the stack allocated to the task. */
					NULL,							/* The parameter is not used. */
					CDC_CLI_TASK_PRIORITY,			/* The priority allocated to the task. */
					NULL);							/* Used to store the handle to the created task - in this case the handle is not required. */

	/* Create the USART CLI task. */
	xTaskCreate(	usb_cdc_data_channel_task,		/* The task that implements the sonar data handler. */
					(const char *const) "CDC_DATA",	/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
					CDC_DATA_CHANNEL_TASK_STACK_SIZE,/* The size of the stack allocated to the task. */
					NULL,							/* The parameter is not used. */
					CDC_DATA_CHANNEL_TASK_PRIORITY,	/* The priority allocated to the task. */
					NULL);							/* Used to store the handle to the created task - in this case the handle is not required. */
}


/*******************************************************************************
 * USB CDC Command Console Task
 * Reads characters from USB CLI Port and processes the commands
 */
static void usb_cdc_command_console_task(void *pvParameters) {
	uint8_t received_char, input_index = 0, *output_string;
	static int8_t input_string[MAX_CLI_INPUT_SIZE];
	portBASE_TYPE returned_value;

	/* Just to remove compiler warnings. */
	(void) pvParameters;

	/* Obtain the address of the output buffer */
	output_string = (uint8_t *) FreeRTOS_CLIGetOutputBuffer();

	for (;;) {
		/* Wait for new data. */
		xSemaphoreTake(cdc_new_cli_data_semaphore, portMAX_DELAY);

		/* Ensure mutually exclusive access is obtained as other tasks can write
		to the CLI. */
		xSemaphoreTake(access_mutex, portMAX_DELAY);

		/* While there are input characters. */
		while (udi_cdc_multi_is_rx_ready(CLI_USB_PORT) == true) {
			received_char = (uint8_t) udi_cdc_multi_getc(CLI_USB_PORT);

			/* Echo the character. */
			udi_cdc_multi_putc(CLI_USB_PORT, received_char);

			if (received_char == '\r') {
				/* Pass the received command to the command interpreter.  The
				command interpreter is called repeatedly until it returns pdFALSE as
				it might generate more than one string. */
				if (input_index > 0) {
					do {
						/* Get the string to write to the UART from the command
						interpreter. */
						returned_value = FreeRTOS_CLIProcessCommand(
								(char *)input_string,
								(char *)output_string,
								configCOMMAND_INT_MAX_OUTPUT_SIZE);

						/* Transmit the generated string. */
						udi_cdc_multi_write_buf(CLI_USB_PORT, (void *) output_string, strlen(
								(char *) output_string));
					} while (returned_value != pdFALSE);
				}
				/* All the strings generated by the input command have been sent.
				Clear the input	string ready to receive the next command. */
				input_index = 0;
				memset(input_string, 0x00, MAX_CLI_INPUT_SIZE);
			} else {
				if (received_char == '\n') {
					/* Ignore the character. */
				} else if (received_char == '\b') {
					/* Backspace was pressed.  Erase the last character in the
					string - if any. */
					if (input_index > 0) {
						input_index--;
						input_string[input_index] = '\0';
					}
				} else {
					/* A character was entered.  Add it to the string
					entered so far.  When a \r is entered the complete
					string will be passed to the command interpreter. */
					if (input_index < MAX_CLI_INPUT_SIZE) {
						input_string[input_index] = received_char;
						input_index++;
					}
				}
			}
		}

		/* Finished with the CDC port, return the mutex until more characters arrive. */
		xSemaphoreGive(access_mutex);
	}
}


/*******************************************************************************
 * USB CDC Data Channel task
 * Sends all data that is in the 'data_channel_queue' to the USB Data Channel
 */
static void usb_cdc_data_channel_task(void *pvParameters) {
	uint8_t *packet_ptr;
	int packet_len;

	/* Just to remove compiler warnings. */
	(void) pvParameters;

	/* Loop forever */
	for (;;) {
		/* Wait for new data. */
		if (xQueueReceive(data_channel_queue, &packet_ptr, portMAX_DELAY) == pdPASS) {
//			printf("Data_packet received: %s", &((struct packet_header_t*)packet_ptr)->data);
			/* Get packet length */
			packet_len = ((struct packet_header_t*)packet_ptr)->length;

			/* Get hold of the USB port, then send packet */
			xSemaphoreTake(access_mutex, portMAX_DELAY);
			udi_cdc_multi_write_buf(SONAR_USB_PORT, packet_ptr, packet_len);
			xSemaphoreGive(access_mutex);

			/* Release buffer */
			vPortFree(packet_ptr);
		}
	}
}


/*******************************************************************************
 * USB CDC CLI string output
 * Sends 'message_string' to the CLI output interface
 */
void cdc_cli_output(const uint8_t *message_string) {
	if (cdc_connected == true) {
		/* This call is outside of the CLI task, so ensure mutually exclusive
		access is obtained. */
		xSemaphoreTake(access_mutex, portMAX_DELAY);
		udi_cdc_multi_write_buf(CLI_USB_PORT, (void *) message_string, strlen((char *) message_string));
		xSemaphoreGive(access_mutex);
	}
}


/*******************************************************************************
 * USB VBUS Event
 */
void usb_vbus_event(bool b_vbus_high) {
	if (b_vbus_high == true) {
		printf("VBUS is high\n");
		udc_attach();
	} else {
		printf("VBUS is low\n");
		udc_detach();
	}
}


/*******************************************************************************
 * USB CDC Received Notify
 * Signals whenever there is new data received on any of the two USB interfaces
 */
void usb_cdc_rx_notify(uint8_t port) {
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* Check which USB port that received data */
	if (port == CLI_USB_PORT) {
//Max		configASSERT(cdc_new_cli_data_semaphore);
		xSemaphoreGiveFromISR(cdc_new_cli_data_semaphore, &xHigherPriorityTaskWoken);
	} else {
//Max		configASSERT(cdc_new_data_channel_semaphore);
		xSemaphoreGiveFromISR(cdc_new_data_channel_semaphore, &xHigherPriorityTaskWoken);		
	}

	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}


/*******************************************************************************
 * USB CDC Enable
 */
bool usb_cdc_enable(uint8_t port) {
	(void) port;

	cdc_connected = true;
	return true;
}


/*******************************************************************************
 * USB CDC Disable
 */
void usb_cdc_disable(uint8_t port) {
	(void) port;

	cdc_connected = false;
}