/*
 * sensors.h
 *
 * Created: 2013-12-29 11:32:40
 *  Author: Max
 */ 


#ifndef SENSORS_H_
#define SENSORS_H_

#include "sensors/max31725.h"
#include "sensors/tmp006.h"

/* MAX31725 configuration */
extern max31725_conf_t max31725_conf;

/* TMP006 configuration */
extern tmp006_conf_t tmp006_conf;

/* eCompas data structure */
typedef struct {
	double heading;					// 0=North, 90=East, 180=South, 270=West
	double strength;				// Magnetic field strength (Gauss)
	double roll;					// Magnetic field Roll
	double pitch;					// Magnetic field Pitch
	double yaw;						// Magnetig field Yaw
} magnetometer_t;

// Initialize all Sensors
int8_t sensors_init(void);

// Get Accelerometer data
double get_accelerometer_data(void);

// Get Magnetic eCompass data
int8_t get_magnetometer_data(magnetometer_t *data);

#endif /* SENSORS_H_ */