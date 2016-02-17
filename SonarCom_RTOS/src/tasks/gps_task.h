/*
 * gps_task.h
 *
 * Created: 2016-02-14 15:37:43
 *  Author: Max
 */ 


#ifndef GPS_TASK_H_
#define GPS_TASK_H_

#define GPS_RX_BUFFER_SIZE 64

void create_gps_task(uint16_t stack_depth_words, unsigned portBASE_TYPE task_priority);

#endif /* GPS_TASK_H_ */