/*
 * tilt.h
 *
 * Created: 2016-09-11 01:24:57
 *  Author: Max
 */ 


#ifndef TILT_H_
#define TILT_H_

#include <stdint.h>
#include "sensors/lis3dsh.h"

typedef struct tilt_data_t {
	double roll;
	double pitch;
} tilt_data_t;

uint8_t tilt_calc(acc_data_t acc_data, tilt_data_t *tilt_data);

#endif /* TILT_H_ */