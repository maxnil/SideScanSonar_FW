/*
 * sonar_data.c
 *
 * Created: 2015-12-29 17:20:04
 *  Author: Max
 */ 

/* Standard includes. */
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "compiler.h"
#include "sonar_data.h"

uint8_t tkn = 'a';

int left_samples[500] =
	  {32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
		  32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768};

int ping_nr = 0;

static const uint8_t *const ping_header = (uint8_t*)"$PING:2014,2500,5.00,500,";

/* Create a Sonar Packet */
int create_ping_packet(uint8_t* buffer, int len) {
	int i;
	int j;
	uint16_t left_data;
	uint16_t right_data;
	int ping_header_len;
	
	ping_header_len = strlen((char *)ping_header);
	memcpy(buffer, ping_header, ping_header_len);
	i = ping_header_len;
	
#if 0	
	buffer[i++] = '$';
	buffer[i++] = 'P';
	buffer[i++] = 'I';
	buffer[i++] = 'N';
	buffer[i++] = 'G';
	buffer[i++] = ',';
	buffer[i++] = '2';
	buffer[i++] = '0';
	buffer[i++] = '1';
	buffer[i++] = '4';
	buffer[i++] = ',';
	buffer[i++] = '2';
	buffer[i++] = '5';
	buffer[i++] = '0';
	buffer[i++] = '0';
	buffer[i++] = ',';
	buffer[i++] = '5';
	buffer[i++] = '.';
	buffer[i++] = '0';
	buffer[i++] = '0';
	buffer[i++] = ',';
	buffer[i++] = '5';
	buffer[i++] = '0';
	buffer[i++] = '0';
	buffer[i++] = ',';
#endif
	left_data = (uint16_t)left_samples[0];
	buffer[i++] = left_data & 0xFF;
	buffer[i++] = (left_data>>8) & 0xFF;

	right_data = (uint16_t)0;
	buffer[i++] = right_data & 0xFF;
	buffer[i++] = (right_data>>8) & 0xFF;
	
	for (j = 1; j < 500-1; j++) {
		left_samples[j] = abs((left_samples[j-1] + left_samples[j+1]) / 2 + ((rand() % 65536) - 32768) / 5);
		if (left_samples[j] > 65535) {
			left_samples[j] = 65535;
		}
		buffer[i++] = left_samples[j] & 0xFF;
		buffer[i++] = (left_samples[j]>>8) & 0xFF;
		
		right_data = abs((int)(65535.0 * (sin((double)(ping_nr * j) / 100.0 / 10.0))));
		buffer[i++] = right_data & 0xFF;
		buffer[i++] = (right_data>>8) & 0xFF;
	}
	
	buffer[i++] = left_samples[j+1] & 0xFF;
	buffer[i++] = (left_samples[j+1]>>8) & 0xFF;

	right_data = abs((int)(65535.0 * (sin((double)(ping_nr * j) / 100.0 / 10.0))));
	buffer[i++] = right_data & 0xFF;
	buffer[i++] = (right_data>>8) & 0xFF;

	buffer[i++] = '\n';
	
	if (i > len) {
		printf("###ERROR: create_ping_packet() i > len, %d > %d\n", i, len);
	}
	ping_nr++;
	
	return i;
}
