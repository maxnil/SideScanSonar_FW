/*
 * timer.h
 *
 * Created: 2014-01-02 19:21:12
 *  Author: Max
 */ 


#ifndef TIMER_H_
#define TIMER_H_

#define TC_TIMEOUT_TICK_HZ 100

#define TC_TIMEOUT_COUNT 4

typedef uint8_t tc_timeout_id_t;

void timer_init(void);

/* Start single shot timeout */
void tc_timeout_start_singleshot(tc_timeout_id_t id, uint16_t timeout);

/* Start periodic timeout */
void tc_timeout_start_periodic(tc_timeout_id_t id, uint16_t period);

/* Start periodic timeout with a specific start timeout */
void tc_timeout_start_offset(tc_timeout_id_t id, uint16_t period, uint16_t start_offset);

/* Test and clear expired flag for running timeout */
bool tc_timeout_test_and_clear_expired(tc_timeout_id_t id);

/* Stop running timeout */
void tc_timeout_stop(tc_timeout_id_t id);

/** @} */


#endif /* TIMER_H_ */