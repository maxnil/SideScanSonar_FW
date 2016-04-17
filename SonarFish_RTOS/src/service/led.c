/*
 * led.c
 *
 * Created: 2015-06-21 14:31:52
 *  Author: Max
 */

#include <asf.h>
#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "user_board.h"

#if 0
/**
 * Turns LED On
 */
void LED_On(uint32_t led_gpio) {
//	taskENTER_CRITICAL();
	ioport_set_pin_level(led_gpio, IOPORT_PIN_LEVEL_HIGH);
//	taskEXIT_CRITICAL();
}


/**
 * Turns LED Off
 */
void LED_Off(uint32_t led_gpio) {
//	taskENTER_CRITICAL();
	ioport_set_pin_level(led_gpio, IOPORT_PIN_LEVEL_LOW);
//	taskEXIT_CRITICAL();
}


/**
 * Toggles LED
 */
void LED_Toggle(uint32_t led_gpio) {
//	taskENTER_CRITICAL();
	ioport_toggle_pin_level(led_gpio);
//	taskEXIT_CRITICAL();
}
#endif
