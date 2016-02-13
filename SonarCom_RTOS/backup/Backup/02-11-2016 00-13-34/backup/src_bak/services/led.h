/*
 * led.h
 *
 * Created: 2015-06-20 13:44:17
 *  Author: Max
 */ 


#ifndef LED_H_
#define LED_H_

#include <asf.h>
#include "compiler.h"
#include "ioport.h"

/**
 * Turns LED On
 */
void LED_On(uint32_t led_gpio);

/**
 * Turns LED Off
 */
void LED_Off(uint32_t led_gpio);

/**
 * Toggles LED
 */
void LED_Toggle(uint32_t led_gpio);

#if 0
/* Turns off the specified LEDs */
#define LED_Off(led)     ioport_set_pin_level(led##_GPIO, led##_INACTIVE_LEVEL)

/* Turns on the specified LEDs */
#define LED_On(led)      ioport_set_pin_level(led##_GPIO, led##_ACTIVE_LEVEL)

/* Toggles the specified LEDs */
#define LED_Toggle(led)  ioport_toggle_pin_level(led##_GPIO)
#endif

#endif /* LED_H_ */