/*
 * rtcc.h
 *
 * Created: 2015-01-08 17:25:20
 *  Author: Max
 */ 


#ifndef RTCC_H_
#define RTCC_H_

/* Default settings for Real Time Clock */
#define RTCC_DEFAUL_YEAR  2015
#define RTCC_DEFAUL_MONTH 1
#define RTCC_DEFAUL_DAY   31

/* Initialize the I2C RTCC */
void rtcc_init(void);

/* Get weekday string */
char* rtcc_weekday_string(int weekday);

/* Get month string */
char* rtcc_month_string(int month);

/* Set the time in the RTCC(s) */
uint32_t rtcc_set_time(uint32_t hour, uint32_t minute, uint32_t second);

/* Set the date in the RTCC(s) */
uint32_t rtcc_set_date(uint32_t year, uint32_t month, uint32_t day);

/* Get the time in the RTCC */
void rtcc_get_time(uint32_t *hour, uint32_t *minute, uint32_t *second);

/* Get the date in the RTCC */
void rtcc_get_date(uint32_t *year, uint32_t *month, uint32_t *day);

/* Get the Power-up time stamp from the RTCC */
Bool rtcc_get_pwrup_time(uint32_t *pwrup_month, uint32_t *pwrup_day, uint32_t *pwrup_hour, uint32_t *pwrup_minute);

/* Get the Power-down time stamp from the RTCC */
Bool rtcc_get_pwrdn_time(uint32_t *pwrdn_month, uint32_t *pwrdn_day, uint32_t *pwrdn_hour, uint32_t *pwrdn_minute);

/* Clear power-up/down time stamp */
void rtcc_clear_pwrupdn(void);

/* Write the SRAM in the RTCC */
Bool rtcc_write_sram(uint8_t *buf);

/* Read the SRAM from the RTCC */
Bool rtcc_read_sram(uint8_t *buf);

/* Write the EEPROM in the RTCC */
Bool rtcc_write_eeprom(uint8_t *buf);

/* Read the EEPROM from the RTCC */
Bool rtcc_read_eeprom(uint8_t *buf);

#endif /* RTCC_H_ */