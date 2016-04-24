/**
 * SonarFish board initialization
 *
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>

#include "ioport.h"
#include "led.h"

/**
 * \brief Set peripheral mode for IOPORT pins.
 * It will configure port mode and disable pin mode (but enable peripheral).
 * \param port IOPORT port to configure
 * \param masks IOPORT pin masks to configure
 * \param mode Mode masks to configure for the specified pin (\ref ioport_modes)
 */
#define ioport_set_port_peripheral_mode(port, masks, mode) \
	do {\
		ioport_set_port_mode(port, masks, mode);\
		ioport_disable_port(port, masks);\
	} while (0)

/**
 * \brief Set peripheral mode for one single IOPORT pin.
 * It will configure port mode and disable pin mode (but enable peripheral).
 * \param pin IOPORT pin to configure
 * \param mode Mode masks to configure for the specified pin (\ref ioport_modes)
 */
#define ioport_set_pin_peripheral_mode(pin, mode) \
	do {\
		ioport_set_pin_mode(pin, mode);\
		ioport_disable_pin(pin);\
	} while (0)

/**
 * \brief Set input mode for one single IOPORT pin.
 * It will configure port mode and disable pin mode (but enable peripheral).
 * \param pin IOPORT pin to configure
 * \param mode Mode masks to configure for the specified pin (\ref ioport_modes)
 * \param sense Sense for interrupt detection (\ref ioport_sense)
 */
/*#define ioport_set_pin_input_mode(pin, mode, sense) \
	do {\
		ioport_set_pin_dir(pin, IOPORT_DIR_INPUT);\
		ioport_set_pin_mode(pin, mode);\
		ioport_set_pin_sense_mode(pin, sense);\
	} while (0)
*/


void board_init(void) {
#ifndef CONF_BOARD_KEEP_WATCHDOG_AT_INIT
	/* Disable the watchdog */
	WDT->WDT_MR = WDT_MR_WDDIS;
#endif

	/* Initialize IOPORTs */
	ioport_init();

	/* Configure LED pins */
	ioport_set_pin_dir(LED0_GPIO, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED1_GPIO, IOPORT_DIR_OUTPUT);

	/* Turn off all LEDs */
	LED_Off(LED_RED);
	LED_Off(LED_BLUE);


	/* Configure Debug Console UART pins */
	ioport_set_port_peripheral_mode(PINS_UART0_PORT, PINS_UART0, PINS_UART0_FLAGS);

	/* Configure Sensor UART pins */
	ioport_set_port_peripheral_mode(PINS_UART1_PORT, PINS_UART1, PINS_UART1_FLAGS);

	/* Configure RS485 USART pins */
	ioport_set_port_peripheral_mode(PINS_USART1_PORT, PINS_USART1, PINS_USART1_FLAGS);

	/* Configure TWI */
	ioport_set_port_peripheral_mode(PINS_TWI0_PORT, PINS_TWI0, PINS_TWI0_FLAGS);

	/* Configure SPI pins */
	ioport_set_port_peripheral_mode(PINS_SPI_PORT, PINS_SPI, PINS_SPI_FLAGS);

	/* Configure USB GPIO pin */
	ioport_set_pin_dir(USB_VBUS_PIN, IOPORT_DIR_INPUT);

//TBD	gpio_configure_pin(USB_VBUS_PIN, USB_VBUS_FLAGS);

	/* Configure SD-card/HSMCI pins */
	ioport_set_port_peripheral_mode(PINS_HSMCI_PORT, PINS_HSMCI, PINS_HSMCI_FLAGS);
	ioport_set_pin_dir(SD_MMC_0_CD_GPIO, IOPORT_DIR_INPUT);


	/* Configure Memory IF pins */
	ioport_set_port_peripheral_mode(PINS_MEM_PORT, PINS_MEM, PINS_MEM_FLAGS);
//	ioport_set_pin_dir(ETH_RST_GPIO, IOPORT_DIR_OUTPUT);
//	ioport_set_pin_level(ETH_RST_GPIO, ETH_RST_ACTIVE_LEVEL);
}
