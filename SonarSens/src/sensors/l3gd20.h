/*
 * l3gd20.h
 *
 * Created: 2013-12-17 22:41:47
 *  Author: Max
 */ 


#ifndef L3GD20_H_
#define L3GD20_H_

typedef struct gyro_t
{
	int16_t x;
	int16_t y;
	int16_t z;
} gyro_t;

typedef struct l3gd20_id_t
{
	uint8_t who_am_i;
} l3gd20_id_t;

// Initialize L3GD20 Accelerometer
void l3gd20_init(port_pin_t ss_pin);

// Get Accelerometer values
int l3gd20_get_data(gyro_t *data);

// Get Device ID
int l3gd20_get_id(l3gd20_id_t *data);

int8_t l3gd20_get_temp(void);

#endif /* L3GD20_H_ */