/*
 * filter.c
 *
 * Created: 2016-09-25 20:38:24
 *  Author: Max
 */ 

#include <stdio.h>

#include "sensors/hmc5983.h"
#include "sensors/lis3dsh.h"
#include "services/compass.h"
#include "services/filter.h"
#include "services/tilt.h"

static acc_data_t acc_filter_array[ACC_FILTER_LEN];
static magn_data_t magn_filter_array[MAGN_FILTER_LEN];

static int acc_data_ptr;
static int magn_data_ptr;

void filter_init() {
	int i;
	
	/* Initialize Accelerator data filter */
	for (i = 0; i < ACC_FILTER_LEN; i++) {
		acc_filter_array[i].x = 0.0;
		acc_filter_array[i].y = 0.0;
		acc_filter_array[i].z = 0.0;
	}
	acc_data_ptr = 0;

	/* Initialize Magnetometer data filter */
	for (i = 0; i < MAGN_FILTER_LEN; i++) {
		magn_filter_array[i].x = 0.0;
		magn_filter_array[i].y = 0.0;
		magn_filter_array[i].z = 0.0;
	}
	magn_data_ptr = 0;
}


void acc_filter(acc_data_t *acc_data) {
	int i;
	double x = 0.0;
	double y = 0.0;
	double z = 0.0;
	
	acc_filter_array[acc_data_ptr].x = acc_data->x;
	acc_filter_array[acc_data_ptr].y = acc_data->y;
	acc_filter_array[acc_data_ptr].z = acc_data->z;
	
	for (i = 0; i < ACC_FILTER_LEN; i++) {
		x += acc_filter_array[i].x;
		y += acc_filter_array[i].y;
		z += acc_filter_array[i].z;
	}
	
	acc_data->x = x/ACC_FILTER_LEN;
	acc_data->y = y/ACC_FILTER_LEN;
	acc_data->z = z/ACC_FILTER_LEN;
	
	acc_data_ptr++;
	if (acc_data_ptr >= ACC_FILTER_LEN)
		acc_data_ptr = 0;
}


void magn_filter(magn_data_t *magn_data) {
	int i;
	double x = 0.0;
	double y = 0.0;
	double z = 0.0;
		
	magn_filter_array[magn_data_ptr].x = magn_data->x;
	magn_filter_array[magn_data_ptr].y = magn_data->y;
	magn_filter_array[magn_data_ptr].z = magn_data->z;
		
	for (i = 0; i < MAGN_FILTER_LEN; i++) {
		x += magn_filter_array[i].x;
		y += magn_filter_array[i].y;
		z += magn_filter_array[i].z;
	}
		
	magn_data->x = x/MAGN_FILTER_LEN;
	magn_data->y = y/MAGN_FILTER_LEN;
	magn_data->z = z/MAGN_FILTER_LEN;
		
	magn_data_ptr++;
	if (magn_data_ptr >= MAGN_FILTER_LEN)
		magn_data_ptr = 0;

}
