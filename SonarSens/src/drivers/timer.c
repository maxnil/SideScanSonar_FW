/*
 * timer.c
 *
 * Created: 2014-01-02 19:20:42
 *  Author: Max
 */

#include "asf.h"
#include "conf_clock.h"
#include "timer.h"

//#include "ioport.h"
//#include "conf_board.h"

#define F_OC1A TC_TIMEOUT_TICK_HZ

struct tc_timeout_struct {
	uint16_t count;
	uint16_t period;
};

static struct tc_timeout_struct tc_timeout_array[TC_TIMEOUT_COUNT];

static uint8_t tc_timeout_active;
static uint8_t tc_timeout_expired;

//! \brief Interrupt handler for TC compare
ISR(TIMER1_COMPA_vect)
{
	uint8_t i;
	
	for (i = 0; i < TC_TIMEOUT_COUNT; i++) {
		if (!(tc_timeout_active & (1 << i)))
			continue;
		tc_timeout_array[i].count--;
 		if (tc_timeout_array[i].count)
			continue;
		tc_timeout_expired |= 1 << i;
		if (tc_timeout_array[i].period)
			tc_timeout_array[i].count = tc_timeout_array[i].period;
		else
			tc_timeout_active &= ~(1 << i);
	}	
}

void timer_init(void)
{
	/* Enable the Timer clock */
	sysclk_enable_peripheral_clock(&TCCR1A);

	/* Select CTC mode, and 1/64 prescaler */
	TCCR1A = (0<<WGM10);
	TCCR1B = (3<<CS10)|(1<<WGM12);
	
	TCNT1 = 0x0000;								// Start the timer at 0
	
	OCR1A = (F_CPU / (64 * F_OC1A)) - 1;		// Set comparator register
	
	TIMSK1 = (1<<OCIE1A);
}

void tc_timeout_start_offset(tc_timeout_id_t id, uint16_t period, uint16_t offset)
{
	irqflags_t flags;

	flags = cpu_irq_save();
	tc_timeout_array[id].count = offset;
	tc_timeout_array[id].period = period;
	tc_timeout_active |= 1 << id;
	// Clear any pending expired in case of timeout restart
	tc_timeout_expired &= ~(1 << id);
	cpu_irq_restore(flags);
}

void tc_timeout_start_singleshot(tc_timeout_id_t id, uint16_t timeout)
{
	tc_timeout_start_offset(id, 0, timeout);
}

void tc_timeout_start_periodic(tc_timeout_id_t id, uint16_t period)
{
	tc_timeout_start_offset(id, period, period);
}

bool tc_timeout_test_and_clear_expired(tc_timeout_id_t id)
{
	irqflags_t flags;

	if (tc_timeout_expired & (1 << id)) {
		// Clear expired flag safely if it's set
		flags = cpu_irq_save();
		tc_timeout_expired &= ~(1 << id);
		cpu_irq_restore(flags);
		return true;
	}

	return false;
}

void tc_timeout_stop(tc_timeout_id_t id)
{
	irqflags_t flags;

	flags = cpu_irq_save();
	tc_timeout_active &= ~(1 << id);
	cpu_irq_restore(flags);
}
