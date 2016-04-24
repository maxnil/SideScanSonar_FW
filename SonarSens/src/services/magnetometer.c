/*
 * magnetometer.c
 *
 * Created: 2016-04-18 18:43:03
 *  Author: Max
 */ 

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <math.h>
#include <stdio.h>
#include "sensors/hmc5983.h"
#include "magnetometer.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*******************************************************************************
 * Get magnetometer data
 */
int8_t get_magnetometer_data(magnetometer_t *mag_data) {
	int8_t err;
	hmc5983_data_t hmc5983_data;
	
	/* Get raw magnetic data from magnetometer */
	if ((err = hmc5983_get_data(&hmc5983_data))) {
		mag_data->strength = 10*10;
		mag_data->pitch = 90*10;
		mag_data->roll = 45*10;
		mag_data->yaw = 60*10;
		mag_data->heading = 15*10;
		printf("Error!!!!!!!!!!!!!!!!");
		return err;
	}

	/* Calculate magnetic field direction */
	mag_data->strength	= (int16_t)(sqrt(
		(double)hmc5983_data.x*(double)hmc5983_data.x +
		(double)hmc5983_data.y*(double)hmc5983_data.y +
		(double)hmc5983_data.z*(double)hmc5983_data.z));
	mag_data->pitch		= (int16_t)(10.0*(180.0 * atan2((double)hmc5983_data.x, (double)hmc5983_data.z) / M_PI));
	mag_data->roll		= (int16_t)(10.0*(180.0 * atan2((double)hmc5983_data.y, (double)hmc5983_data.z) / M_PI));
	mag_data->yaw		= (int16_t)(10.0*(180.0 * atan2((double)hmc5983_data.x, (double)hmc5983_data.y) / M_PI));

	/* Tilt compensated heading (NOT YET IMPLEMENTED!) */
	mag_data->heading	= (int16_t)(10.0*(180.0 - (180.0 * atan2((double)hmc5983_data.x, (double)hmc5983_data.y) / M_PI)));

	return 0;
}