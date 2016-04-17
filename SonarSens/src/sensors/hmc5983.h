/*
 * hmc5983.h
 *
 * Created: 2013-12-17 22:40:46
 *  Author: Max
 */ 


#ifndef HMC5983_H_
#define HMC5983_H_

#include <stdint.h>
#include "asf.h"

typedef struct hmc5983_data_t {
	int16_t x;
	int16_t y;
	int16_t z;
} hmc5983_data_t;

typedef struct hmc5983_id_t {
	int8_t a;
	int8_t b;
	int8_t c;
} hmc5983_id_t;

// Initialize HMC5983 eCompass
void hmc5983_init(port_pin_t ss_pin);

// Get temperature from HMC5983
double hmc5983_get_temp(void);

// Get Magnetic values
int8_t hmc5983_get_data(hmc5983_data_t *data);

// Get Device ID
int hmc5983_get_id(hmc5983_id_t *data);

#endif /* HMC5983_H_ */