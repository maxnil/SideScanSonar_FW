/*
 * timer_task.h
 *
 * Created: 2016-02-10 22:56:47
 *  Author: Max
 */ 


#ifndef TIMER_TASK_H_
#define TIMER_TASK_H_

/* Defines the LED toggled to provide visual feedback that the system is
 * running.  The rate is defined in milliseconds, then converted to RTOS ticks
 * by the portTICK_RATE_MS constant. */
#define mainSOFTWARE_TIMER_LED			LED0_GPIO

void create_timer_task(const TickType_t xTimerPeriodInTicks);

#endif /* TIMER_TASK_H_ */