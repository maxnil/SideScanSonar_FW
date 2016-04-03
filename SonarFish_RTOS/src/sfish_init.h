/*
 * sfish_init.h
 *
 * Created: 2016-04-03 13:07:48
 *  Author: Max
 */ 


#ifndef SFISH_INIT_H_
#define SFISH_INIT_H_

/* Analog Input initialization */
void sfish_analog_init(void);

/* Debug Console initialization */
void sfish_debug_console_init(void);

/* Get Chip temperature */
int32_t get_chip_temperature(void);

/* Get analog input value */
int32_t get_analog_input(int adc_nr);

#endif /* SFISH_INIT_H_ */