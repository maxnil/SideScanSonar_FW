/*
 * sfish_init.c
 *
 * Created: 2016-04-03 13:08:22
 *  Author: Max
 */ 

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <asf.h>
#include <string.h>

#include "sfish_init.h"
#include "led.h"

#include "conf_uart_serial.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define ADC_CLOCK   6400000

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*******************************************************************************
 * Initialize Analog inputs and internal Temperature Sensor
 */
void sfish_analog_init(void) {

	adc_init(ADC, sysclk_get_main_hz(), ADC_CLOCK, ADC_STARTUP_TIME_15);
	adc_configure_timing(ADC, 0, ADC_SETTLING_TIME_3, 1);
	adc_configure_trigger(ADC, 0, 1); // ADC_MR_FREERUN_ON
	adc_set_resolution(ADC, ADC_MR_LOWRES_BITS_12);
	adc_enable_channel(ADC, ADC_IF_0);
	adc_enable_channel(ADC, ADC_IF_1);
	adc_enable_ts(ADC);
	adc_enable_channel(ADC, ADC_TEMPERATURE_SENSOR);
	adc_start(ADC);
}


/*******************************************************************************
 * Initialize STDIO Debug Console
 */
void sfish_debug_console_init(void) {
	const usart_serial_options_t console_uart_options = {
		.baudrate     = CONF_DEBUG_UART_BAUDRATE,
		.paritytype   = CONF_DEBUG_UART_PARITY
	};

	stdio_serial_init(CONF_DEBUG_UART, &console_uart_options);
}


/*******************************************************************************
 * Get chip temperature (in Celsius)
 */
int get_chip_temperature(void) {
	int ul_mvolt;
	int ul_value;
	int ul_temp;

	ul_value = adc_get_channel_value(ADC, ADC_TEMPERATURE_SENSOR);
	
	ul_mvolt = ul_value * VOLT_REF / MAX_DIGITAL;

	/*
	 * According to datasheet, The output voltage VT = 1.44V at 27C
	 * and the temperature slope dVT/dT = 4.7 mV/C
	 */
	ul_temp = (((ul_mvolt - 1440)  * 100) / 470) + 27;
	return ul_temp;
}


/*******************************************************************************
 * Get analog voltage value
 */
int get_analog_input(int adc_nr) {
	int ul_mvolt;
	int ul_value;

	if (adc_nr >= BOARD_NR_ADC) {
		return 0;
	}
	
	ul_value = adc_get_channel_value(ADC, ADC_IF_0 + adc_nr);
	
	ul_mvolt = (ul_value * VOLT_REF) / MAX_DIGITAL;
	
	return ul_mvolt;
}
