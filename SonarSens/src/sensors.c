/*
 * sensors.c
 *
 * Created: 2013-12-29 11:32:27
 *  Author: Max
 */

#include <math.h>

#include "conf_board.h"
#include "sensors.h"
#include "sensors/hmc5983.h"
#include "sensors/lis3dsh.h"
#include "sensors/l3gd20.h"
#include "sensors/max31725.h"
#include "sensors/tmp006.h"

/* MAX31725 configuration */
max31725_conf_t max31725_conf = {-1, -1, -1, -1};

/* TMP006 configuration */
tmp006_conf_t tmp006_conf;


int8_t sensors_init(void)
{
	/* Initialize Accelerometer */
	lis3dsh_init(ACCELEROMETER_SS);

	/* Initialize eCompass */
	hmc5983_init(ECOMPASS_SS);
	
	/* Initialize Gyro */
	l3gd20_init(GYRO_SS);
	
	/* Initialize Board Temperature Sensor */
	max31725_init(&max31725_conf);

	/* Initialize IR Temperature Sensor */
	tmp006_init(&tmp006_conf);
	
	return 0;
}

int8_t get_magnetometer_data(magnetometer_t *mag_data)
{
	int8_t err;
	hmc5983_data_t hmc5983_data;
	
	/* Get raw magnetic data from magnetometer */
	if ((err = hmc5983_get_data(&hmc5983_data)))
		return err;

	/* Calculate magnetic field direction */
	mag_data->strength	= sqrt((double)hmc5983_data.x*(double)hmc5983_data.x +
							   (double)hmc5983_data.y*(double)hmc5983_data.y +
							   (double)hmc5983_data.z*(double)hmc5983_data.z);
	mag_data->pitch		= 180.0 * atan2((double)hmc5983_data.x, (double)hmc5983_data.z) / M_PI;
	mag_data->roll		= 180.0 * atan2((double)hmc5983_data.y, (double)hmc5983_data.z) / M_PI;
	mag_data->yaw		= 180.0 * atan2((double)hmc5983_data.x, (double)hmc5983_data.y) / M_PI;

	/* Tilt compensated heading (NOT YET IMPLEMENTED!) */
	mag_data->heading	= 180.0 - (180.0 * atan2((double)hmc5983_data.x, (double)hmc5983_data.y) / M_PI);

	return 0;
}