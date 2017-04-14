/*
 * tmp006.h
 *
 * Created: 2013-12-17 22:42:52
 *  Author: Max
 */ 


#ifndef TMP006_H_
#define TMP006_H_

#include "asf.h"

/* TMP006 data struct */
typedef struct tmp006_data_t {
	int16_t		v_obj;	// Sensor Voltage
	int16_t		t_amb;	// Temperature
} tmp006_data_t;


/* MAX31725 configuration struct */
typedef struct tmp006_conf_t {
	uint16_t	conf;
	uint16_t	man_id;	// Manufacture ID
	uint16_t	dev_id;	// Device ID
} tmp006_conf_t;


/* Initialize TMP006 temperature sensor */
int8_t tmp006_init(void);


/* Get data from TMP006 */
bool tmp006_get_data(tmp006_data_t *data);

#endif /* TMP006_H_ */