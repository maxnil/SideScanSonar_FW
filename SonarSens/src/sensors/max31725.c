/*
 * max31725.c
 *
 * Created: 2013-12-17 22:43:39
 *  Author: Max
 */ 

#include "asf.h"
#include "max31725.h"
#include "drivers/twi_master.h"

#define SLA 0x90

#define TEMP	0x00
#define CONF	0x01
#define THYST	0x02
#define TOS		0x03

/* Initialize MAX31725 temperature sensor */
int8_t max31725_init(max31725_conf_t *conf)
{
	int err;
	uint8_t buf[6];
	
	/* Get current configuration values */
	err = twi_read(SLA, CONF, 6, buf);
	if (err) {
		conf->conf  = -1;
		conf->thyst = -1;
		conf->thyst = -1;
		conf->err   = err;
		return -1;
	} else {
		conf->conf  = buf[0];
		conf->thyst = (buf[2]<<8) | buf[3];
		conf->thyst = (buf[4]<<8) | buf[5];
		conf->err   = err;
	}
	return 0;
}


/* Get data from MAX31725 */
int8_t max31725_get_data(max31725_data_t *data)
{
	int8_t err;
	uint8_t buf[2];
	
	/* Get current data values */
	err = twi_read(SLA, TEMP, 2, buf);
	if (err) {
		data->temp = -1;
		data->err  = err;
	} else {
		data->temp = (buf[0]<<8) | buf[1];
		data->err  = 0;
	}
	return err;
}
