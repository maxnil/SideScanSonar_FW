/*
 * max31725.h
 *
 * Created: 2013-12-17 22:43:07
 *  Author: Max
 */ 


#ifndef MAX31725_H_
#define MAX31725_H_

#include "asf.h"

/* MAX31725 data struct */
typedef struct max31725_data_t
{
	int16_t		temp;
	int			err;
} max31725_data_t;


/* MAX31725 configuration struct */
typedef struct max31725_conf_t
{
	uint8_t		conf;
	int16_t		thyst;
	int16_t		tos;
	int			err;
} max31725_conf_t;


/* Initialize MAX31725 temperature sensor */
int8_t max31725_init(max31725_conf_t *conf);


/* Get data from MAX31725 */
int8_t max31725_get_data(max31725_data_t *data);

#endif /* MAX31725_H_ */