/*
 * main.c
 *
 * Main routine for SonarFish_RTOS
 *
 * Created: 2016-04-02 12:37:32
 *  Author: Max
 */ 

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL VARIABLES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL FUNCTION DECLARATIONS ~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*******************************************************************************
 * USB CDC Task creator
 * Creates both the CLI and Data Channel USB Task
 */


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <asf.h>
#include "application_hooks.h"
#include "sfish_init.h"

/* Configuration */
#include "conf_sonarfish.h"
#include "conf_twi_master.h"
#include "conf_spi_master.h"

/* Drivers */
#include "led.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "freertos_peripheral_control.h"
#include "freertos_spi_master.h"
#include "freertos_twi_master.h"
#include "freertos_uart_serial.h"
#include "freertos_usart_serial.h"
#include "task.h"

/* Tasks */
#include "cli_task.h"
#include "rs485_task.h"
#include "sensor_task.h"
#include "timer_task.h"
//#include "USB_CDC_tasks.h"
#include "task_queues.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* The priorities at which various tasks will get created. */
#define SOFTWARE_TIMER_RATE		(200 / portTICK_PERIOD_MS)

#define DBG_WELCOME_HEADER \
"------------------------------------\r\n" \
"-- SonarFish_RTOS 2016 " SW_VERSION "     --\r\n" \
"-- Compiled: "__DATE__" "__TIME__" --\r\n" \
"------------------------------------\r\n" \
"--  ## DEBUG OUTPUT INTERFACE ##  --\r\n" \
"------------------------------------\r\n"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Main routine.
 * Initializes interfaces and components
 * Starts tasks
 */
int main (void) {
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Initialize PLL and clocks
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	sysclk_init();

	/* Ensure all priority bits are assigned as preemption priority bits. */
	NVIC_SetPriorityGrouping(0);
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Initialize the board
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	board_init();
	
	LED_On(LED_RED);	// Turn on Red LED
	LED_Off(LED_BLUE);	// Turn off Blue LED

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Initialize interfaces
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef CONF_SFISH_ENABLE_DEBUG_CONSOLE
	/* Initialize Debug Console */
	sfish_debug_console_init();
	printf(DBG_WELCOME_HEADER);			// Print welcome message on Debug UART
#endif

#ifdef CONF_SFISH_ENABLE_ANALOG
	/* Initialize Analog Front End (ADC) */
	sfish_analog_init();
#endif

	printf("ADC IF 0: %d\n", get_analog_input(0));
	printf("ADC IF 1: %d\n", get_analog_input(1));
	printf("Internal temperature: %d\n", get_chip_temperature());

#ifdef CONF_SFISH_ENABLE_FREERTOS_SENSOR_UART
	/* Initialize FreeRTOS Sensor UART driver */
	uint8_t *sensor_rx_buffer;
	configASSERT(sensor_rx_buffer = (uint8_t*)pvPortMalloc(SENSOR_RX_BUFFER_SIZE));

	freertos_peripheral_options_t sensor_periph_opt = {
		.receive_buffer = sensor_rx_buffer,
		.receive_buffer_size = SENSOR_RX_BUFFER_SIZE,
		.interrupt_priority = configLIBRARY_LOWEST_INTERRUPT_PRIORITY - 1,
		.operation_mode = UART_RS232,
		.options_flags = WAIT_RX_COMPLETE | WAIT_TX_COMPLETE
	};

	sam_uart_opt_t sensor_uart_opt = {
		.ul_baudrate = CONF_SENSOR_UART_BAUDRATE,
		.ul_mode = CONF_SENSOR_UART_PARITY,
		.ul_mck = sysclk_get_peripheral_hz()
	};

	configASSERT(freertos_uart_serial_init(CONF_SENSOR_UART, &sensor_uart_opt, &sensor_periph_opt));
#endif

#ifdef CONF_SFISH_ENABLE_FREERTOS_RS485_USART
	/* Initialize FreeRTOS RS485 USART driver */
	uint8_t *rs485_rx_buffer;
	configASSERT(rs485_rx_buffer = (uint8_t*)pvPortMalloc(RS485_RX_BUFFER_SIZE));

	freertos_peripheral_options_t rs485_periph_opt = {
		.receive_buffer = rs485_rx_buffer,
		.receive_buffer_size = RS485_RX_BUFFER_SIZE,
		.interrupt_priority = configLIBRARY_LOWEST_INTERRUPT_PRIORITY,
		.operation_mode = USART_RS232,
		.options_flags = WAIT_RX_COMPLETE | WAIT_TX_COMPLETE
	};

	sam_usart_opt_t rs485_usart_opt = {
		.baudrate = CONF_RS485_BAUDRATE,
		.channel_mode = US_MR_CHMODE_NORMAL,
		.char_length = US_MR_CHRL_8_BIT,
		.irda_filter = 0,
		.parity_type = CONF_RS485_UART_PARITY,
		.stop_bits = US_MR_NBSTOP_1_BIT
	};

	configASSERT(freertos_usart_serial_init(CONF_RS485_UART, &rs485_usart_opt, &rs485_periph_opt));
	
	/* Set RS485 mode. */
	CONF_RS485_UART->US_MR = (CONF_RS485_UART->US_MR & ~US_MR_USART_MODE_Msk) | US_MR_USART_MODE_RS485;
#endif

#ifdef CONF_SFISH_ENABLE_FREERTOS_SPI
	/* Initialize FreeRTOS SPI driver */
	freertos_peripheral_options_t spi_periph_opt = {
		.receive_buffer = NULL,						// Not used by the SPI driver
		.receive_buffer_size = 0,					// Not used by the SPI driver
		.interrupt_priority = configLIBRARY_LOWEST_INTERRUPT_PRIORITY,
		.operation_mode = SPI_MASTER,
		.options_flags = WAIT_RX_COMPLETE | WAIT_TX_COMPLETE
	};

	configASSERT(freertos_spi_master_init(CONF_SPI, &spi_periph_opt));

	struct spi_device spi_device_ext_0 = {
		.id = CONF_SPI_EXT_0_DEVICE_ID
	};

	spi_master_setup_device(CONF_SPI, &spi_device_ext_0, CONF_SPI_EXT_0_MODE, CONF_SPI_EXT_0_BAUDRATE, 0);
	spi_set_bits_per_transfer(CONF_SPI, spi_device_ext_0.id, CONF_SPI_EXT_0_BITS_PER_TRANSFER);

	struct spi_device spi_device_ext_1 = {
		.id = CONF_SPI_EXT_1_DEVICE_ID
	};

	spi_master_setup_device(CONF_SPI, &spi_device_ext_1, CONF_SPI_EXT_1_MODE, CONF_SPI_EXT_1_BAUDRATE, 0);
	spi_set_bits_per_transfer(CONF_SPI, spi_device_ext_1.id, CONF_SPI_EXT_1_BITS_PER_TRANSFER);

#endif

#ifdef CONF_SFISH_ENABLE_FREERTOS_TWI
	// Initialize the TWI master driver.
	const freertos_peripheral_options_t twi_periph_opt = {
		.receive_buffer = NULL,						// Not used by the TWI driver
		.receive_buffer_size = 0,					// Not used by the TWI driver
		.interrupt_priority =configLIBRARY_LOWEST_INTERRUPT_PRIORITY,
		.operation_mode = TWI_I2C_MASTER,
		.options_flags = USE_RX_ACCESS_MUTEX | USE_TX_ACCESS_MUTEX | WAIT_RX_COMPLETE | WAIT_TX_COMPLETE
	};

	freertos_twi_master_init(CONF_TWI, &twi_periph_opt);
	twi_set_speed(CONF_TWI, CONF_TWI_SPEED, sysclk_get_cpu_hz());
#endif

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Initialize external components
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef CONF_SFISH_ENABLE_TEMP
	/* Initialize the MAX31725 temperature sensors */
	max31726_init();
#endif

#ifdef CONF_SFISH_ENABLE_FPGA
	/* Initialize the display */
	fpga_init();
#endif

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Create tasks
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	/* Create Task Queues */
	create_task_queues();

#ifdef CONF_SFISH_ENABLE_CLI_TASK
	/* Create CLI task */
	create_cli_task();
#endif

#ifdef CONF_SFISH_ENABLE_RS485_TASK
	/* Create RS485 task */
	create_rs485_task();
#endif

#ifdef CONF_SFISH_ENABLE_SENSORS_TASK
	/* Create Sensors task */
	create_sensor_task();
#endif

#ifdef CONF_SFISH_ENABLE_TIMER_TASK
	/* Create Timer task */
	create_timer_task(SOFTWARE_TIMER_RATE);
#endif

	printf("Starting all RTOS tasks\n");
	vTaskStartScheduler();	// This function call should never return

	printf("###ERROR: vTaskStartScheduler() failed\n");
	for( ;; );
}
