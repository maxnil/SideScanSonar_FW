/*
 * timer_task.c
 *
 * Created: 2016-02-10 22:56:57
 *  Author: Max
 */ 

/* Drivers */
#include "led.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include "timer_task.h"

static TimerHandle_t xLEDTimer;


/*
 * The callback function used by the software timer.  See the comments at the
 * top of this file.
 */
static void LEDTimerCallback(void *pvParameters) {
	/* Toggle an LED to show the system is executing. */
	LED_Toggle(SOFTWARE_TIMER_LED);
}

void create_timer_task(const TickType_t xTimerPeriodInTicks) {

	/* Create the timer that toggles an LED to show that the system is running,
	and that the other tasks are behaving as expected. */
	xLEDTimer = xTimerCreate((const char * const) "LED timer",/* A text name, purely to help debugging. */
							xTimerPeriodInTicks,		/* The timer period. */
							pdTRUE,						/* This is an auto-reload timer, so xAutoReload is set to pdTRUE. */
							NULL,						/* The timer does not use its ID, so the ID is just set to NULL. */
							LEDTimerCallback			/* The function that is called each time the timer expires. */
							);

	/* Sanity check the timer's creation, then start the timer.  The timer
	will not actually start until the FreeRTOS kernel is started. */
	configASSERT(xLEDTimer);
	xTimerStart(xLEDTimer, 0);
}