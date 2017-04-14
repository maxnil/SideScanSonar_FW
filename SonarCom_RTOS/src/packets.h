/*
 * packets.h
 *
 * Created: 2016-03-17 20:08:40
 *  Author: Max
 */ 


#ifndef PACKETS_H_
#define PACKETS_H_

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* Standard includes. */
#include <stdint.h>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* Sync Start and Sync End bytes */
#define START_SYNC_BYTE0 0xAF
#define START_SYNC_BYTE1 0xAF

/* Packet type */
#define IDLE_PACKET			(uint8_t)0x00
#define GPS_PACKET			(uint8_t)0x01
#define SONAR_PACKET		(uint8_t)0x02
#define SENSOR_PACKET		(uint8_t)0x03
#define RESPONSE_PACKET		(uint8_t)0x04
#define COMMAND_PACKET		(uint8_t)0x05
#define PONG_PACKET			(uint8_t)0x06
#define LAST_RESPONSE_PACKET (uint8_t)0x07
#define UNKNOWN_PACKET		(uint8_t)0x08

/* Packet Header Size: 'Sync start' + 'Length' + 'Type' */
#define PACKET_HEADER_SIZE (2 + 2 + 1)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ STRUCTS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

struct packet_t {
	uint8_t start_sync[2];
	uint16_t length;
	uint8_t type;
//	uint8_t dummy[3];
	uint8_t data[];
};

#endif /* PACKETS_H_ */