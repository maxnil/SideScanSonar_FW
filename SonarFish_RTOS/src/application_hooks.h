/*
 * application_hooks.h
 *
 * Created: 2016-03-06 20:48:23
 *  Author: Max
 */ 


#ifndef APPLICATION_HOOKS_H_
#define APPLICATION_HOOKS_H_

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"

void vApplicationMallocFailedHook(void);

void vApplicationIdleHook(void);

void vApplicationStackOverflowHook(TaskHandle_t pxTask, signed char *pcTaskName);

void vApplicationTickHook(void);

//void vAssertCalled(uint32_t ulLine, const char *pcFile);

#endif /* APPLICATION_HOOKS_H_ */