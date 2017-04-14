/*
 * filter.h
 *
 * Created: 2016-09-25 20:38:10
 *  Author: Max
 */ 


#ifndef FILTER_H_
#define FILTER_H_


#define ACC_FILTER_LEN 10
#define MAGN_FILTER_LEN 10

void filter_init(void);

void acc_filter(acc_data_t *acc_data);

void magn_filter(magn_data_t *magn_data);

#endif /* FILTER_H_ */