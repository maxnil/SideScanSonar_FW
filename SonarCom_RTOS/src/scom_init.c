/*
 * scom_init.c
 *
 * Created: 2015-01-04 21:58:15
 *  Author: Max
 */ 

#include <asf.h>
#include <string.h>

#include "scom_init.h"
#include "led.h"

#include "conf_afec.h"
#include "conf_spi_master.h"
#include "conf_twi_master.h"
//#include "conf_timer.h"
#include "conf_uart_serial.h"
//#include "rs485.h"

//Bool sonar_pwr_en = false;


/**********************************************************************
 * Initialize Timer
 */
#if 0
void scom_timer_init(void) {
	/* Configure the PMC to enable the TC module. */
	sysclk_enable_peripheral_clock(ID_TC_RS485_TIMEOUT);

	/** Configure TC interrupts for TC WAVE only */
	NVIC_DisableIRQ(TC_RS485_TIMEOUT_IRQn);
	NVIC_ClearPendingIRQ(TC_RS485_TIMEOUT_IRQn);
	NVIC_SetPriority(TC_RS485_TIMEOUT_IRQn, 0);
	NVIC_EnableIRQ(TC_RS485_TIMEOUT_IRQn);
	
	/* Initialize TC to waveform mode. */
	tc_init(TC, TC_RS485_TIMEOUT_CHANNEL, TC_RS485_TIMEOUT_CLOCK | TC_CMR_CPCDIS | TC_CMR_WAVE | TC_CMR_WAVSEL_UP);

	/* Configure waveform frequency and duty cycle. */
	tc_write_rc(TC, TC_RS485_TIMEOUT_CHANNEL, TC_RS485_TIMEOUT_VALUE);

	tc_enable_interrupt(TC, TC_RS485_TIMEOUT_CHANNEL, TC_IER_CPCS);

	/* Start Timeout counter */
//	tc_start(TC, TC_RS485_TIMEOUT_CHANNEL);
}


/**
 * \brief Interrupt handler for the TC TC_CHANNEL_CAPTURE
 */
void TC_RS485_TIMEOUT_Handler(void) {
	uint32_t tc_status;
	
	tc_status = tc_get_status(TC, TC_RS485_TIMEOUT_CHANNEL);
	
	/* Check for RX timeout */
	if ((tc_status & TC_SR_CPCS) == TC_SR_CPCS) {
		LED_Toggle(LED3);
		usart_enable_interrupt(CONF_RS485_USART, US_IER_TXEMPTY);
		usart_disable_interrupt(CONF_RS485_USART, US_IER_TIMEOUT);

		/* Disable RX, enable TX */
		usart_enable_tx(CONF_RS485_USART);
		usart_disable_rx(CONF_RS485_USART);
			
		rs485_send_ping();
	}
}
#endif



/**********************************************************************
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


/**********************************************************************
 * Initialize STDIO Debug Console
 */
void scom_debug_console_init(void) {
	const usart_serial_options_t console_uart_options = {
		.baudrate     = CONSOLE_UART_BAUDRATE,
		.paritytype   = CONSOLE_UART_PARITY
	};

	stdio_serial_init(CONSOLE_UART, &console_uart_options);
}


/**********************************************************************
 * Initialize TWI/I2C controller
 */
void scom_twi_init(void) {
	twi_master_options_t opt;

	memset((void *)&opt, 0, sizeof(opt));
	opt.speed = CONF_TWI_SPEED;
	opt.chip  = 0x00; // ?? /Max

	// Initialize the TWI master driver.
	twi_master_setup(CONF_TWI, &opt);
}

#if 0
/**********************************************************************
 * Initialize SPI controller
 */
void scom_spi_init(void) {
	struct spi_device spi_device_ext = {
		.id = CONF_SPI_EXT_DEVICE_ID
	};

	struct spi_device spi_device_disp = {
		.id = CONF_SPI_DISP_DEVICE_ID
	};

	spi_master_init(CONF_SPI);
	spi_master_setup_device(CONF_SPI, &spi_device_disp, CONF_SPI_DISP_MODE, CONF_SPI_DISP_BAUDRATE, 0);
	spi_set_bits_per_transfer(CONF_SPI, spi_device_disp.id, CONF_SPI_DISP_BITS_PER_TRANSFER);

	spi_master_setup_device(CONF_SPI, &spi_device_ext, CONF_SPI_EXT_MODE, CONF_SPI_EXT_BAUDRATE, 0);
	spi_set_bits_per_transfer(CONF_SPI, spi_device_ext.id, CONF_SPI_EXT_BITS_PER_TRANSFER);
	spi_enable(SPI);
}
#endif

#if 0
/**********************************************************************
 * Enable Sonar Power
 */
void sonar_power_enable(Bool sonar_enable) {
	if (sonar_enable) {
#ifdef DEBUG_L1
		printf("Sonar power is on\n");
#endif
		LED_Off(LED0);												// Turn off green LED #0
		LED_On(LED1);												// Turn on red LED #1
		ioport_set_pin_level(RELAY_GPIO, RELAY_ACTIVE_LEVEL);		// Turn on relay
	} else {
#ifdef DEBUG_L1
		printf("Sonar power is off\n");
#endif
		LED_Off(LED0);												// Turn off green LED #0
		LED_Off(LED1);												// Turn off red LED #1
		ioport_set_pin_level(RELAY_GPIO, RELAY_INACTIVE_LEVEL);		// Turn off relay		
	}	
	sonar_pwr_en = sonar_enable;
}


/**********************************************************************
 * Check Sonar Power status
 */
Bool sonar_power_is_enabled(void) {
	return sonar_pwr_en;
}
#endif

/**********************************************************************
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


