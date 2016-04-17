/*
 * spi.h
 *
 * Created: 2013-12-17 22:08:28
 *  Author: Max
 */ 


#ifndef SPI_H_
#define SPI_H_

void spi_init(void);

void spi_transmit(int8_t cData);

int8_t spi_receive(void);

void spi_select_device(port_pin_t port_pin);

void spi_deselect_device(port_pin_t port_pin);

#endif /* SPI_H_ */