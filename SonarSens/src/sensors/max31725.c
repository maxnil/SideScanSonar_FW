/*
 * max31725.c
 *
 * Created: 2013-12-17 22:43:39
 *  Author: Max
 */ 

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "asf.h"
#include "max31725.h"
#include "drivers/twi_master.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define SLA		0x90

#define TEMP	0x00
#define CONF	0x01
#define THYST	0x02
#define TOS		0x03

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL VARIABLES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* MAX31725 configuration */
static max31725_conf_t max31725_conf = {-1, -1, -1, -1};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*******************************************************************************
 * Initialize MAX31725 temperature sensor
 */
int8_t max31725_init(void) {
	int err;
	uint8_t buf[6];
	
	/* Get current configuration values */
	err = twi_read(SLA, CONF, 6, buf);
	if (err) {
		max31725_conf.conf  = -1;
		max31725_conf.thyst = -1;
		max31725_conf.tos   = -1;
		return -1;
	} else {
		max31725_conf.conf  = buf[0];
		max31725_conf.thyst = (buf[2]<<8) | buf[3];
		max31725_conf.tos   = (buf[4]<<8) | buf[5];
	}
	return 0;
}


/*******************************************************************************
 * Get temperature from MAX31725
 */
int8_t max31725_get_temp(uint16_t *temp) {
	int8_t err;
	uint8_t buf[2];
	
	/* Get current data values */
	err = twi_read(SLA, TEMP, 2, buf);
	if (err) {
		*temp = -1;
	} else {
		*temp = (buf[0]<<8) | buf[1];
	}
	return err;
}
