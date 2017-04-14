/*
 * timer_task.h
 *
 * Created: 2016-02-10 22:56:47
 *  Author: Max
 */ 


#ifndef TIMER_TASK_H_
#define TIMER_TASK_H_

#define USB_CONNECTED_LED    LED0_GPIO
#define USB_ERROR_LED LED1_GPIO

void create_timer_task(const TickType_t xTimerPeriodInTicks);

#endif /* TIMER_TASK_H_ */