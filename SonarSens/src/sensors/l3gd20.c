/*
 * l3gd20.c
 *
 * Created: 2013-12-17 22:41:20
 *  Author: Max
 */

#include "asf.h"
#include "l3gd20.h"
#include "drivers/spi.h"

#define WHO_AM_I      0x0F
#define CTRL_REG1     0x20
#define CTRL_REG2     0x21
#define CTRL_REG3     0x22
#define CTRL_REG4     0x23
#define CTRL_REG5     0x24
#define REFERENCE     0x25
#define OUT_TEMP      0x26
#define STATUS_REG    0x27
#define OUT_X_L       0x28
#define OUT_X_H       0x29
#define OUT_Y_L       0x2A
#define OUT_Y_H       0x2B
#define OUT_Z_L       0x2C
#define OUT_Z_H       0x2D
#define FIFO_CTRL_REG 0x2E
#define FIFO_SRC_REG  0x2F
#define INT1_CFG      0x30
#define INT1_SRC      0x31
#define INT1_TSH_XL   0x32
#define INT1_TSH_XH   0x33
#define INT1_TSH_YL   0x34
#define INT1_TSH_YH   0x35
#define INT1_TSH_ZL   0x36
#define INT1_TSH_ZH   0x37
#define INT1_DURATION 0x38

// SPI command
#define READ_INC  (0x80 | 0x40)
#define WRITE_INC (0x00 | 0x40)
#define READ      (0x80 | 0x00)
#define WRITE     (0x00 | 0x00)

// Local struct for l3gd20 configuration parameters
typedef struct l3gd20_config_t
{
	port_pin_t ss_pin;
} l3gd20_config_t;

l3gd20_config_t l3gd20_config;

// Initialize l3gd20 Gyro
void l3gd20_init(port_pin_t ss_pin)
{
	l3gd20_config.ss_pin = ss_pin;
	
	// Activate L3GD20 slave select
	spi_select_device(l3gd20_config.ss_pin);
	
	spi_transmit(WRITE_INC | CTRL_REG1);		// Send SPI command
	spi_receive();								// Receive dummy byte
	spi_transmit(0x5F);							// Write CTRL_REG1 (190Hz, 25, PD, Zen, Yen, Xen)
	spi_receive();								// Receive dummy byte
	spi_transmit(0x00);							// Write CTRL_REG2 ()
	spi_receive();								// Receive dummy byte
	spi_transmit(0x80);							// Write CTRL_REG3 (I2_DRDY)
	spi_receive();								// Receive dummy byte
	spi_transmit(0x00);							// Write CTRL_REG4 (BDU)
	spi_receive();								// Receive dummy byte
	spi_transmit(0xC8);							// Write CTRL_REG5 ()
	spi_receive();								// Receive dummy byte
	
	// Deactivate L3GD20 slave select
	spi_deselect_device(l3gd20_config.ss_pin);
	
	delay_ms(10);
	
	// Activate L3GD20 slave select
	spi_select_device(l3gd20_config.ss_pin);
	
	spi_transmit(WRITE | FIFO_CTRL_REG);		// Send SPI command
	spi_receive();								// Receive dummy byte
	spi_transmit(0x00);							// Write FIFO_CTRL_REG (Bypass mode)
	spi_receive();								// Receive dummy byte
	
	// Deactivate L3GD20 slave select
	spi_deselect_device(l3gd20_config.ss_pin);
}


// Get temperature from l3gd20
int8_t l3gd20_get_temp(void)
{
	int8_t data;
	
	// Activate l3gd20 slave select
	spi_select_device(l3gd20_config.ss_pin);
	
	spi_transmit(READ | OUT_TEMP);
	spi_receive();								// Receive dummy byte
	spi_transmit(0x00);							// Transmit 8 cycles
	data = spi_receive();						// Receive first data byte
	
	// Deactivate l3gd20 slave select
	spi_deselect_device(l3gd20_config.ss_pin);
	
	return data;
}

// Get Accelerometer values
int l3gd20_get_data(gyro_t *data)
{
	uint8_t tmp_l, tmp_h;
	
	// Wait for DRDY to go high
//	while (!ioport_get_pin_level(GYRO_DRDY));

	// Activate L3GD20 slave select
	spi_select_device(l3gd20_config.ss_pin);
	
	spi_transmit(READ_INC | OUT_X_L);
	spi_receive();								// Receive dummy byte

	spi_transmit(0x00);							// Transmit 8 cycles
	tmp_l = spi_receive();						// Receive first data byte
	spi_transmit(0x00);							// Transmit 8 cycles
	tmp_h = spi_receive();						// Receive second data byte
	data->x = (tmp_h << 8) | tmp_l;

	spi_transmit(0x00);							// Transmit 8 cycles
	tmp_l = spi_receive();						// Receive third data byte
	spi_transmit(0x00);							// Transmit 8 cycles
	tmp_h = spi_receive();						// Receive forth data byte
	data->z = (tmp_h << 8) | tmp_l;

	spi_transmit(0x00);							// Transmit 8 cycles
	tmp_l = spi_receive();						// Receive fifth data byte
	spi_transmit(0x00);							// Transmit 8 cycles
	tmp_h = spi_receive();						// Receive sixth data byte
	data->y = (tmp_h << 8) | tmp_l;

	// Deactivate L3GD20 slave select
	spi_deselect_device(l3gd20_config.ss_pin);

	return 0;
}


// Get Identification Register values
int l3gd20_get_id(l3gd20_id_t *data)
{
	// Activate L3GD20 slave select
	spi_select_device(l3gd20_config.ss_pin);
	
	spi_transmit(READ | WHO_AM_I);
	spi_receive();								// Receive dummy byte

	spi_transmit(0x00);							// Transmit 8 cycles
	data->who_am_i = spi_receive();				// Receive first data byte
	
	// Deactivate L3GD20 slave select
	spi_deselect_device(l3gd20_config.ss_pin);

	return 0;
}