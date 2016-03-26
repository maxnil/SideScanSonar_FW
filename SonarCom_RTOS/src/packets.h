/*
 * packets.h
 *
 * Created: 2016-03-17 20:08:40
 *  Author: Max
 */ 


#ifndef PACKETS_H_
#define PACKETS_H_

/* Standard includes. */
#include <stdint.h>

/* Sync Start and End word */
#define SYNC_START0 (uint16_t)0xF5
#define SYNC_START1 (uint16_t)0xF5
#define SYNC_END0   (uint16_t)0x0A
#define SYNC_END1   (uint16_t)0x0A

#define START_SYNC  (uint16_t)0xF5F5
#define END_SYNC    (uint16_t)0x0A0A

/* Packet type */
#define IDLE_PACKET     (uint8_t)0x00
#define GPS_PACKET      (uint8_t)0x01
#define SONAR_PACKET    (uint8_t)0x02
#define SENSOR_PACKET   (uint8_t)0x03
#define RESPONSE_PACKET (unit8_t)0x04

/* Packet Header Size: 'Sync start' + 'Length' + 'Type' */
#define PACKET_HEADER_SIZE (2+2+1)
/* Packet Footer Size: 'Sync end' */
#define PACKET_FOOTER_SIZE (2)

struct packet_header_t {
	uint16_t start_sync;
	uint16_t length;
	uint8_t type;
	uint8_t data;
};

struct packet_footer_t {
	uint16_t end_sync;
};

#endif /* PACKETS_H_ */