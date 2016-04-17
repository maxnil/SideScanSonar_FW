/**
 * \file
 *
 * \brief User board initialization template
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>

void board_init(void)
{
	ioport_init();
	
	// Set output direction on the given LED IOPORTs
	ioport_set_pin_dir(GREEN_LED,           IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(YELLOW_LED,          IOPORT_DIR_OUTPUT);
	// Set input direction on the given Data Ready IOPORTs
	ioport_set_pin_dir(SENSORS_ACC_DRDY,    IOPORT_DIR_INPUT);
	ioport_set_pin_dir(SENSORS_COMP_DRDY,   IOPORT_DIR_INPUT);
	ioport_set_pin_dir(SENSORS_GYRO_DRDY,   IOPORT_DIR_INPUT);
	ioport_set_pin_dir(SENSORS_IRTEMP_DRDY, IOPORT_DIR_INPUT);
	// Set output direction on the given SPI IOPORTs
	ioport_set_pin_dir(SENSORS_ACC_CS,      IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(SENSORS_COMP_CS,     IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(SENSORS_GYRO_CS,     IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(SENSORS_SCK,         IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(SENSORS_MOSI,        IOPORT_DIR_OUTPUT);
	// Set input/output direction on the given 'bit-banging' SPI IOPORTs
	ioport_set_pin_dir(PRESSURE_SCK,        IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PRESSURE_MOSI,       IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PRESSURE_MISO,       IOPORT_DIR_INPUT);

	// Turn off LEDs
	ioport_set_pin_low(GREEN_LED);
	ioport_set_pin_low(YELLOW_LED);
	// Inactivate all SPI slave select
	ioport_set_pin_high(SENSORS_ACC_CS);
	ioport_set_pin_high(SENSORS_COMP_CS);
	ioport_set_pin_high(SENSORS_GYRO_CS);
	// Inactivate bit-bang SPI port
	ioport_set_pin_low(PRESSURE_SCK);
	ioport_set_pin_low(PRESSURE_MOSI);
}
