/*
 * lis3dsh.c
 *
 * Created: 2013-12-17 22:42:22
 *  Author: Max
 */ 

#include <math.h>
#include "asf.h"
#include "lis3dsh.h"
#include "drivers/spi.h"

#define INFO1     0x0D
#define INFO2     0x0E
#define WHO_AM_I  0xOF
#define CTRL_REG3 0x23
#define CTRL_REG4 0x20
#define CTRL_REG5 0x24
#define CTRL_REG6 0x25
#define STATUS    0x27
#define OUT_T     0x0C
#define OFF_X     0x10
#define OFF_Y     0x11
#define OFF_Z     0x12
#define CS_X      0x13
#define CS_Y      0x14
#define CS_Z      0x15
#define LC_L      0x16
#define LC_H      0x17
#define STAT      0x18
#define VFC_1     0x1B
#define VFC_2     0x1C
#define VFC_3     0x1D
#define VFC_4     0x1E
#define THRS3     0x1F
#define OUT_X_L   0x28
#define OUT_X_H   0x29
#define OUT_Y_L   0x2A
#define OUT_Y_H   0x2B
#define OUT_Z_L   0x2C
#define OUT_Z_H   0x2D
#define FIFO_CTRL 0x2E
#define FIFO_SRC  0x2F
#define CTRL_REG1 0x21
#define ST1_1     0x40
#define ST1_2     0x41
#define ST1_3     0x42
#define ST1_4     0x43
#define ST1_5     0x44
#define ST1_6     0x45
#define ST1_7     0x46
#define ST1_8     0x47
#define ST1_9     0x48
#define ST1_10    0x49
#define ST1_11    0x4A
#define ST1_12    0x4B
#define ST1_13    0x4C
#define ST1_14    0x4D
#define ST1_15    0x4E
#define ST1_16    0x4F
#define TIM4_1    0x50
#define TIM3_1    0x51
#define TIM2_1_L  0x52
#define TIM2_1_H  0x52
#define TIM1_1_L  0x54
#define TIM1_1_H  0x54
#define THRS2_1   0x56
#define THRS1_1   0x57
#define MASK1_B   0x59
#define MASK1_A   0x5A
#define SETT1     0x5B
#define PR1       0x5C
#define TC1_L     0x5D
#define TC1_H     0x5E
#define OUTS1     0x5F
#define PEAK1     0x19
#define CTRL_REG2 0x22
#define ST2_1     0x60
#define ST2_2     0x61
#define ST2_3     0x62
#define ST2_4     0x63
#define ST2_5     0x64
#define ST2_6     0x65
#define ST2_7     0x66
#define ST2_8     0x67
#define ST2_9     0x68
#define ST2_10    0x69
#define ST2_11    0x6A
#define ST2_12    0x6B
#define ST2_13    0x6C
#define ST2_14    0x6D
#define ST2_15    0x6E
#define ST2_16    0x6F
#define TIM4_2    0x70
#define TIM3_2    0x71
#define TIM2_2_L  0x72
#define TIM2_2_H  0x73
#define TIM1_2_L  0x74
#define TIM1_2_H  0x75
#define THRS2_2   0x76
#define THRS1_2   0x77
#define MASK2_B   0x79
#define MASK2_A   0x7A
#define SETT2     0x7B
#define PR2       0x7C
#define TC2_L     0x7D
#define TC2_H     0x7E
#define OUTS2     0x7F
#define PEAK2     0x1A
#define DES2      0x78

// SPI command
#define READ      0x80
#define WRITE     0x00

// Local struct for LIS3DSH configuration parameters
typedef struct lis3dsh_config_t
{
	port_pin_t ss_pin;
} lis3dsh_config_t;

lis3dsh_config_t lis3dsh_config;

// Initialize LIS3DSH Accelerometer
void lis3dsh_init(port_pin_t ss_pin)
{
	lis3dsh_config.ss_pin = ss_pin;
	
	// Activate LIS3DSH slave select
	spi_select_device(lis3dsh_config.ss_pin);
	
	spi_transmit(WRITE | CTRL_REG6);			// Send SPI command
	spi_receive();								// Receive dummy byte
	spi_transmit(0x10);							// Write ADD_INC in CTRL_REG6
	spi_receive();								// Receive dummy byte
	
	// Deactivate LIS3DSH slave select
	spi_deselect_device(lis3dsh_config.ss_pin);

	delay_ms(10);

	// Activate LIS3DSH slave select
	spi_select_device(lis3dsh_config.ss_pin);
	
	spi_transmit(WRITE | CTRL_REG4);			// Address CTRL_REG4
	spi_receive();								// Receive dummy byte
	spi_transmit(0x3<<4 | 1<<3 | 0x7);			// Write 12.5 Hz, BDU, Zen, Yen, Xen in CTRL_REG4
	spi_receive();								// Receive dummy byte

	spi_transmit(0x00);							// Write 0x00 to CTRL_REG1
	spi_receive();								// Receive dummy byte
	spi_transmit(0x00);							// Write 0x00 to CTRL_REG2
	spi_receive();								// Receive dummy byte
	spi_transmit(1<<2);							// Write VFILT in CTRL_REG3
	spi_receive();								// Receive dummy byte
	spi_transmit((0x3<<6) | (0<<3));			// Write BW=50 Hz, FSCALE=2g in CTRL_REG5
	spi_receive();								// Receive dummy byte
	spi_transmit(0x10);							// Write ADD_INC in CTRL_REG6
	spi_receive();								// Receive dummy byte
	
	// Deactivate LIS3DSH slave select
	spi_deselect_device(lis3dsh_config.ss_pin);
}


// Get temperature from LIS3DSH
int8_t lis3dsh_get_temp(void)
{
	int8_t data;
	
	// Activate LIS3DSH slave select
	spi_select_device(lis3dsh_config.ss_pin);
	
	spi_transmit(READ | OUT_T);
	spi_receive();								// Receive dummy byte
	spi_transmit(0x00);							// Transmit 8 cycles
	data = spi_receive();						// Receive first data byte
	
	// Deactivate LIS3DSH slave select
	spi_deselect_device(lis3dsh_config.ss_pin);
	
	return data;
}

// Get Accelerometer values
int lis3dsh_get_data(acc_data_t *data)
{
	uint8_t tmp_l, tmp_h;
	
	// Wait for DRDY to go high
	while (!ioport_get_pin_level(ACCELEROMETER_DRDY));

	// Activate LIS3DSH slave select
	spi_select_device(lis3dsh_config.ss_pin);
	
	do {
		spi_transmit(READ | STATUS);			// Address STATUS register
		tmp_l = spi_receive();					// Receive status byte
	} while (!(tmp_l & (1<<3)));				// Wait for bit 3 (ZYXDA) to become 1

	spi_transmit(READ | OUT_X_L);				// Address OUT_X_L register
	spi_receive();								// Receive dummy byte

	spi_transmit(0x00);							// Transmit dummy byte
	tmp_l = spi_receive();						// Receive OUT_X_L data byte
	spi_transmit(0x00);							// Transmit dummy byte
	tmp_h = spi_receive();						// Receive OUT_X_H data byte
//	data->x = (tmp_h << 8) | tmp_l;
	data->y = (tmp_h << 8) | tmp_l;				// Remapping x<->y to fit with board orientation

	spi_transmit(0x00);							// Transmit dummy byte
	tmp_l = spi_receive();						// Receive OUT_Y_L data byte
	spi_transmit(0x00);							// Transmit dummy byte
	tmp_h = spi_receive();						// Receive OUT_Y_H data byte
//	data->y = (tmp_h << 8) | tmp_l;
	data->x = (tmp_h << 8) | tmp_l;				// Remapping x<->y to fit with board orientation

	spi_transmit(0x00);							// Transmit dummy byte
	tmp_l = spi_receive();						// Receive OUT_Z_L data byte
	spi_transmit(0x00);							// Transmit dummy byte
	tmp_h = spi_receive();						// Receive OUT_Z_H data byte
	data->z = (tmp_h << 8) | tmp_l;

	// Deactivate LIS3DSH slave select
	spi_deselect_device(lis3dsh_config.ss_pin);

	return 0;
}


// Get Identification Register values
int lis3dsh_get_id(lis3dsh_id_t *data)
{
	// Activate LIS3DSH slave select
	spi_select_device(lis3dsh_config.ss_pin);
	
	spi_transmit(READ | INFO1);
	spi_receive();								// Receive dummy byte

	spi_transmit(0x00);							// Transmit 8 cycles
	data->info1 = spi_receive();				// Receive first data byte
	spi_transmit(0x00);							// Transmit 8 cycles
	data->info2 = spi_receive();				// Receive second data byte
	spi_transmit(0x00);							// Transmit 8 cycles
	data->who_am_i = spi_receive();				// Receive third data byte
	
	// Deactivate LIS3DSH slave select
	spi_deselect_device(lis3dsh_config.ss_pin);

	return 0;
}