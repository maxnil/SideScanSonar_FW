/*
 * mcp79410.h
 *
 * Created: 2015-01-06 10:21:02
 *  Author: Max
 */ 

#include <asf.h>

#ifndef MCP79410_H_
#define MCP79410_H_

#include "rtcc.h"

/* Write MCP79410 registers */
void mcp79410_wr_reg(uint8_t rtcc_reg, uint8_t data);

/* Read MCP79410 registers */
uint8_t mcp79410_rd_reg(uint8_t rtcc_reg);

/* Initialize the I2C RTCC */
void mcp79410_init(void);

/* Set the time in the RTCC */
void mcp79410_set_time(uint32_t hour, uint32_t minute, uint32_t second);

/* Set the date in the RTCC */
void mcp79410_set_date(uint32_t year, uint32_t month, uint32_t day);

/* Get the time in the RTCC */
void mcp79410_get_time(uint32_t *hour, uint32_t *minute, uint32_t *second);

/* Get the date in the RTCC */
void mcp79410_get_date(uint32_t *year, uint32_t *month, uint32_t *day);

/* Get the Power-up time stamp from the RTCC */
Bool mcp79410_get_pwrup_time(uint32_t *pwrup_month, uint32_t *pwrup_day, uint32_t *pwrup_hour, uint32_t *pwrup_minute);

/* Get the Power-down time stamp from the RTCC */
Bool mcp79410_get_pwrdn_time(uint32_t *pwrdn_month, uint32_t *pwrdn_day, uint32_t *pwrdn_hour, uint32_t *pwrdn_minute);

/* Clear power-up/down time stamp in RTCC */
void mcp79410_clear_pwrupdn(void);

/* Write the SRAM in the RTCC */
Bool mcp79410_write_sram(uint8_t *buf);

/* Read the SRAM from the RTCC */
Bool mcp79410_read_sram(uint8_t *buf);

/* Write the EEPROM in the RTCC */
Bool mcp79410_write_eeprom(uint8_t *buf);

/* Read the EEPROM from the RTCC */
Bool mcp79410_read_eeprom(uint8_t *buf);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                   GLOBAL CONSTANTS RTCC - REGISTERS ADDRESSES
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define  MCP79410_EEPROM_ADDR	0x57	//  Device address for EEPROM
#define  MCP79410_RTCC_ADDR		0x6F	//  Device address for RTCC
//.............................................................................................
#define  ADDR_RTCSEC			0x00	// 
#define  ADDR_RTCMIN			0x01	// 
#define  ADDR_RTCHOUR			0x02	// 
#define  ADDR_RTCWKDAY			0x03	// 
#define  ADDR_RTCDATE			0x04	// 
#define  ADDR_RTCMTH			0x05	// 
#define  ADDR_RTCYEAR			0x06	// 
#define  ADDR_CONTROL			0x07	// 
#define  ADDR_OSCTRIM			0x08	// 
#define  ADDR_EEUNLOCK			0x09	// 
//............................................................................................
#define  ADDR_ALM0SEC			0x0A	// 
#define  ADDR_ALM0MIN			0x0B	// 
#define  ADDR_ALM0HOUR			0x0C	// 
#define  ADDR_ALM0WKDAY			0x0D	// 
#define  ADDR_ALM0WKDATE		0x0E	// 
#define  ADDR_ALM0WKMTH			0x0F	// 
//.............................................................................................
#define  ADDR_ALM1SEC			0x11	// 
#define  ADDR_ALM1MIN			0x12	// 
#define  ADDR_ALM1HOUR			0x13	// 
#define  ADDR_ALM1WKDAY			0x14	// 
#define  ADDR_ALM1WKDATE		0x15 	// 
#define  ADDR_ALM1WKMTH			0x16	// 
//.............................................................................................
#define  ADDR_PWRDNMIN			0x18	// 
#define  ADDR_PWRDNHOUR			0x19	// 
#define  ADDR_PWRDNDATE			0x1A	// 
#define  ADDR_PWRDNMTH			0x1B	// 
//.............................................................................................
#define  ADDR_PWRUPMIN			0x1C	// 
#define  ADDR_PWRUPHOUR			0x1D	// 
#define  ADDR_PWRUPDATE			0x1E	// 
#define  ADDR_PWRUPMTH			0x1F 	// 
//.............................................................................................
#define  ADDR_SRAM_PTR			0x20	//  Pointer of the SRAM area (RTCC)
//.............................................................................................
#define  ADDR_EEPROM_SR			0xFF	//  EEPROM status register
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                  GLOBAL CONSTANTS RTCC - INITIALIZATION
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define  MCP79410_SRAM_SIZE     64         // SRAM size
#define  MCP79410_EEPROM_SIZE   128        // EEPROM size	

/* CONTROL register */
#define  OUT					(1<<7)		// Logic Level for General Purpose Output
#define  SQWEN					(1<<6)		// Square Wave Output Enable
#define  ALM1EN					(1<<5)		// Alarm 1 Module Enable
#define  ALM0EN					(1<<4)		// Alarm 0 Module Enable
#define  EXTOSC					(1<<3)		// External Oscillator Input
#define  CRSTRIM				(1<<2)		// Coarse Trim Mode Enable
#define  SQWFS1H				(0<<0)		// 1 Hz     Square Wave Clock Output Frequency
#define  SQWFS4K				(1<<0)		// 4096 Hz  Square Wave Clock Output Frequency
#define  SQWFS8K				(2<<0)		// 8192 Hz  Square Wave Clock Output Frequency
#define  SQWFS32K				(3<<0)		// 32768 Hz Square Wave Clock Output Frequency

#define  OSCON					(1<<5)		//  state of the oscillator(running or not)
#define  VBATEN					(1<<3)		//  enable battery for back-up
#define  PWRFAIL				(1<<4)		//  Power fail
#define  START_32KHZ			(1<<7)		//  start crystal: ST = b7 (ADDR_SEC)

#endif /* MCP79410_H_ */