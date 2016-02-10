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
#include "task.h"
#include "queue.h"
#include "timers.h"

#include "timer_task.h"
#include "USB_CDC_tasks.h"

/* The priorities at which various tasks will get created. */
#define CDC_CLI_TASK_PRIORITY		(tskIDLE_PRIORITY + 1)
#define CDC_SONAR_TASK_PRIORITY		(tskIDLE_PRIORITY + 1)
//#define mainUSART_ECHO_TASK_PRIORITY	(tskIDLE_PRIORITY)
//Max #define mainSPI_FLASH_TASK_PRIORITY		(tskIDLE_PRIORITY)
//Max #define mainTWI_EEPROM_TASK_PRIORITY	(tskIDLE_PRIORITY)

/* The stack sizes allocated to the various tasks. */
//Max #define mainUART_CLI_TASK_STACK_SIZE    (configMINIMAL_STACK_SIZE * 2)
//Max #define mainUSART_CLI_TASK_STACK_SIZE   (configMINIMAL_STACK_SIZE * 2)
#define CDC_CLI_TASK_STACK_SIZE     (configMINIMAL_STACK_SIZE * 2)
#define CDC_SONAR_TASK_STACK_SIZE   (configMINIMAL_STACK_SIZE * 2)
//Max #define mainUSART_ECHO_TASK_STACK_SIZE  (configMINIMAL_STACK_SIZE)
//Max #define mainSPI_FLASH_TASK_STACK_SIZE   (configMINIMAL_STACK_SIZE * 2)
//Max #define mainTWI_EEPROM_TASK_STACK_SIZE  (configMINIMAL_STACK_SIZE * 2)
#define SOFTWARE_TIMER_RATE			(200 / portTICK_PERIOD_MS)

#define DBG_WELCOME_HEADER \
"------------------------------------\r\n" \
"-- SonarCom_RTOS 2016 " SW_VERSION "        --\r\n" \
"-- Compiled: "__DATE__" "__TIME__" --\r\n" \
"------------------------------------\r\n" \
"--  ## DEBUG OUTPUT INTERFACE ##  --\r\n" \
"------------------------------------\r\n"


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
	scom_debug_console_init();			// Initialize Debug Console
#endif
#ifdef CONF_SCOM_ENABLE_ANALOG
	scom_analog_init();					// Initialize Analog Front End (ADC)
#endif
#ifdef CONF_SCOM_ENABLE_SPI
	scom_spi_init();					// Initialize SPI
#endif
#ifdef CONF_SCOM_ENABLE_TIMER
	scom_timer_init();					// Initialize Timers
#endif
#ifdef CONF_SCOM_ENABLE_TWI
	scom_twi_init();					// Initialize TWI/I2C
#endif
#ifdef CONF_SCOM_ENABLE_SSD1322
	ssd1322_init();						// Initialize the display
#endif
#ifdef CONF_SCOM_ENABLE_RTCC
	rtcc_init();						// Initialize the Real Time Clock(s)
#endif
#ifdef CONF_SCOM_ENABLE_RS485
	rs485_init();						// Initialize RS485 USART
#endif

	/* Print welcome message on Debug UART */
	printf(DBG_WELCOME_HEADER);

	/* Create Timer task */
	create_timer_task(SOFTWARE_TIMER_RATE);

	/* Create USB CDC tasks */
	create_usb_cdc_tasks(CDC_CLI_TASK_STACK_SIZE, CDC_CLI_TASK_PRIORITY,
						 CDC_SONAR_TASK_STACK_SIZE, CDC_SONAR_TASK_PRIORITY);
	
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

