/*
 * tilt.c
 *
 * Created: 2016-09-11 01:24:44
 *  Author: Max
 */ 

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <math.h>
#include "tilt.h"
#include "sensors/lis3dsh.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*******************************************************************************
 * Get tilt
 */
uint8_t tilt_calc(acc_data_t acc_data, tilt_data_t *tilt_data) {
	double pitch;
	double roll;
	
	roll = -atan2(acc_data.y, acc_data.z);
	pitch = atan(acc_data.x/(acc_data.y*sin(roll) + acc_data.z*cos(roll)));
	                
	// Convert from radians to degrees
	tilt_data->pitch = pitch * (180.0 * M_1_PI);	// 180/pi
	tilt_data->roll = roll * (180.0 * M_1_PI);		// 180/pi
	return 0;
}
