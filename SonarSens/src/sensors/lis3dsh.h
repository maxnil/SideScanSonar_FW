/*
 * lis3dsh.h
 *
 * Created: 2013-12-17 22:42:07
 *  Author: Max
 */ 


#ifndef LIS3DSH_H_
#define LIS3DSH_H_

#include "asf.h"

typedef struct acc_data_t {
	int16_t x;
	int16_t y;
	int16_t z;
} acc_data_t;

typedef struct lis3dsh_id_t {
	uint8_t info1;
	uint8_t info2;
	uint8_t who_am_i;
} lis3dsh_id_t;

// Initialize LIS3DSH Accelerometer
void lis3dsh_init(port_pin_t ss_pin);

// Get Accelerometer values
int lis3dsh_get_data(acc_data_t *data);

// Get Device ID
int lis3dsh_get_id(lis3dsh_id_t *data);

int8_t lis3dsh_get_temp(void);

#endif /* LIS3DSH_H_ */