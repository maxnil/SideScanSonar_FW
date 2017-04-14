/*
 * main.c
 *
 * Main routine for SonarSensor
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
#include "drivers/spi.h"
#include "drivers/timer.h"
#include "drivers/twi_master.h"
#include "drivers/uart.h"
#include "sensors/hmc5983.h"
#include "sensors/lis3dsh.h"
#include "sensors/l3gd20.h"
#include "sensors/max31725.h"
#include "sensors/tmp006.h"
#include "services/compass.h"
#include "services/filter.h"
#include "services/serial_port.h"
#include "services/temperature.h"
#include "services/tilt.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#define DBG_WELCOME_HEADER \
"---------------------------\n" \
"-- SonarSens 2016 " SW_VERSION " --\n" \
"-- "__DATE__" "__TIME__"  --\n" \
"---------------------------\n"

enum {
	SENSOR_READOUT_TIMER,
	OUTPUT_TIMER,
	IDLE_TIMER
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
	acc_data_t		acc_data;			// Accelerometer data
	magn_data_t		magn_data;			// Magnetometer data
	gyro_t			gyro_data;			// Gyroscope data
	max31725_data_t	max31725_data;		// Board temperature data
	tmp006_data_t	tmp006_data;		// IR temperature data
	
	comp_data_t     comp_data;			// Compass heading (deg)
	tilt_data_t     tilt_data;			// Pitch and Roll (deg)
	temp_data_t		temp_data;			// Temperature (C)

	uint32_t idle_counter_min;
	uint32_t idle_counter_max;
	uint32_t idle_counter;

	lis3dsh_id_t lis3dsh_id;
	l3gd20_id_t l3gd20_id;

	
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
	/* Initialize Timer */
	timer_init();
	
	/* Start timers */
	tc_timeout_start_periodic(SENSOR_READOUT_TIMER, TC_TIMEOUT_TICK_HZ/10);	// 10Hz
	tc_timeout_start_periodic(OUTPUT_TIMER,         TC_TIMEOUT_TICK_HZ/5);	// 1Hz
	tc_timeout_start_periodic(IDLE_TIMER,         TC_TIMEOUT_TICK_HZ/1);	// 1Hz
#endif
		
	printf(DBG_WELCOME_HEADER);

	delay_ms(100);	// To make sure that Welcome message is completely sent out
	
	lis3dsh_get_id(&lis3dsh_id);
		
	printf("LIS3DSH ID: %.2X %.2X %.2X\n", lis3dsh_id.info1, lis3dsh_id.info2, lis3dsh_id.who_am_i);
	
	l3gd20_get_id(&l3gd20_id);

	printf("L3GD20 ID: %.2X\n", l3gd20_id.who_am_i);	

//	printf("MAX31725 regs: 0x%.2X 0x%.4X 0x%.4X\n", max31725_conf.conf, max31725_conf.thyst, max31725_conf.tos);

//	printf("TMP006 config: %.4X %.4X %.4X\n", tmp006_conf.conf, tmp006_conf.man_id, tmp006_conf.dev_id);

	idle_counter_min = __INT_MAX__;
	idle_counter_max = 0;

	
	/* Main loop */
	while (1) {
		/* Sensor read-out and calculation */
		if (tc_timeout_test_and_clear_expired(SENSOR_READOUT_TIMER)) {
			ioport_toggle_pin(GREEN_LED);
			
			//-----------------------------------
			// Sensor read-out
			//-----------------------------------
			
			/* Get Accelerator */
			lis3dsh_get_data(&acc_data);
			
			/* Get Magnetometer data */
			hmc5983_get_data(&magn_data);
			
			/* Get Gyroscope data */
			l3gd20_get_data(&gyro_data);
			
			/* Get board temperature */
			max31725_get_data(&max31725_data);
			
			/* Get IR temperature */
			tmp006_get_data(&tmp006_data);
			
			//-----------------------------------
			// Filtration
			//-----------------------------------
			
			/* Accelerator filtration */
			acc_filter(&acc_data);
			
			/* Magnetometer filtration */
			magn_filter(&magn_data);
			
			//-----------------------------------
			// Calculations
			//-----------------------------------
			
			/* Calculate pitch and roll */
			tilt_calc(acc_data, &tilt_data);
			
			/* Calculate compass heading */
			comp_calc(tilt_data, magn_data, &comp_data);

			/* Calculate board and IR temperature */
			temp_calc(max31725_data, tmp006_data, &temp_data);

//			send_data(false, comp_data, tilt_data, temp_data, tmp006_data);
		} else {
			idle_counter++;
		}

		
		/* Send sensor data to UART */
		if (tc_timeout_test_and_clear_expired(OUTPUT_TIMER)) {
			ioport_toggle_pin(YELLOW_LED);
			
			printf("AX,%5d,AY,%5d,AZ,%5d,MX,%5d,MY,%5d,MZ,%5d,P,% 2.1f,R,% 3.1f,H,% 3.1f,HC,% 3.1f\n", acc_data.x, acc_data.y, acc_data.z, magn_data.x, magn_data.y, magn_data.z, tilt_data.pitch, tilt_data.roll, comp_data.heading, comp_data.heading_comp);
/*
			printf("$ACC, %5d,%5d,%5d\n", acc_data.x, acc_data.y, acc_data.z);
			printf("$TILT,% #2.1f,% #3.1f\n", tilt_data.pitch, tilt_data.roll);
			printf("$MAGN,%5d,%5d,%5d\n", magn_data.x, magn_data.y, magn_data.z);
			printf("$COMP ,% #3.1f\n", comp_data.heading);
			printf("$COMPc,% #3.1f\n", comp_data.heading_comp);
*/

//			send_data(true, comp_data, tilt_data, temp_data, tmp006_data);
/*			if (idle_counter_h > idle_counter_max)
				idle_counter_max = idle_counter_h;
				
			if (idle_counter_h < idle_counter_min)
				idle_counter_min = idle_counter_h;
			printf("Min %d Max %d Curr %d\n", idle_counter_min, idle_counter_max, idle_counter_h);
*/
		} else {
			idle_counter++;
		}


		/* Idle counter checker */
		if (tc_timeout_test_and_clear_expired(IDLE_TIMER)) {
			if (idle_counter > idle_counter_max)
				idle_counter_max = idle_counter;
				
			if (idle_counter < idle_counter_min)
				idle_counter_min = idle_counter;
				
			idle_counter = 0;				
		}
	}
}


int usart_putchar_printf(char var, FILE *stream) {
//	if (var == '\n') {
//		uart_putchar('\r');
//	}
	uart_putchar(var);
	return 0;
}