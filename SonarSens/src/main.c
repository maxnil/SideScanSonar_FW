/*
 * main.c
 *
 * Main routin for SonarSensor
 *
 * Created: 2016-04-17 23:41:45
 * Author : Max
 */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <math.h>
#include <stdio.h>

#include "asf.h"
#include "conf_board.h"
#include "conf_sonarsens.h"
#include "magnetometer.h"
#include "drivers/spi.h"
#include "drivers/timer.h"
#include "drivers/twi_master.h"
#include "drivers/uart.h"
#include "sensors/hmc5983.h"
#include "sensors/lis3dsh.h"
#include "sensors/l3gd20.h"
#include "sensors/max31725.h"
#include "sensors/tmp006.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define DBG_WELCOME_HEADER \
"------------------------------------\r\n" \
"-- SonarSens     2016 " SW_VERSION "      --\r\n" \
"-- Compiled: "__DATE__" "__TIME__" --\r\n" \
"------------------------------------\r\n"

enum {
	TEMPERATURE_TIMEOUT,
	SENSORS_TIMEOUT,
	OUTPUT_TIMEOUT
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL VARIABLES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL FUNCTION DECLARATIONS ~~~~~~~~~~~~~~~~~~~~~~~~~

int usart_putchar_printf(char var, FILE *stream);

FILE uart_output = FDEV_SETUP_STREAM(usart_putchar_printf, NULL, _FDEV_SETUP_WRITE);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Main routine.
 * Initializes interfaces and components
 * Starts tasks
 */
int main (void)
{
	acc_t			acc_data;		// Accelerometer data
	hmc5983_data_t	hmc5983_data;	// HMC5983 data
	gyro_t			gyro_data;		// Gyroscope data
	uint16_t		temp1_data;		// Board temperature data
	tmp006_data_t	temp2_data;		// IR temperature data
	
	magnetometer_t	magn_data;		// Magnetometer data
	
	uint16_t idle_counter_min;
	uint16_t idle_counter_max;
	uint16_t idle_counter_h;
	uint16_t idle_counter_l;

	uint16_t err;
	uint8_t buf[4];
	
//	hmc5983_id_t hmc5983_id;
	lis3dsh_id_t lis3dsh_id;
	l3gd20_id_t l3gd20_id;

	double ir_temp;
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Initialize PLL and clocks
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	sysclk_init();
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Initialize the board
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	board_init();
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Initialize interfaces
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
#ifdef CONF_SSENS_ENABLE_SPI
	/* Initialize SPI */
	spi_init();
#endif
		
#ifdef CONF_SSENS_ENABLE_TIMER
	/* Initialize Timer */
	timer_init();
#endif
			
#ifdef CONF_SSENS_ENABLE_TWI
	/* Initialize TWI (I2C) */
	twi_init();
#endif

#ifdef CONF_SSENS_ENABLE_UART
	/* Initialize UART */
	uart_init();
	stdout = &uart_output;
#endif

#ifdef CONF_SSENS_ENABLE_HMC5983
	/* Initialize eCompass */
	hmc5983_init(ECOMPASS_SS);
#endif
	
#ifdef CONF_SSENS_ENABLE_L3GD20
	/* Initialize Gyro */
	l3gd20_init(GYRO_SS);
#endif
	
#ifdef CONF_SSENS_ENABLE_LIS3DSH
	/* Initialize Accelerometer */
	lis3dsh_init(ACCELEROMETER_SS);
#endif

#ifdef CONF_SSENS_ENABLE_MAX31725
	/* Initialize Board Temperature Sensor */
	max31725_init();
#endif

#ifdef CONF_SSENS_ENABLE_TMP006
	/* Initialize IR Temperature Sensor */
	tmp006_init();
#endif

	/* Enable interrupts */
	cpu_irq_enable();
	
#ifdef CONF_SSENS_ENABLE_TIMER
	/* Start timers */
	tc_timeout_start_periodic(TEMPERATURE_TIMEOUT, TC_TIMEOUT_TICK_HZ/1);	// 1Hz
	tc_timeout_start_periodic(OUTPUT_TIMEOUT,      TC_TIMEOUT_TICK_HZ/2);	// 2Hz
	tc_timeout_start_periodic(SENSORS_TIMEOUT,     TC_TIMEOUT_TICK_HZ/10);	// 10Hz
#endif
		
	printf(DBG_WELCOME_HEADER);

	lis3dsh_get_id(&lis3dsh_id);
		
	printf("LIS3DSH ID: %.2X %.2X %.2X\n", lis3dsh_id.info1, lis3dsh_id.info2, lis3dsh_id.who_am_i);
	
	l3gd20_get_id(&l3gd20_id);

	printf("L3GD20 ID: %.2X\n", l3gd20_id.who_am_i);	

//	printf("MAX31725 regs: 0x%.2X 0x%.4X 0x%.4X\n", max31725_conf.conf, max31725_conf.thyst, max31725_conf.tos);

//	printf("TMP006 config: %.4X %.4X %.4X\n", tmp006_conf.conf, tmp006_conf.man_id, tmp006_conf.dev_id);

	idle_counter_min = __INT_MAX__;
	idle_counter_max = 10;
	idle_counter_h = 0;
	idle_counter_l = 0;

	/* Main loop */
	while (1) {
		if (tc_timeout_test_and_clear_expired(SENSORS_TIMEOUT)) {
			ioport_toggle_pin(GREEN_LED);
			/* Get Magnetometer data */
//			hmc5983_get_data(&hmc5983_data);
			get_magnetometer_data(&magn_data);
			/* Get Accelerometer data */
			lis3dsh_get_data(&acc_data);
			/* Get Gyroscope data */
			l3gd20_get_data(&gyro_data);
/*			if (idle_counter > idle_counter_max)
				idle_counter_max = idle_counter;
			if (idle_counter < idle_counter_min)
				idle_counter_min = idle_counter;
			idle_counter = 0;
*/		} else {
			if (idle_counter_l < 100) {
				idle_counter_l++;
			} else {
				idle_counter_l = 0;
				idle_counter_h++;
			}
		}
		
		if (tc_timeout_test_and_clear_expired(TEMPERATURE_TIMEOUT)) {
			ioport_toggle_pin(YELLOW_LED);
			/* Get board temperature */
			err = max31725_get_temp(&temp1_data);
			/* Get IR temperature */
			err = tmp006_get_data(&temp2_data);
			ir_temp = tmp006_temp_calc(temp2_data.v_obj*156.25e-9, temp2_data.t_amb*0.0078125 + 273.15);		
/*			if (idle_counter > idle_counter_max)
				idle_counter_max = idle_counter;
			if (idle_counter < idle_counter_min)
				idle_counter_min = idle_counter;
			idle_counter = 0;
*/		} else {
			if (idle_counter_l < 100) {
				idle_counter_l++;
			} else {
				idle_counter_l = 0;
				idle_counter_h++;
			}
		}
		
		if (tc_timeout_test_and_clear_expired(OUTPUT_TIMEOUT)) {
//			printf("$MAGN,%6.2f,%6.2f,%7.2f,%7.2f,%7.2f\n", magn_data.strength, magn_data.heading, magn_data.yaw, magn_data.pitch, magn_data.roll);
			printf("$MAGN,%5d,%5d,%5d,%5d,%5d\n", magn_data.strength, magn_data.heading, magn_data.yaw, magn_data.pitch, magn_data.roll);
			printf("$ACC, %5d,%5d,%5d\n", acc_data.x, acc_data.y, acc_data.z);
			printf("$GYRO,%5d,%5d,%5d\n", gyro_data.x, gyro_data.y, gyro_data.z);
			printf("$COMP,%5d,%5d,%5d\n", hmc5983_data.x, hmc5983_data.y, hmc5983_data.y);
			printf("$TEMP,%5d,%5d,%5d,%5d\n", temp1_data, temp2_data.t_amb, temp2_data.v_obj, ir_temp);
/*			if (idle_counter_h > idle_counter_max)
				idle_counter_max = idle_counter_h;
				
			if (idle_counter_h < idle_counter_min)
				idle_counter_min = idle_counter_h;
			printf("Min %d Max %d Curr %d\n", idle_counter_min, idle_counter_max, idle_counter_h);
*/			idle_counter_l = 0;
			idle_counter_h = 0;
		} else {
			if (idle_counter_l < 100) {
				idle_counter_l++;
			} else {
				idle_counter_l = 0;
				idle_counter_h++;
			}
		}
	}
	
//		printf("TMP006 Vobj Tamb: 0x%.4X 0x%.4X %e %.2f\n", temp2_data.v_obj, temp2_data.t_amb, temp2_data.v_obj*156.25e-9, temp2_data.t_amb*0.0078125);
//		delay_ms(100);
	
//		printf("TMP006      Tobj: %f\n", tmp006_temp_calc(temp2_data.v_obj*156.25e-9, temp2_data.t_amb*0.0078125 + 273.15));
	
		printf("%.2f,%.2f,%.2f\n", temp1_data/256.0, temp2_data.t_amb*0.0078125, tmp006_temp_calc(temp2_data.v_obj*156.25e-9, temp2_data.t_amb*0.0078125 + 273.15));
		
//		ioport_set_pin_high(GREEN_LED);
		ioport_set_pin_low(YELLOW_LED);
		delay_ms(500);
//		ioport_set_pin_low(GREEN_LED);
		ioport_set_pin_high(YELLOW_LED);
		delay_ms(500);
	

	while (1) {
	//	printf("Max");
/*
		hmc5983_get_data(&ecomp_data);
		lis3dsh_get_data(&acc_data);
		l3gd20_get_data(&gyro_data);
*/
//		ecomp_dir = get_compass_dir();
		
//		printf("%4i,%4i,%4i,%7.1f,%6.1f\n", ecomp_data.x, ecomp_data.y, ecomp_data.z, ecomp_dir, 180.0-ecomp_dir);
//		printf("%6.1f\n", 180.0-ecomp_dir);
	
//		printf("%6i,%6i,%6i\n", acc_data.x, acc_data.y, acc_data.z);
//		printf("%5.2f,%5.2f,%5.2f\n", acc_data.x/16384.0, acc_data.y/16384.0, acc_data.z/16384.0);

//		printf("%6i,%6i,%6i\n", gyro_data.x, gyro_data.y, gyro_data.z);
/*	
		printf("A:%5.2f,%5.2f,%5.2f,C:%6.3f,%6.3f,%6.3f,G:%7.2f,%7.2f,%7.2f\n",
		acc_data.x/16384.0, acc_data.y/16384.0, acc_data.z/16384.0,
		ecomp_data.x/1370.0, ecomp_data.y/1370.0, ecomp_data.z/1370.0,
		gyro_data.x/(32768.0/250.0), gyro_data.y/(32768.0/250.0), gyro_data.z/(32768.0/250.0));
*/		
	}
}


int usart_putchar_printf(char var, FILE *stream) {
	if (var == '\n')
	uart_putchar('\r');
	uart_putchar(var);
	return 0;
}