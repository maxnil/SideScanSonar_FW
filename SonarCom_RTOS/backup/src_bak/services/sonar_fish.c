/*
 * sonar_fish.c
 *
 * Created: 2015-06-24 22:51:31
 *  Author: Max
 */ 

#include "led.h"
#include "sonar_fish.h"

Bool sonar_pwr_en = false;

#define SONAR_OK_LED					LED2_GPIO
#define SONAR_ERROR_LED					LED3_GPIO

/**********************************************************************
 * Enable Sonar Power
 */
void sonar_power_enable(Bool sonar_enable) {
	if (sonar_enable) {
		LED_Off(SONAR_ERROR_LED);									// Turn off Error LED
		LED_On(SONAR_OK_LED);										// Turn on OK LED
		ioport_set_pin_level(RELAY_GPIO, RELAY_ACTIVE_LEVEL);		// Turn on relay
	} else {
		LED_On(SONAR_ERROR_LED);									// Turn on ERROR LED
		LED_Off(SONAR_OK_LED);										// Turn off OK LED
		ioport_set_pin_level(RELAY_GPIO, RELAY_INACTIVE_LEVEL);		// Turn off relay		
	}	
	sonar_pwr_en = sonar_enable;
}


/**********************************************************************
 * Check Sonar Power status
 */
Bool sonar_power_status(void) {
	return sonar_pwr_en;
}