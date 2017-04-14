/*
 * compass.h
 *
 * Created: 2016-09-11 01:12:11
 *  Author: Max
 */ 


#ifndef COMPASS_H_
#define COMPASS_H_

#include <stdint.h>
#include "services/tilt.h"
#include "sensors/hmc5983.h"

/* Hard metal compensation */
#define VX 0.0
#define VY 0.0
#define VZ 0.0


typedef struct comp_data_t {
		double heading;
		double heading_comp;
} comp_data_t;

uint8_t comp_calc(tilt_data_t tilt_data, magn_data_t magn_data, comp_data_t *comp_data);

#endif /* COMPASS_H_ */