/**
 * main routine for SonarCom_RTOS
 */

#include <asf.h>
#include "application_hooks.h"
#include "scom_init.h"
#include "scom.h"

/* Configuration */
#include "conf_sonarcom.h"
#include "conf_twi_master.h"

/* Drivers */
#include "led.h"
#include "rtcc.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "freertos_peripheral_control.h"
#include "freertos_spi_master.h"
#include "freertos_twi_master.h"
#include "freertos_usart_serial.h"
#include "task.h"

/* Tasks */
#include "gps_task.h"
#include "rs485_task.h"
#include "timer_task.h"
#include "USB_CDC_tasks.h"
#include "task_queues.h"

/* The priorities at which various tasks will get created. */
#define SOFTWARE_TIMER_RATE		(200 / portTICK_PERIOD_MS)

#define DBG_WELCOME_HEADER \
"------------------------------------\r\n" \
"-- SonarCom_RTOS 2016 " SW_VERSION "        --\r\n" \
"-- Compiled: "__DATE__" "__TIME__" --\r\n" \
"------------------------------------\r\n" \
"--  ## DEBUG OUTPUT INTERFACE ##  --\r\n" \
"------------------------------------\r\n"


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
	
	LED_On(LED3_GPIO);	// Turn on Red LED

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Initialize interfaces
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
#ifdef CONF_SCOM_ENABLE_ANALOG
	/* Initialize Analog Front End (ADC) */
	scom_analog_init();
#endif

#ifdef CONF_SCOM_ENABLE_DEBUG_CONSOLE
	/* Initialize Debug Console */
	scom_debug_console_init();
	printf(DBG_WELCOME_HEADER);			// Print welcome message on Debug UART
#endif

#ifdef CONF_SCOM_ENABLE_FREERTOS_GPS_USART
	/* Initialize FreeRTOS GPS USART driver */
	uint8_t *gps_rx_buffer;
	configASSERT(gps_rx_buffer = (uint8_t*)pvPortMalloc(GPS_MAX_SENTENCE_LENGTH));
	
	freertos_peripheral_options_t gps_periph_opt = {
		.receive_buffer = gps_rx_buffer,
		.receive_buffer_size = GPS_MAX_SENTENCE_LENGTH,
		.interrupt_priority = configLIBRARY_LOWEST_INTERRUPT_PRIORITY - 1,
		.operation_mode = USART_RS232,
		.options_flags = WAIT_RX_COMPLETE | WAIT_TX_COMPLETE
	};

	sam_usart_opt_t gps_usart_opt = {
		.baudrate = CONF_GPS_BAUDRATE,
		.char_length = US_MR_CHRL_8_BIT,
		.parity_type = US_MR_PAR_NO,
		.stop_bits = US_MR_NBSTOP_1_BIT,
		.channel_mode = US_MR_CHMODE_NORMAL,
		.irda_filter = 0
	};
	
	configASSERT(freertos_usart_serial_init(CONF_GPS_USART, &gps_usart_opt, &gps_periph_opt));
#endif

#ifdef CONF_SCOM_ENABLE_FREERTOS_RS485_USART
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
		.parity_type = US_MR_PAR_NO,
		.stop_bits = US_MR_NBSTOP_1_BIT
	};

	configASSERT(freertos_usart_serial_init(CONF_RS485_USART, &rs485_usart_opt, &rs485_periph_opt));
#else
#ifdef CONF_SCOM_ENABLE_RS485
	/* Initialize RS485 USART */
	rs485_init();
#endif
#endif

#ifdef CONF_SCOM_ENABLE_FREERTOS_SPI
	/* Initialize FreeRTOS SPI driver */
	freertos_peripheral_options_t spi_periph_opt = {
		.receive_buffer = NULL,						// Not used by the SPI driver
		.receive_buffer_size = 0,					// Not used by the SPI driver
		.interrupt_priority = configLIBRARY_LOWEST_INTERRUPT_PRIORITY,
		.operation_mode = SPI_MASTER,
		.options_flags = WAIT_RX_COMPLETE | WAIT_TX_COMPLETE
	};

	configASSERT(freertos_spi_master_init(CONF_SPI, &spi_periph_opt));
	
	struct spi_device spi_device_disp = {
		.id = CONF_SPI_DISP_DEVICE_ID
	};
	
	spi_master_setup_device(CONF_SPI, &spi_device_disp, CONF_SPI_DISP_MODE, CONF_SPI_DISP_BAUDRATE, 0);
	spi_set_bits_per_transfer(CONF_SPI, spi_device_disp.id, CONF_SPI_DISP_BITS_PER_TRANSFER);

	struct spi_device spi_device_ext = {
		.id = CONF_SPI_EXT_DEVICE_ID
	};

	spi_master_setup_device(CONF_SPI, &spi_device_ext, CONF_SPI_EXT_MODE, CONF_SPI_EXT_BAUDRATE, 0);
	spi_set_bits_per_transfer(CONF_SPI, spi_device_ext.id, CONF_SPI_EXT_BITS_PER_TRANSFER);
#else
#ifdef CONF_SCOM_ENABLE_SPI
	/* Initialize SPI */
	scom_spi_init();
#endif
#endif

#ifdef CONF_SCOM_ENABLE_FREERTOS_TWI
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
#else
#ifdef CONF_SCOM_ENABLE_TWI
	/* Initialize TWI/I2C */
	scom_twi_init();
#endif
#endif

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Initialize external components
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef CONF_SCOM_ENABLE_RTCC
	/* Initialize the Real Time Clocks (internal and external) */
	rtcc_init();
#endif

#ifdef CONF_SCOM_ENABLE_SSD1322
	/* Initialize the display */
	ssd1322_init();
#endif

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Create tasks
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	printf("FreeHeepSize 1: %d\n", xPortGetFreeHeapSize());
	
	/* Create Task Queues */
	create_task_queues();

#ifdef CONF_SCOM_ENABLE_GPS_TASK
	/* Create GPS task */
	create_gps_task();
#endif

#ifdef CONF_SCOM_ENABLE_RS485_TASK
	/* Create RS485 task */
	create_rs485_task();
#endif

#ifdef CONF_SCOM_ENABLE_TIMER_TASK
	/* Create Timer task */
	create_timer_task(SOFTWARE_TIMER_RATE);
#endif

#ifdef CONF_SCOM_ENABLE_USB_CDC_TASK
	/* Create USB CDC tasks */
	create_usb_cdc_tasks();
#endif

	LED_On(LED2_GPIO);	// Turn on Green LED
	LED_Off(LED3_GPIO);	// Turn off Red LED
	
	printf("FreeHeepSize 2: %d\n", xPortGetFreeHeapSize());


	printf("Starting all RTOS tasks\n");
	vTaskStartScheduler();	// This function call should never return
	
	printf("###ERROR: vTaskStartScheduler() failed\n");
	for( ;; );
}
