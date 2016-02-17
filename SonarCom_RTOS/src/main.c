/**
 * main routine for SonarCom_RTOS
 */

#include <asf.h>
#include "scom_init.h"

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
#include "timer_task.h"
#include "USB_CDC_tasks.h"

/* The priorities at which various tasks will get created. */
#define CDC_CLI_TASK_PRIORITY		(tskIDLE_PRIORITY + 1)
#define CDC_SONAR_TASK_PRIORITY		(tskIDLE_PRIORITY + 1)
#define GPS_TASK_PRIORITY			(tskIDLE_PRIORITY + 1)

/* The stack sizes allocated to the various tasks. */
#define CDC_CLI_TASK_STACK_SIZE     (configMINIMAL_STACK_SIZE * 2)
#define CDC_SONAR_TASK_STACK_SIZE   (configMINIMAL_STACK_SIZE * 2)
#define GPS_TASK_STACK_SIZE			(configMINIMAL_STACK_SIZE * 4)

#define SOFTWARE_TIMER_RATE			(200 / portTICK_PERIOD_MS)

#define DBG_WELCOME_HEADER \
"------------------------------------\r\n" \
"-- SonarCom_RTOS 2016 " SW_VERSION "        --\r\n" \
"-- Compiled: "__DATE__" "__TIME__" --\r\n" \
"------------------------------------\r\n" \
"--  ## DEBUG OUTPUT INTERFACE ##  --\r\n" \
"------------------------------------\r\n"

#define RS485_RX_BUFFER_SIZE 128

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

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Initialize components and interfaces
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef CONF_SCOM_ENABLE_DEBUG_CONSOLE
	/* Initialize Debug Console */
	scom_debug_console_init();
	printf(DBG_WELCOME_HEADER);			// Print welcome message on Debug UART
#endif

#ifdef CONF_SCOM_ENABLE_ANALOG
	/* Initialize Analog Front End (ADC) */
	scom_analog_init();
#endif

#ifdef CONF_SCOM_ENABLE_FREERTOS_GPS
	/* Initialize FreeRTOS GPS USART driver */
	uint8_t *gps_rx_buffer;
	if ((gps_rx_buffer = (uint8_t*)pvPortMalloc(GPS_RX_BUFFER_SIZE)) == NULL) {
		printf("###ERROR malloc failed for gps_rx_buffer\n");
		for(;;);
	}
	
	freertos_peripheral_options_t gps_periph_opt = {
		.receive_buffer = gps_rx_buffer,
		.receive_buffer_size = GPS_RX_BUFFER_SIZE,
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
	
	if (freertos_usart_serial_init(CONF_GPS_USART, &gps_usart_opt, &gps_periph_opt) == NULL) {
		printf("###ERROR freertos_usart_serial_init failed\n");
	}
#endif

#ifdef CONF_SCOM_ENABLE_FREERTOS_RS485
	/* Initialize FreeRTOS RS485 USART driver */
	uint8_t *rs485_rx_buffer;
	if ((rs485_rx_buffer = (uint8_t*)pvPortMalloc(RS485_RX_BUFFER_SIZE)) == NULL) {
		printf("###ERROR malloc failed for rs485_rx_buffer\n");
		for(;;);
	}
	
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

	if (freertos_usart_serial_init(CONF_RS485_USART, &rs485_usart_opt, &rs485_periph_opt) == NULL) {
		printf("###ERROR freertos_usart_serial_init failed\n");
	}
#endif


#ifdef CONF_SCOM_ENABLE_FREERTOS_SPI
	/* Initialize FreeRTOS SPI driver */
	freertos_peripheral_options_t spi_periph_opt;

	spi_periph_opt.receive_buffer = NULL;						// Not used by the SPI driver
	spi_periph_opt.receive_buffer_size = 0;					// Not used by the SPI driver
	spi_periph_opt.interrupt_priority = configLIBRARY_LOWEST_INTERRUPT_PRIORITY;
	spi_periph_opt.operation_mode = SPI_MASTER;
	spi_periph_opt.options_flags = WAIT_RX_COMPLETE | WAIT_TX_COMPLETE;

	freertos_spi_master_init(CONF_SPI, &spi_periph_opt);
#else
#ifdef CONF_SCOM_ENABLE_SPI
	/* Initialize SPI */
	scom_spi_init();
#endif
#endif

#ifdef CONF_SCOM_ENABLE_FREERTOS_TIMER
	/* Create Timer tasks */
	create_timer_task(SOFTWARE_TIMER_RATE);
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

#ifdef CONF_SCOM_ENABLE_SSD1322
	/* Initialize the display */
	ssd1322_init();	
#endif

#ifdef CONF_SCOM_ENABLE_RTCC
	/* Initialize the Real Time Clocks (internal and external) */
	rtcc_init();
#endif

#ifdef CONF_SCOM_ENABLE_RS485
	/* Initialize RS485 USART */
	rs485_init();
#endif

#ifdef CONF_SCOM_ENABLE_USB_CDC_TASK
	/* Create USB CDC tasks */
	create_usb_cdc_tasks(CDC_CLI_TASK_STACK_SIZE, CDC_CLI_TASK_PRIORITY,
						 CDC_SONAR_TASK_STACK_SIZE, CDC_SONAR_TASK_PRIORITY);
#endif

#ifdef CONF_SCOM_ENABLE_GPS_TASK
	/* Create GPS task */
	create_gps_task(GPS_TASK_STACK_SIZE, GPS_TASK_PRIORITY);
#endif

	LED_On(LED2_GPIO);

	printf("Starting all RTOS tasks\n");
	vTaskStartScheduler();
	
	/* If all is well, the scheduler will now be running, and the following
	line will never be reached.  If the following line does execute, then
	there was insufficient FreeRTOS heap memory available for the idle and/or
	timer tasks	to be created.  See the memory management section on the
	FreeRTOS web site for more details. */
	printf("###ERROR: vTaskStartScheduler() failed\n");
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook(void);

void vApplicationMallocFailedHook(void)
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	printf("###ERROR: vApplicationMallocFailedHook() failed\n");
	for (;;) {
	}
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook(void);

void vApplicationIdleHook(void)
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(TaskHandle_t pxTask, signed char *pcTaskName);

void vApplicationStackOverflowHook(TaskHandle_t pxTask, signed char *pcTaskName)
{
	(void) pcTaskName;
	(void) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	printf("###ERROR: vApplicationStackOverflowHook() %s\n", pcTaskName);
	for (;;) {
	}
}
/*-----------------------------------------------------------*/

void vApplicationTickHook(void);

void vApplicationTickHook(void)
{
	/* This function will be called by each tick interrupt if
	configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
	added here, but the tick hook is called from an interrupt context, so
	code must not attempt to block, and only the interrupt safe FreeRTOS API
	functions can be used (those that end in FromISR()). */
}
/*-----------------------------------------------------------*/

void vAssertCalled(uint32_t ulLine, const char *pcFile)
{
	/* The following two variables are just to ensure the parameters are not
	optimized away and therefore unavailable when viewed in the debugger. */
	volatile uint32_t ulLineNumber = ulLine, ulSetNonZeroInDebuggerToReturn = 1;
	volatile const char * const pcFileName = pcFile;

	taskENTER_CRITICAL();
	printf("Assert: %d %s\n", (int)ulLineNumber, pcFileName);

	while( ulSetNonZeroInDebuggerToReturn == 0 )
	{
		/* If you want to set out of this function in the debugger to see the
		assert() location then set ulSetNonZeroInDebuggerToReturn to a non-zero
		value. */
	}
	taskEXIT_CRITICAL();

	( void ) pcFileName;
	( void ) ulLineNumber;
}
/*-----------------------------------------------------------*/

