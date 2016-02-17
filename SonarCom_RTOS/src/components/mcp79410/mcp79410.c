/**********************************************************************************
 * mcp79410.c
 *
 * Created: 2015-01-06 10:27:17
 *  Author: Max
 */ 

#include <asf.h>
#include "FreeRTOS.h"
#include "freertos_twi_master.h"
#include "conf_twi_master.h"
#include "mcp79410.h"

//............................................................................................
// Write a data byte in the I2C RTCC
//............................................................................................
void mcp79410_wr_reg(uint8_t rtcc_reg, uint8_t data) {
	status_code_t twi_status;
	uint8_t buffer[1];

	buffer[0] = data;
		
	twi_package_t packet = {
		.addr[0]      = rtcc_reg,
		.addr_length  = sizeof (uint8_t),		// TWI slave memory address data size
		.chip         = MCP79410_RTCC_ADDR,		// TWI slave bus address
		.buffer       = (void *)buffer,			// transfer data source buffer
		.length       = 1						// transfer data size (bytes)
	};
	
#ifdef CONF_SCOM_ENABLE_FREERTOS_TWI
	twi_status = freertos_twi_write_packet(CONF_TWI, &packet, 100 / portTICK_PERIOD_MS);
	if (twi_status != STATUS_OK) {
		printf("###ERROR: TWI write error %d\n", twi_status);
	}
#else
	if (twi_master_write(CONF_TWI, &packet) != TWI_SUCCESS) {
		printf("TWI write timeout\n");
	}
#endif
}

//..............................................................................................
// Read a data byte from the I2C RTCC
//..............................................................................................
uint8_t mcp79410_rd_reg(uint8_t rtcc_reg) {
	status_code_t twi_status;
	uint8_t buffer[1];

	twi_package_t packet = {
		.addr[0]      = rtcc_reg,
		.addr_length  = sizeof (uint8_t),		// TWI slave memory address data size
		.chip         = MCP79410_RTCC_ADDR,		// TWI slave bus address
		.buffer       = (void *)buffer,			// transfer data source buffer
		.length       = 1						// transfer data size (bytes)
	};
	
#ifdef CONF_SCOM_ENABLE_FREERTOS_TWI
	twi_status = freertos_twi_read_packet(CONF_TWI, &packet, 100 / portTICK_PERIOD_MS);
	if (twi_status != STATUS_OK) {
		printf("###ERROR: TWI read error %d\n", twi_status);
	}
#else
	if (twi_master_read(CONF_TWI, &packet) != TWI_SUCCESS) {
		printf("TWI read timeout\n");
	}
#endif

	return buffer[0];
}


//................................................................................................
// Initializes the I2C RTCC
//................................................................................................
void mcp79410_init(void) {
	uint8_t tmp;

	tmp = mcp79410_rd_reg(ADDR_RTCWKDAY); 				// read day + OSCON bit
	if (!(tmp & VBATEN)) {
		mcp79410_wr_reg(ADDR_RTCWKDAY, VBATEN | 0x01);	// enable the battery back-up + Monday
		printf("Enabling VBATEN in external RTCC\n");
	}

	// Check if oscillator is running, if not set default time and date
	if (!(tmp & OSCON)) {
		mcp79410_set_date(RTCC_DEFAUL_YEAR, RTCC_DEFAUL_MONTH, RTCC_DEFAUL_DAY);
		mcp79410_set_time(0, 0, 0);
	}

	mcp79410_wr_reg(ADDR_CONTROL, SQWEN | SQWFS32K);	// enable square wave output at 32kHz
	mcp79410_wr_reg(ADDR_OSCTRIM, 0x00);				// No oscillator trim
}


//................................................................................................
// Set the time in the RTCC
//................................................................................................
void mcp79410_set_time(uint32_t hour, uint32_t minute, uint32_t second) {
	mcp79410_wr_reg(ADDR_RTCHOUR, (uint8_t)((hour / 10) << 4 | hour % 10));
	mcp79410_wr_reg(ADDR_RTCMIN,  (uint8_t)((minute / 10) << 4 | minute % 10));
	mcp79410_wr_reg(ADDR_RTCSEC,  (uint8_t)((second / 10) << 4 | second % 10 | START_32KHZ));
}

//................................................................................................
// Set the date in the RTCC
//................................................................................................
void mcp79410_set_date(uint32_t year, uint32_t month, uint32_t day) {
	mcp79410_wr_reg(ADDR_RTCYEAR, (uint8_t)(((year-2000) / 10) << 4 | year % 10));
	mcp79410_wr_reg(ADDR_RTCMTH,  (uint8_t)((month / 10) << 4 | month % 10));
	mcp79410_wr_reg(ADDR_RTCDATE, (uint8_t)((day / 10) << 4 | day % 10));
}


//................................................................................................
// Get the time from the external RTCC
//................................................................................................
void mcp79410_get_time(uint32_t *hour, uint32_t *minute, uint32_t *second) {
	uint8_t tmp;
	
	tmp = mcp79410_rd_reg(ADDR_RTCHOUR) & 0x3F;
	*hour = 10*(tmp>>4) + (tmp & 0x0F);
	tmp = mcp79410_rd_reg(ADDR_RTCMIN) & 0x7F;
	*minute = 10*(tmp>>4) + (tmp & 0x0F);
	tmp = mcp79410_rd_reg(ADDR_RTCSEC) & 0x7F;
	*second = 10*(tmp>>4) + (tmp & 0x0F);
}


//................................................................................................
// Get the date from the external RTCC
//................................................................................................
void mcp79410_get_date(uint32_t *year, uint32_t *month, uint32_t *day) {
	uint8_t tmp;
	
	tmp = mcp79410_rd_reg(ADDR_RTCYEAR);
	*year = 2000 + 10*(tmp>>4) + (tmp & 0x0F);
	tmp = mcp79410_rd_reg(ADDR_RTCMTH) & 0x1F;
	*month = 10*(tmp>>4) + (tmp & 0x0F);
	tmp = mcp79410_rd_reg(ADDR_RTCDATE) & 0x3F;
	*day = 10*(tmp>>4) + (tmp & 0x0F);
	//	temp = mcp79410_rd_reg(ADDR_RTCWKDAY);
}


//................................................................................................
// Get the power-up time-stamp from the RTCC
//................................................................................................
Bool mcp79410_get_pwrup_time(uint32_t *pwrup_month, uint32_t *pwrup_day, uint32_t *pwrup_hour, uint32_t *pwrup_minute) {
	uint8_t tmp;
	
	tmp = mcp79410_rd_reg(ADDR_RTCWKDAY);
	if (tmp & PWRFAIL) {
		tmp = mcp79410_rd_reg(ADDR_PWRUPMTH) & 0x1F;
		*pwrup_month =  (tmp>>4)*10 + (tmp & 0x0F);;
		tmp = mcp79410_rd_reg(ADDR_PWRUPDATE) & 0x3F;
		*pwrup_day =    (tmp>>4)*10 + (tmp & 0x0F);
		tmp = mcp79410_rd_reg(ADDR_PWRUPHOUR) & 0x3F;
		*pwrup_hour =   (tmp>>4)*10 + (tmp & 0x0F);
		tmp = mcp79410_rd_reg(ADDR_PWRUPMIN) & 0x7F;
		*pwrup_minute = (tmp>>4)*10 + (tmp & 0x0F);

		return true;
	} else {
		return false;
	}
}

//................................................................................................
// Get the power-down time-stamp from the RTCC
//................................................................................................
Bool mcp79410_get_pwrdn_time(uint32_t *pwrdn_month, uint32_t *pwrdn_day, uint32_t *pwrdn_hour, uint32_t *pwrdn_minute) {
	uint8_t tmp;
	
	tmp = mcp79410_rd_reg(ADDR_RTCWKDAY);
	if (tmp & PWRFAIL) {
		tmp = mcp79410_rd_reg(ADDR_PWRDNMTH) & 0x1F;
		*pwrdn_month =  (tmp>>4)*10 + (tmp & 0x0F);
		tmp = mcp79410_rd_reg(ADDR_PWRDNDATE) & 0x3F;
		*pwrdn_day =    (tmp>>4)*10 + (tmp & 0x0F);
		tmp = mcp79410_rd_reg(ADDR_PWRDNHOUR) & 0x3F;
		*pwrdn_hour =   (tmp>>4)*10 + (tmp & 0x0F);
		tmp = mcp79410_rd_reg(ADDR_PWRDNMIN) & 0x7F;
		*pwrdn_minute = (tmp>>4)*10 + (tmp & 0x0F);
		return true;
	} else {
		return false;
	}
}


//................................................................................................
// Get the power-down time-stamp from the RTCC
//................................................................................................
void mcp79410_clear_pwrupdn(void) {
	uint8_t tmp;
	
	// Clear Power-up/down timestamp
	tmp = mcp79410_rd_reg(ADDR_RTCWKDAY);
	mcp79410_wr_reg(ADDR_RTCWKDAY, tmp);
}


//............................................................................................
// Write 64 bytes to the I2C SRAM
//............................................................................................
Bool mcp79410_write_sram(uint8_t *buf) {
	uint8_t checksum = 0xFF;
	int i;

	twi_package_t packet = {
		.addr[0]      = ADDR_SRAM_PTR,
		.addr_length  = sizeof (uint8_t),		// TWI slave memory address data size
		.chip         = MCP79410_RTCC_ADDR,		// TWI slave bus address
		.buffer       = (void *)buf,			// transfer data source buffer
		.length       = 64						// transfer data size (bytes)
	};
	
	for (i = 0; i < MCP79410_SRAM_SIZE-1; i++) {
		checksum += buf[i];
	}
	
	buf[MCP79410_SRAM_SIZE-1] = 0xFF-checksum;
	
	while (twi_master_write(CONF_TWI, &packet) != TWI_SUCCESS);

	return true;
}

//..............................................................................................
// Read 64 bytes from the I2C EEPROM
//..............................................................................................
Bool mcp79410_read_sram(uint8_t *buf) {
	int i;
	uint8_t checksum = 0;

	twi_package_t packet = {
		.addr[0]      = ADDR_SRAM_PTR,
		.addr_length  = sizeof (uint8_t),		// TWI slave memory address data size
		.chip         = MCP79410_RTCC_ADDR,		// TWI slave bus address
		.buffer       = (void *)buf,			// transfer data source buffer
		.length       = 64						// transfer data size (bytes)
	};

	while (twi_master_read(CONF_TWI, &packet) != TWI_SUCCESS);

	for (i = 0; i < MCP79410_SRAM_SIZE; i++) {
		checksum += buf[i];
	}

	return checksum == 0xFF ? true : false;
}

//............................................................................................
// Write 128 bytes to the I2C EEPROM
//............................................................................................
Bool mcp79410_write_eeprom(uint8_t *buf) {
	int i;

	twi_package_t packet = {
		.addr[0]      = ADDR_SRAM_PTR,
		.addr_length  = sizeof (uint8_t),		// TWI slave memory address data size
		.chip         = MCP79410_EEPROM_ADDR,		// TWI slave bus address
		.buffer       = (void *)buf,			// transfer data source buffer
		.length       = 8						// transfer data size (bytes)
	};

	// Writing 8 bytes at a time
	for (i = 0; i < 128; i += 8) {
		packet.addr[0] = i;
		packet.buffer = (void *)(buf+i);
		while (twi_master_write(CONF_TWI, &packet) != TWI_SUCCESS);
	}

	return true;
}

//..............................................................................................
// Read 128 bytes from the I2C EEPROM
//..............................................................................................
Bool mcp79410_read_eeprom(uint8_t *buf) {

	twi_package_t packet = {
		.addr[0]      = ADDR_SRAM_PTR,
		.addr_length  = sizeof (uint8_t),		// TWI slave memory address data size
		.chip         = MCP79410_EEPROM_ADDR,		// TWI slave bus address
		.buffer       = (void *)buf,			// transfer data source buffer
		.length       = 128						// transfer data size (bytes)
	};

	while (twi_master_read(CONF_TWI, &packet) != TWI_SUCCESS);

	return true;
}