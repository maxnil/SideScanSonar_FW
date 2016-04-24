/*
 * magnetometer.h
 *
 * Created: 2016-04-18 18:42:48
 *  Author: Max
 */ 


#ifndef MAGNETOMETER_H_
#define MAGNETOMETER_H_

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "sensors/hmc5983.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* eCompass data structure */
typedef struct {
	int16_t heading;				// 0=North, 90=East, 180=South, 270=West
	uint16_t strength;				// Magnetic field strength (Gauss)
	int16_t roll;					// Magnetic field Roll
	int16_t pitch;					// Magnetic field Pitch
	int16_t yaw;					// Magnetic field Yaw
} magnetometer_t;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* Get magnetometer data */
int8_t get_magnetometer_data(magnetometer_t *mag_data);

#endif /* MAGNETOMETER_H_ */