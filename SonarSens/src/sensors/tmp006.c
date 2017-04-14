/*
 * tmp006.c
 *
 * Created: 2013-12-17 22:42:40
 *  Author: Max
 */ 

#include <math.h>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "asf.h"
#include "tmp006.h"
#include "drivers/twi_master.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* TMP006 I2C address */
#define SLA 0x80

/* TMP006 registers */
#define V_OBJ	0x00
#define T_AMB	0x01
#define CONF	0x02
#define MAN_ID	0xFE
#define DEV_ID	0xFF


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL VARIABLES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

tmp006_conf_t tmp006_conf = {-1, -1, -1};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*******************************************************************************
 * Initialize TMP006 temperature sensor
 */
int8_t tmp006_init(void) {
	uint16_t err;
	uint8_t buf[2];

	/* Get current configuration values */
	err = twi_read(SLA, CONF, 2, buf);
	if (err)
		return -1;

	tmp006_conf.conf  = (buf[0]<<8) | buf[1];

	err = twi_read(SLA, MAN_ID, 2, buf);
	if (err)
		return -1;

	tmp006_conf.man_id = (buf[0]<<8) | buf[1];

	err = twi_read(SLA, DEV_ID, 2, buf);
	if (err)
		return -1;

	tmp006_conf.dev_id = (buf[0]<<8) | buf[1];
	
	// CR = '010', DRDY
	buf[0] = 0x70 | (2<<1) | (1<<0);
	buf[1] = 0x00;
		
	err = twi_write(SLA, CONF, 2, buf);

	return err;
}


/*******************************************************************************
 * Get data from TMP006
 */
bool tmp006_get_data(tmp006_data_t *data) {
	uint16_t err;
	uint8_t buf[2];
	
	/* Get current vObj value */
	err = twi_read(SLA, V_OBJ, 2, buf);
	if (err > 0)
		return false;

	data->v_obj = (buf[0]<<8) | buf[1];
		
	/* Get current tAmb value */
	err = twi_read(SLA, T_AMB, 2, buf);
	if (err) {
		return false;
	}
	
	data->t_amb = (buf[0]<<8) | buf[1];
	return true;
}

