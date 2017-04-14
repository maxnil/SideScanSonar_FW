/*
 * temperature.h
 *
 * Created: 2016-09-12 23:27:39
 *  Author: Max
 */ 


#ifndef TEMPERATURE_H_
#define TEMPERATURE_H_

#include <stdint.h>
#include "sensors/max31725.h"
#include "sensors/tmp006.h"

typedef struct temp_data_t {
	double board_temp;
	double ir_temp;
} temp_data_t;

bool temp_calc(max31725_data_t max31725_data, tmp006_data_t tmp006_data, temp_data_t *temp_data);

#endif /* TEMPERATURE_H_ */