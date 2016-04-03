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


#endif /* LED_H_ */