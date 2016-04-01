/*
 * scom_init.c
 *
 * Created: 2015-01-04 21:58:15
 *  Author: Max
 */ 

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <asf.h>
#include <string.h>

#include "scom_init.h"
#include "led.h"

#include "conf_afec.h"
#include "conf_spi_master.h"
#include "conf_twi_master.h"
#include "conf_uart_serial.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*******************************************************************************
 * Initialize Analog inputs and internal Temperature Sensor
 */
void scom_analog_init(void) {
	afec_enable(CONF_AFEC);

	struct afec_config afec_cfg;

	afec_get_config_defaults(&afec_cfg);
	afec_cfg.afec_clock = CONF_AFEC_CLOCK_FREQ;
	afec_init(CONF_AFEC, &afec_cfg);

	afec_set_trigger(CONF_AFEC, AFEC_TRIG_FREERUN);		// Keep the ADC running...
//	afec_set_trigger(CONF_AFEC, AFEC_TRIG_SW);		// SW trigger

	struct afec_ch_config afec_ch_cfg;
	afec_ch_get_config_defaults(&afec_ch_cfg);
	afec_ch_set_config(CONF_AFEC, AFEC_CHANNEL_0, &afec_ch_cfg);
	afec_ch_set_config(CONF_AFEC, AFEC_CHANNEL_1, &afec_ch_cfg);
	afec_ch_set_config(CONF_AFEC, AFEC_CHANNEL_2, &afec_ch_cfg);
	afec_ch_set_config(CONF_AFEC, AFEC_CHANNEL_3, &afec_ch_cfg);
	afec_ch_set_config(CONF_AFEC, AFEC_TEMPERATURE_SENSOR, &afec_ch_cfg);

	afec_channel_enable(CONF_AFEC, AFEC_CHANNEL_0);
	afec_channel_enable(CONF_AFEC, AFEC_CHANNEL_1);
	afec_channel_enable(CONF_AFEC, AFEC_CHANNEL_2);
	afec_channel_enable(CONF_AFEC, AFEC_CHANNEL_3);
	afec_channel_enable(CONF_AFEC, AFEC_TEMPERATURE_SENSOR);

	// Internal ADC offset is 0x800
	afec_channel_set_analog_offset(CONF_AFEC, AFEC_TEMPERATURE_SENSOR, 0x800);

	struct afec_temp_sensor_config afec_temp_sensor_cfg;
	afec_temp_sensor_get_config_defaults(&afec_temp_sensor_cfg);
//	afec_temp_sensor_cfg.rctc = true;
	afec_temp_sensor_set_config(CONF_AFEC, &afec_temp_sensor_cfg);
}


/*******************************************************************************
 * Initialize STDIO Debug Console
 */
void scom_debug_console_init(void) {
	const usart_serial_options_t console_uart_options = {
		.baudrate     = CONSOLE_UART_BAUDRATE,
		.paritytype   = CONSOLE_UART_PARITY
	};

	stdio_serial_init(CONSOLE_UART, &console_uart_options);
}


/*******************************************************************************
 * Get chip temperature (in Celsius)
 */
int32_t get_chip_temperature(void) {
	int32_t ul_vol;
	int32_t ul_value;
	int32_t ul_temp;

	ul_value = afec_channel_get_value(CONF_AFEC, AFEC_TEMPERATURE_SENSOR);
	
	ul_vol = ul_value * VOLT_REF / MAX_DIGITAL;

	/*
	 * According to datasheet, The output voltage VT = 1.44V at 27C
	 * and the temperature slope dVT/dT = 4.7 mV/C
	 */
	ul_temp = (ul_vol - 1440)  * 100 / 470 + 27;
	return ul_temp;
}


/*******************************************************************************
 * Get analog voltage value
 */
int32_t get_analog_input(int adc_nr) {
	int32_t ul_vol;
	int32_t ul_value;

	if (adc_nr >= BOARD_NR_ADC) {
		return 0;
	}
	
	ul_value = afec_channel_get_value(CONF_AFEC, AFEC_CHANNEL_0 + adc_nr);
	
	ul_vol = ul_value * VOLT_REF / MAX_DIGITAL;
	
	return ul_vol;
}


