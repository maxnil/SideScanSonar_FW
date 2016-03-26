/*
 * sonar_data.h
 *
 * Created: 2015-12-29 17:19:46
 *  Author: Max
 */ 


#ifndef SONAR_DATA_H_
#define SONAR_DATA_H_

#define MAX_SONAR_SAMPLES 2000

struct sonar_data_t {
		uint8_t type;			// Left, Right...
		uint8_t range;			// Range in meter
		uint16_t nr_samples;	// Number of samples
		uint32_t sample_rate;	// Sample rate (Hz)
		uint16_t data[MAX_SONAR_SAMPLES];
	};


/* Create a Sonar Packet */
int create_ping_packet(uint8_t* buffer, int len);

#endif /* SONAR_DATA_H_ */