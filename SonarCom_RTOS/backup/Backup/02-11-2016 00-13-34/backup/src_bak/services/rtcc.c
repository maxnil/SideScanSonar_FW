/**********************************************************************************
 * rtcc.c
 *
 * Created: 2015-01-06 10:27:17
 *  Author: Max
 */ 

#include "components/mcp79410/mcp79410.h"
#include "rtcc.h"
#include "led.h"


const char *WEEKDAYS[7] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
const char *MONTHS[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sept", "Oct", "Nov", "Dec"};

//................................................................................................
// Initializes the I2C RTCC
//................................................................................................
void rtcc_init(void) {
	uint32_t year;
	uint32_t month;
	uint32_t day;
	uint32_t hour;
	uint32_t minute;
	uint32_t second;

	mcp79410_init();						// Initialize the external real time clock

	osc_enable(OSC_SLCK_32K_BYPASS);		// Enable the XIN32 clock input
	rtc_set_hour_mode(RTC, 0);
	mcp79410_get_date(&year, &month, &day);
	rtc_set_date(RTC, year, month, day, 1);
	
	mcp79410_get_time(&hour, &minute, &second);
	rtc_set_time(RTC, hour, minute, second);
#if 0
	NVIC_DisableIRQ(RTC_IRQn);
	NVIC_ClearPendingIRQ(RTC_IRQn);
	NVIC_SetPriority(RTC_IRQn, 0);
	NVIC_EnableIRQ(RTC_IRQn);
	rtc_enable_interrupt(RTC, RTC_IER_SECEN);
#endif
}

//................................................................................................
// Get weekday string
//................................................................................................
char* rtcc_weekday_string(int weekday) {
	return (char*)WEEKDAYS[weekday-1];
}

//................................................................................................
// Get month string
//................................................................................................
char* rtcc_month_string(int month) {
	return (char*)MONTHS[month-1];	
}

//................................................................................................
// Set the time in the RTCC(s)
//................................................................................................
uint32_t rtcc_set_time(uint32_t hour, uint32_t minute, uint32_t second) {
	
	mcp79410_set_time(hour, minute, second);
	return rtc_set_time(RTC, hour, minute, second);
}

//................................................................................................
// Set the date in the RTCC(s)
//................................................................................................
uint32_t rtcc_set_date(uint32_t year, uint32_t month, uint32_t day) {
	mcp79410_set_date(year, month, day);
	return rtc_set_date(RTC, year, month, day, 1);
}

//................................................................................................
// Get the time in the RTCC
//................................................................................................
void rtcc_get_time(uint32_t *hour, uint32_t *minute, uint32_t *second) {
	rtc_get_time(RTC, hour, minute, second);
}

//................................................................................................
// Get the date in the RTCC
//................................................................................................
void rtcc_get_date(uint32_t *year, uint32_t *month, uint32_t *day) {
	uint32_t weekday;

	rtc_get_date(RTC, year, month, day, &weekday);
}

//................................................................................................
// Get the power-up time-stamp from the external RTCC
//................................................................................................
Bool rtcc_get_pwrup_time(uint32_t *pwrup_month, uint32_t *pwrup_day, uint32_t *pwrup_hour, uint32_t *pwrup_minute) {
	return mcp79410_get_pwrup_time(pwrup_month, pwrup_day, pwrup_hour, pwrup_minute);
}

//................................................................................................
// Get the power-down time-stamp from the external RTCC
//................................................................................................
Bool rtcc_get_pwrdn_time(uint32_t *pwrdn_month, uint32_t *pwrdn_day, uint32_t *pwrdn_hour, uint32_t *pwrdn_minute) {
	return mcp79410_get_pwrdn_time(pwrdn_month, pwrdn_day, pwrdn_hour, pwrdn_minute);
}

//................................................................................................
// Get the power-down time-stamp from the external RTCC
//................................................................................................
void rtcc_clear_pwrupdn(void) {
	mcp79410_clear_pwrupdn();
}
//............................................................................................
// Write 64 bytes to the I2C SRAM
//............................................................................................
Bool rtcc_write_sram(uint8_t *buf) {
	return mcp79410_write_sram(buf);
}

//..............................................................................................
// Read 64 bytes from the I2C EEPROM
//..............................................................................................
Bool rtcc_read_sram(uint8_t *buf) {
	return mcp79410_read_sram(buf);
}

//............................................................................................
// Write 128 bytes to the I2C EEPROM
//............................................................................................
Bool rtcc_write_eeprom(uint8_t *buf) {
	return mcp79410_write_eeprom(buf);
}

//..............................................................................................
// Read 128 bytes from the I2C EEPROM
//..............................................................................................
Bool rtcc_read_eeprom(uint8_t *buf) {
	return mcp79410_read_eeprom(buf);
}