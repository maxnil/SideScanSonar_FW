/*
 * sonar_data.h
 *
 * Created: 2015-12-29 17:19:46
 *  Author: Max
 */ 


#ifndef SONAR_DATA_H_
#define SONAR_DATA_H_

/* Create a Sonar Packet */
int create_ping_packet(uint8_t* buffer, int len);

#endif /* SONAR_DATA_H_ */