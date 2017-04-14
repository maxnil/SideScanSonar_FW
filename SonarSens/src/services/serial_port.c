/*
 * serial_port.c
 *
 * Created: 2016-09-25 10:11:40
 *  Author: Max
 */ 

#include <stdio.h>

#include "sensors/tmp006.h"
#include "services/compass.h"
#include "services/serial_port.h"
#include "services/temperature.h"
#include "services/tilt.h"

static comp_data_t comp_data_old = {.heading = 1000};
static temp_data_t temp_data_old = {.board_temp = 1000, .ir_temp = 1000};
static tilt_data_t tilt_data_old = {.pitch = 1000, .roll = 1000};

bool comp_data_changed(comp_data_t comp_data);
bool temp_data_changed(temp_data_t temp_data);
bool tilt_data_changed(tilt_data_t tilt_data);


bool comp_data_changed(comp_data_t comp_data) {
	bool status = false;
	
	if ((comp_data.heading >= comp_data_old.heading + 0.5) || (comp_data.heading <= comp_data_old.heading - 0.5)) {
		comp_data_old.heading = comp_data.heading;
		status = true;
	}
	return status;
}


bool temp_data_changed(temp_data_t temp_data) {
	bool status = false;

	if ((temp_data.board_temp >= temp_data_old.board_temp + 0.1) || (temp_data.board_temp <= temp_data_old.board_temp - 0.1)) {
		temp_data_old.board_temp = temp_data_old.board_temp;
		status = true;
	}

	if ((temp_data.ir_temp >= temp_data_old.ir_temp + 0.1) || (temp_data.ir_temp <= temp_data_old.ir_temp - 0.1)) {
		temp_data_old.ir_temp = temp_data_old.ir_temp;
		status = true;
	}
	return status;
}


bool tilt_data_changed(tilt_data_t tilt_data) {
	bool status = false;

	if ((tilt_data.pitch >= tilt_data_old.pitch + 0.1) || (tilt_data.pitch <= tilt_data_old.pitch - 0.1)) {
		tilt_data_old.pitch = tilt_data.pitch;
		status = true;
	}

	if ((tilt_data.roll >= tilt_data_old.roll + 0.1) || (tilt_data.roll <= tilt_data_old.roll - 0.1)) {
		tilt_data_old.roll = tilt_data.roll;
		status = true;
	}
	return status;
}

void send_data(bool override_check, comp_data_t comp_data, tilt_data_t tilt_data, temp_data_t temp_data, tmp006_data_t tmp006_data) {
//	printf("$ACC, %5d,%5d,%5d\n", acc_data.x, acc_data.y, acc_data.z);
//	printf("$GYRO,%5d,%5d,%5d\n", gyro_data.x, gyro_data.y, gyro_data.z);
//	printf("$MAGN,%5d,%5d,%5d\n", magn_data.x, magn_data.y, magn_data.y);
//	printf("$IR  ,%5d,%5d\n", tmp006_data.t_amb, tmp006_data.v_obj);

	/* Send Compass data */
	if (comp_data_changed(comp_data) || override_check)
		printf("$COMP,% #3.1f\n", comp_data.heading);
	
	/* Send Temperature data */
//	if (temp_data_changed(temp_data) || override_check)
//		printf("$TEMP,% #2.1f,% 2.1f\n", temp_data.board_temp, temp_data.ir_temp);
	
	/* Send Tilt data */
//	if (tilt_data_changed(tilt_data) || override_check)
//		printf("$TILT,% #2.1f,% #3.1f\n", tilt_data.pitch, tilt_data.roll);
}