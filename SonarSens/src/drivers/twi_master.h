/*
 * twi_master.h
 *
 * Created: 2013-12-17 22:32:41
 *  Author: Max
 */ 


#ifndef TWI_MASTER_H_
#define TWI_MASTER_H_

/* Initialize the TWI controller */
void twi_init(void);

/* Read 'length' number of bytes into 'buff' from 'addr' */
uint16_t twi_read(uint8_t twi_addr, uint8_t reg_ptr, uint8_t len, uint8_t* buf);

/* Write 'length' number of bytes from 'buff' to 'addr' */
uint16_t twi_write(uint8_t twi_addr, uint8_t reg_ptr, uint8_t len, uint8_t* buf);

#endif /* TWI_MASTER_H_ */