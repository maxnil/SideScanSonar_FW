/*
 * hmc5983.c
 *
 * Created: 2013-12-17 22:41:02
 *  Author: Max
 */ 

#include <math.h>

#include "asf.h"
#include "hmc5983.h"
#include "drivers/spi.h"

// Configuration Register A
#define CRA  0x00
#define CRA_MS 0
#define CRA_DO 2
#define CRA_MA 5
#define CRA_TS 7
// Configuration Register B
#define CRB  0x01
#define CRB_GN 5
// Mode Register
#define MR   0x02
#define MR_MD  0
#define MR_SIM 2
#define MR_LP  5
// Data Output X Register A and B
#define DXRA 0x03
#define DXRB 0x04
// Data Output Z Register A and B
#define DZRA 0x05
#define DZRB 0x06
// Data Output Y Register A and B
#define DYRA 0x07
#define DYRB 0x08
// Status Register
#define SR   0x09
#define SR_RDY  0
#define SR_LOCK 1
#define SR_DOW  4
// Identification Register A, B and C
#define IRA  0x0A
#define IRB  0x0B
#define IRC  0x0C
// Temperatur Register A
#define TEMPH 0x31
#define TEMPL 0x32

// SPI command
#define READ_INC  (0x80 | 0x40)
#define WRITE_INC (0x00 | 0x40)
#define READ      (0x80 | 0x00)
#define WRITE     (0x00 | 0x00)

// Local struct for HMC5983 configuration parameters
typedef struct hmc5983_config_t
{
	port_pin_t ss_pin;
} hmc5983_config_t;

hmc5983_config_t hmc5983_config;

// Initialize HMC5983 eCompass
void hmc5983_init(port_pin_t ss_pin)
{
	hmc5983_config.ss_pin = ss_pin;
	
	// Activate HMC5983 slave select
	spi_select_device(hmc5983_config.ss_pin);
		
	spi_transmit(WRITE_INC | CRA);				// Send SPI command
	spi_receive();								// Receive dummy byte
	spi_transmit(1<<CRA_TS | 3<<CRA_MA);		// Write CRA (Temperature enable, measure average = 8)
	spi_receive();								// Receive dummy byte
	spi_transmit(0<<CRB_GN);					// Write CRB (Gain = 0)
	spi_receive();								// Receive dummy byte
	spi_transmit(1<<MR_MD);						// Write MR (Single-Measurement Mode)
	spi_receive();								// Receive dummy byte
		
	// Deactivate HMC5983 slave select
	spi_deselect_device(hmc5983_config.ss_pin);
}

// Get temperature from HMC5983
double hmc5983_get_temp(void)
{
	int16_t data;
	double temp;
	
	// Activate HMC5983 slave select
	spi_select_device(hmc5983_config.ss_pin);
	
	spi_transmit(READ_INC | TEMPH);
	spi_receive();								// Receive dummy byte
	spi_transmit(0x00);							// Transmit 8 cycles
	data = spi_receive() << 8;					// Receive first data byte
	spi_transmit(0x00);							// Transmit 8 cycles
	data |= spi_receive() & 0x00FF;				// Receive second data byte
	
	// Deactivate HMC5983 slave select
	spi_deselect_device(hmc5983_config.ss_pin);

	temp = (data / 128.0) + 25.0;

	return temp;
}

// Get magnetic values
int8_t hmc5983_get_data(hmc5983_data_t *data)
{
	uint8_t tmp_l, tmp_h;
	
	// Activate HMC5983 slave select
	spi_select_device(hmc5983_config.ss_pin);
	
	spi_transmit(WRITE | MR);					// Send SPI command
	spi_receive();								// Receive dummy byte
	spi_transmit(1<<MR_MD);						// Write MR
	spi_receive();								// Receive dummy byte
	
	// Deactivate HMC5983 slave select
	spi_deselect_device(hmc5983_config.ss_pin);
	
	// Wait for DRDY to go high
	while (!ioport_get_pin_level(ECOMPASS_DRDY));
	
	// Activate HMC5983 slave select
	spi_select_device(hmc5983_config.ss_pin);
		
	spi_transmit(READ_INC | DXRA);
	spi_receive();								// Receive dummy byte

	spi_transmit(0x00);							// Transmit 8 cycles
	tmp_h = spi_receive();						// Receive first data byte
	spi_transmit(0x00);							// Transmit 8 cycles
	tmp_l = spi_receive();						// Receive second data byte
	data->x = (tmp_h << 8) | tmp_l;	

	spi_transmit(0x00);							// Transmit 8 cycles
	tmp_h = spi_receive();						// Receive third data byte
	spi_transmit(0x00);							// Transmit 8 cycles
	tmp_l = spi_receive();						// Receive forth data byte
	data->z = (tmp_h << 8) | tmp_l;

	spi_transmit(0x00);							// Transmit 8 cycles
	tmp_h = spi_receive();						// Receive fifth data byte
	spi_transmit(0x00);							// Transmit 8 cycles
	tmp_l = spi_receive();						// Receive sixth data byte
	data->y = (tmp_h << 8) | tmp_l;
	
	// Deactivate HMC5983 slave select
	spi_deselect_device(hmc5983_config.ss_pin);

	// Wait for DRDY to go low
//	while (ioport_get_pin_level(ECOMPASS_DRDY));

	return 0;
}


// Get Identification Register values
int hmc5983_get_id(hmc5983_id_t *data)
{
	// Activate HMC5983 slave select
	spi_select_device(hmc5983_config.ss_pin);
	
	spi_transmit(READ_INC | IRA);
	spi_receive();								// Receive dummy byte

	spi_transmit(0x00);							// Transmit 8 cycles
	data->a = spi_receive();						// Receive first data byte
	spi_transmit(0x00);							// Transmit 8 cycles
	data->b = spi_receive();						// Receive second data byte
	spi_transmit(0x00);							// Transmit 8 cycles
	data->c = spi_receive();						// Receive sixth data byte
	
	// Deactivate HMC5983 slave select
	spi_deselect_device(hmc5983_config.ss_pin);

	return 0;
}
