/*
 * temperature.c
 *
 * Created: 2016-09-12 23:27:51
 *  Author: Max
 */ 

#include <math.h>
#include "temperature.h"
#include "sensors/max31725.h"
#include "sensors/tmp006.h"

/* TMP006 constants */
#define S0		 6.0e-14
#define A1		 1.75e-3
#define A2		-1.678e-5
#define TREF   298.15
#define B0		-2.94e-5
#define B1		-5.7e-7
#define B2		 4.63e-9
#define C2		13.4


bool temp_calc(max31725_data_t max31725_data, tmp006_data_t tmp006_data, temp_data_t *temp_data) {
	double S, Vos, FVobj;
	double Tobj;
	double Vobj;
	double Tdie;

	temp_data->board_temp = (double)max31725_data.temp / 256.0;
	
	Vobj = tmp006_data.v_obj*156.25e-9;
	Tdie = (double)tmp006_data.t_amb * 0.0078125 + 273.15;
	
	S = S0 * (1.0 + A1 * (Tdie - TREF) + A2 * (Tdie - TREF) * (Tdie - TREF));
	
	Vos = B0 + B1 * (Tdie - TREF) + B2 * (Tdie - TREF) * (Tdie - TREF);

	FVobj = (Vobj - Vos) + C2 * (Vobj - Vos) * (Vobj - Vos);
	
	Tobj = pow((Tdie * Tdie * Tdie * Tdie) + (FVobj / S), 0.25);
	
//	return Tobj - 273.15;
	return true;
}