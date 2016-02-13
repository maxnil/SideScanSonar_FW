/*
 * scom_init.h
 *
 * Created: 2015-01-04 21:56:37
 *  Author: Max
 */ 


#ifndef SCOM_INIT_H_
#define SCOM_INIT_H_

/* Analog Input initialization */
void scom_analog_init(void);

/* Debug Console initialization */
void scom_debug_console_init(void);

/* SPI initialization */
void scom_spi_init(void);

/* TWI/I2C initialization */
void scom_twi_init(void);

/* Enable Sonar Power */
void sonar_power_enable(Bool sonar_enable);

/* Check Sonar Power status */
Bool sonar_power_is_enabled(void);

/* Get Chip temperature */
int32_t get_chip_temperature(void);

/* Get analog input value */
int32_t get_analog_input(int adc_nr);

#endif /* SCOM_INIT_H_ */