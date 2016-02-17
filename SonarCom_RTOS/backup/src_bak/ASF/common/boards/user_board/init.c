/**
 * \file
 *
 * \brief SonarCom initialization
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

	/* Configure LED pins
	 * default initial state to high (LEDs off) */
	ioport_set_pin_dir(LED0_GPIO, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED1_GPIO, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED2_GPIO, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED3_GPIO, IOPORT_DIR_OUTPUT);
	/* Turn off all LEDs */
	LED_Off(LED0_GPIO);
	LED_Off(LED1_GPIO);
	LED_Off(LED2_GPIO);
	LED_Off(LED3_GPIO);
	
	/* Configure Debug Console UART pins */
	ioport_set_port_peripheral_mode(PINS_UART0_PORT, PINS_UART0, PINS_UART0_FLAGS);

	/* Configure GPS USART pins */
	ioport_set_port_peripheral_mode(PINS_USART0_PORT, PINS_USART0, PINS_USART0_FLAGS);

	/* Configure RS485 USART pins */
	ioport_set_port_peripheral_mode(PINS_USART1_PORT, PINS_USART1, PINS_USART1_FLAGS);

	/* Configure TWI */
	ioport_set_port_peripheral_mode(PINS_TWI0_PORT, PINS_TWI0, PINS_TWI0_FLAGS);

	/* Configure SPI pins */
	ioport_set_port_peripheral_mode(PINS_SPI_PORT, PINS_SPI, PINS_SPI_FLAGS);

	/* Configure Display GPIO pin and set it to active*/
	ioport_set_pin_dir(DISP_RST_GPIO, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(DISP_RST_GPIO, DISP_RST_ACTIVE_LEVEL);

	/* Configure Relay GPIO pin and set it to inactive */
	ioport_set_pin_dir(RELAY_GPIO, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(RELAY_GPIO, RELAY_INACTIVE_LEVEL);

	/* Configure USB GPIO pin */
	ioport_set_pin_dir(USB_VBUS_PIN, IOPORT_DIR_INPUT);

//TBD	gpio_configure_pin(USB_VBUS_PIN, USB_VBUS_FLAGS);

	/* Configure SD-card/HSMCI pins */
	ioport_set_port_peripheral_mode(PINS_HSMCI_PORT, PINS_HSMCI, PINS_HSMCI_FLAGS);
	ioport_set_pin_dir(SD_MMC_0_CD_GPIO, IOPORT_DIR_INPUT);

	/* Configure SMII/Ethernet pins */
	ioport_set_port_peripheral_mode(PINS_GMAC_PORT, PINS_GMAC, PINS_GMAC_FLAGS);
//	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_RXC_IDX, PIN_KSZ8051MNL_RXC_FLAGS);
//	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_TXC_IDX, PIN_KSZ8051MNL_TXC_FLAGS);
//	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_TXEN_IDX, PIN_KSZ8051MNL_TXEN_FLAGS);
//	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_TXD3_IDX, PIN_KSZ8051MNL_TXD3_FLAGS);
//	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_TXD2_IDX, PIN_KSZ8051MNL_TXD2_FLAGS);
//	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_TXD1_IDX, PIN_KSZ8051MNL_TXD1_FLAGS);
//	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_TXD0_IDX, PIN_KSZ8051MNL_TXD0_FLAGS);
//	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_RXD3_IDX, PIN_KSZ8051MNL_RXD3_FLAGS);
//	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_RXD2_IDX, PIN_KSZ8051MNL_RXD2_FLAGS);
//	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_RXD1_IDX, PIN_KSZ8051MNL_RXD1_FLAGS);
//	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_RXD0_IDX, PIN_KSZ8051MNL_RXD0_FLAGS);
//	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_RXER_IDX, PIN_KSZ8051MNL_RXER_FLAGS);
//	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_RXDV_IDX, PIN_KSZ8051MNL_RXDV_FLAGS);
//	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_CRS_IDX, PIN_KSZ8051MNL_CRS_FLAGS);
//	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_COL_IDX, PIN_KSZ8051MNL_COL_FLAGS);
//	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_MDC_IDX, PIN_KSZ8051MNL_MDC_FLAGS);
//	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_MDIO_IDX, PIN_KSZ8051MNL_MDIO_FLAGS);
	ioport_set_pin_dir(PIN_KSZ8051MNL_INTRP_IDX, IOPORT_DIR_INPUT);
	
	ioport_set_pin_dir(ETH_RST_GPIO, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(ETH_RST_GPIO, ETH_RST_ACTIVE_LEVEL);
}
