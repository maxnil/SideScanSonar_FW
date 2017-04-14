/*
 * serial_port.h
 *
 * Created: 2016-09-25 10:11:53
 *  Author: Max
 */ 


#ifndef SERIAL_PORT_H_
#define SERIAL_PORT_H_

#include "sensors/tmp006.h"
#include "services/compass.h"
#include "services/temperature.h"
#include "services/tilt.h"

void send_data(bool override_check, comp_data_t comp_data, tilt_data_t tilt_data, temp_data_t temp_data, tmp006_data_t tmp006_data);

#endif /* SERIAL_PORT_H_ */