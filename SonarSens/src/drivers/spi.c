/*
 * spi.c
 *
 * Created: 2013-12-17 22:07:50
 *  Author: Max
 */ 

#include "asf.h"
#include "board.h"
#include "spi.h"

void spi_init(void)
{
	/* Enable the SPI clock */
//	sysclk_enable_peripheral_clock(&SPCR); // Does not work!
	sysclk_enable_module(POWER_RED_REG0, PRSPI_bm);
	
	/* Enable SPI, Master, set clock rate fck/4 */
//	SPCR = (1<<SPE)|(1<<MSTR)|(1<<CPOL)|(1<<CPHA)|(0<<SPR0);
//	SPSR = (0<<SPI2X);

	/* Enable SPI, Master, set clock rate fck/8 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<CPOL)|(1<<CPHA)|(1<<SPR0);
	SPSR = (1<<SPI2X);
}


void spi_transmit(int8_t cData) {
	/* Start transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
}


int8_t spi_receive(void) {
	/* Wait for reception complete */
	while(!(SPSR & (1<<SPIF)));
	/* Return Data Register */
	return SPDR;
}

void spi_select_device(port_pin_t port_pin) {
	ioport_set_pin_low(port_pin);
}

void spi_deselect_device(port_pin_t port_pin) {
	ioport_set_pin_high(port_pin);
}
