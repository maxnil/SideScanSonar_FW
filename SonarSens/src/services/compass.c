/*
 * compass.c
 *
 * Created: 2016-09-11 01:12:25
 *  Author: Max
 */ 

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <math.h>
#include <stdio.h>
#include "compass.h"
#include "tilt.h"
#include "sensors/hmc5983.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*******************************************************************************
 * Calculate compass heading
 */
uint8_t comp_calc(tilt_data_t tilt_data, magn_data_t magn_data, comp_data_t *comp_data) {
	double cos_pitch;
	double sin_pitch;
	double cos_roll;
	double sin_roll;
	double heading;
	double heading_comp;

	cos_pitch = cos(tilt_data.pitch / (180.0 * M_1_PI));
	sin_pitch = sin(tilt_data.pitch / (180.0 * M_1_PI));
	cos_roll = cos(tilt_data.roll / (180.0 * M_1_PI));
	sin_roll = sin(tilt_data.roll / (180.0 * M_1_PI));
	
	heading_comp = atan2(((magn_data.z - VZ) * sin_roll) - ((magn_data.y - VY) * cos_roll),
						((magn_data.x - VX) * cos_pitch) + ((magn_data.y - VY) * sin_roll * sin_pitch) + ((magn_data.z - VZ) * sin_pitch * cos_roll));
					

	heading = atan2(-magn_data.y, magn_data.x);

	// Convert from radians to degrees
	if (heading < 0)
		comp_data->heading = heading * (180.0 * M_1_PI) + 360;				// 180/pi
	else
		comp_data->heading = heading * (180.0 * M_1_PI);					// 180/pi
	
	if (heading_comp < 0)
		comp_data->heading_comp = heading_comp * (180.0 * M_1_PI) + 360;	// 180/pi
	else
		comp_data->heading_comp = heading_comp * (180.0 * M_1_PI);			// 180/pi
	
	return 0;	
}

