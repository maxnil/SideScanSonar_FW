/*
 * max31725.h
 *
 * Created: 2013-12-17 22:43:07
 *  Author: Max
 */ 


#ifndef MAX31725_H_
#define MAX31725_H_

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "asf.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* MAX31725 configuration struct */
typedef struct max31725_conf_t {
	uint8_t		conf;
	int16_t		thyst;
	int16_t		tos;
} max31725_conf_t;


/* Initialize MAX31725 temperature sensor */
int8_t max31725_init(void);


/* Get temperature from MAX31725 */
int8_t max31725_get_temp(uint16_t *temp);

#endif /* MAX31725_H_ */