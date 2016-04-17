/*
 * twi_master.c
 *
 * Created: 2013-12-17 22:32:29
 *  Author: Max
 */ 

#include <board.h>
#include <util/twi.h>
#include "asf.h"
#include "twi_master.h"

#define MAX_ITTER 5
//#define MULTI_MASTER

void twi_init(void)
{
	// Enable the SPI clock
	sysclk_enable_peripheral_clock(&TWBR);

	/* Set baud rate to 400kHz, SCL freq= F_CPU/(16+2(TWBR).4^TWPS) */
    TWBR = 0x07;
    TWSR = (0<<TWPS1)|(0<<TWPS0);
}


/* Write 'length' number of bytes from 'buff' to 'reg_ptr' at 'twi_addr' */
uint16_t twi_write(uint8_t sla, uint8_t reg_ptr, uint8_t len, uint8_t* buf)
{
//	uint8_t rv;
	uint8_t itter = 0;
	uint16_t err = 0;

restart:
	if (itter++ >= MAX_ITTER) {
//		return -1;
		err |= 1<<0;
		return err;
	}

#ifdef MULTI_MASTER
begin:
#endif

	/* Send START condition */
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);			// Clear TWI interrupt flag, Put start condition on SDA, Enable TWI
	while (!(TWCR & (1<<TWINT)));					// Wait till start condition is transmitted

	switch (TW_STATUS) {
		case TW_REP_START:							// OK, but should not happen
		case TW_START:
			break;

#ifdef MULTI_MASTER
		case TW_MT_ARB_LOST:						// Note [9]
			err |= 1<<1;
			goto begin;
#endif

		default:
			err |= 1<<2;
//			return -1;								// Error: not in start condition, do not send STOP condition
			return err;
	}

	/* Send SLA_W */
	TWDR = sla | TW_WRITE;							// Address and write instruction
	TWCR = (1<<TWINT)|(1<<TWEN);					// Clear TWI interrupt flag,Enable TWI
	while (!(TWCR & (1<<TWINT)));					// Wait till complete TWDR byte transmitted

	switch (TW_STATUS) {
		case TW_MT_SLA_ACK:
			break;

		case TW_MT_SLA_NACK:						// NACK during select: device busy(?)
			err |= 1<<3;
			goto restart;

#ifdef MULTI_MASTER
		case TW_MT_ARB_LOST:						// re-arbitrate
			err |= 1<<4;
			goto begin;
#endif

		default:
			err |= 1<<5;
			goto error;								// Must send stop condition
	}
	
	/* Send register pointer */
	TWDR = reg_ptr;									// Put register pointer in TWDR
	TWCR = (1<<TWINT)|(1<<TWEN);					// Clear TWI interrupt flag,Enable TWI
	while (!(TWCR & (1<<TWINT)));					// Wait till complete TWDR byte transmitted

	switch (TW_STATUS) {
		case TW_MT_DATA_ACK:
			break;

		case TW_MT_DATA_NACK:						// NACK during select: device busy(?)
			err |= 1<<6;
			goto restart;

#ifdef MULTI_MASTER
		case TW_MT_ARB_LOST:						// re-arbitrate
			err |= 1<<7;
			goto begin;
#endif

		default:
			err |= 1<<8;
			goto error;								// Must send stop condition
	}

	/* Write data */
	for (; len > 0; len--) {
		TWDR = *buf++;
		TWCR = (1<<TWINT)|(1<<TWEN);
				
		while (!(TWCR & (1<<TWINT)));				// Wait till complete TWDR byte transmitted

		switch (TW_STATUS) {
			case TW_MT_DATA_NACK:
				err |= 1<<12;
				goto error;

			case TW_MT_DATA_ACK:
//				rv++;  
				break;

			default:
				err |= 1<<13;
				goto error;
		}
	}
  
quit:
	/* Note [14] */
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);			// Clear TWI interrupt flag, Put stop condition on SDA, Enable TWI
	while (!(TWCR & (1<<TWSTO)));					// Wait till stop condition is transmitted

//	return rv;
	return err;

error:
//	rv = -1;
	goto quit;
}


/* Read 'length' number of bytes from 'buff' to 'reg_ptr' at 'twi_addr' */
uint16_t twi_read(uint8_t slv, uint8_t reg_ptr, uint8_t len, uint8_t* buf)
{
//	uint8_t rv;
	uint8_t itter = 0;
	uint16_t err = 0;

restart:
	if (itter++ >= MAX_ITTER) {
//		return -1;
		err |= 1<<0;
		return err;
	}

#ifdef MULTI_MASTER
begin:
#endif

	/* Send START condition */
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);		// Clear TWI interrupt flag, Put start condition on SDA, Enable TWI
	while (!(TWCR & (1<<TWINT)));				// Wait till start condition is transmitted

	switch (TW_STATUS) {
		case TW_REP_START:						// OK, but should not happen
		case TW_START:
			break;

#ifdef MULTI_MASTER
		case TW_MT_ARB_LOST:					// Note [9]
			err |= 1<<1;
			goto begin;
#endif

		default:
			err |= 1<<2;
//			return -1;							// Error: not in start condition, do not send STOP condition
			return err;
	}

	/* Send SLA_W */
	TWDR = slv | TW_WRITE;					// Address and write instruction
	TWCR = (1<<TWINT)|(1<<TWEN);				// Clear TWI interrupt flag,Enable TWI
	while (!(TWCR & (1<<TWINT)));				// Wait till complete TWDR byte transmitted

	switch (TW_STATUS) {
		case TW_MT_SLA_ACK:
			break;

		case TW_MT_SLA_NACK:					// NACK during select: device busy(?)
			err |= 1<<3;
			goto restart;

#ifdef MULTI_MASTER
		case TW_MT_ARB_LOST:					// re-arbitrate
			err |= 1<<4;
			goto begin;
#endif

		default:
			err |= 1<<5;
			goto error;							// Must send stop condition
	}
	
	/* Send register pointer */
	TWDR = reg_ptr;								// Put register pointer in TWDR
	TWCR = (1<<TWINT)|(1<<TWEN);				// Clear TWI interrupt flag,Enable TWI
	while (!(TWCR & (1<<TWINT)));				// Wait till complete TWDR byte transmitted

	switch (TW_STATUS) {
		case TW_MT_DATA_ACK:
			break;

		case TW_MT_DATA_NACK:					// NACK during select: device busy(?)
			err |= 1<<6;
			goto restart;

#ifdef MULTI_MASTER
		case TW_MT_ARB_LOST:					// re-arbitrate
			err |= 1<<7;
			goto begin;
#endif

		default:
			err |= 1<<8;
			goto error;							// Must send stop condition
	}

	/* Send REPEATED_START condition */
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);		// Clear TWI interrupt flag, Put start condition on SDA, Enable TWI
	while (!(TWCR & (1<<TWINT)));				// Wait till start condition is transmitted

	switch (TW_STATUS) {
		case TW_START:							// OK, but should not happen
		case TW_REP_START:
			break;

#ifdef MULTI_MASTER
		case TW_MT_ARB_LOST:					// Note [9]
			err |= 1<<9;
			goto begin;
#endif

		default:
			err |= 1<<10;
			goto error;							// Error: not in start condition
	}
	/* Send SLA_R */
	TWDR = slv | TW_READ;						// Address and write instruction
	TWCR = (1<<TWINT)|(1<<TWEN);				// Clear TWI interrupt flag,Enable TWI
	while (!(TWCR & (1<<TWINT)));				// Wait till complete TWDR byte transmitted

	switch (TW_STATUS) {
		case TW_MR_SLA_ACK:
			break;

		case TW_MR_SLA_NACK:
			err |= 1<<11;
			goto quit;

#ifdef MULTI_MASTER
		case TW_MR_ARB_LOST:
			err |= 1<<12;
			goto begin;
#endif

		default:
			err |= 1<<13;
			goto error;
	}


	/* Get data */
	for (; len > 0; len--) {
		if (len == 1)
			TWCR = (1<<TWINT)|(1<<TWEN);			// Send NACK to terminate transfer
		else
			TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);  // Send ACK
		
		while (!(TWCR & (1<<TWINT)));				// Wait till complete TWDR byte transmitted

		switch (TW_STATUS) {
			case TW_MR_DATA_NACK:
//				len = 0;
				*buf++ = TWDR;
//				rv++;
			//	err |= 1<<14;
				goto quit;

			case TW_MR_DATA_ACK:
				*buf++ = TWDR;
//				rv++;  
				break;

			default:
				err |= 1<<15;
				goto error;
		}
	}
  
quit:
	/* Note [14] */
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);			// Clear TWI interrupt flag, Put stop condition on SDA, Enable TWI
	while (!(TWCR & (1<<TWSTO)));					// Wait till stop condition is transmitted

//	return rv;
	return err;

error:
//	rv = -1;
	goto quit;
}