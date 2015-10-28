/**
 * Author: Sven Akkermans (sven.akkermans@cs.kuleuven.be)
 * Date:   September 2015
 * Description: atmega128rfa1-specific definition of the "bsp_timer" bsp module.
 */

#include "bsp_timer.h"
#include "board.h"
#include "board_info.h"

#include <avr/pgmspace.h>
#include <avr/fuse.h>
#include <avr/eeprom.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <avr/iom128rfa1.h> //Sven: this is advised against, but works.
#include <avr/sleep.h>
#include <avr/power.h>


//=========================== define ==========================================

//=========================== variables =======================================

typedef struct {
	bsp_timer_cbt    cb;
	PORT_TIMER_WIDTH last_compare_value;
} bsp_timer_vars_t;

bsp_timer_vars_t bsp_timer_vars;

//=========================== prototypes ======================================

//=========================== public ==========================================

/**
\brief Initialize this module.

This functions starts the timer, i.e. the counter increments, but doesn't set
any compare registers, so no interrupt will fire.
 */
// We have to pick Timer/Counter2, only that can interrupt sleep.
void bsp_timer_init(){
	// clear local variables
	memset(&bsp_timer_vars,0,sizeof(bsp_timer_vars_t));

	PRR0 &= ~(1<<PRTIM2); // turn on timer 2 for crystal

	/* Disable TIMER2 interrupt */
	TIMSK2 &= ~(1 << OCIE2A);

	ASSR = 0x00;
	while(ASSR & (1<<TCN2UB));      // Wait for ASSR to change.


	/* Set counter to zero */
	TCNT2 = 0;
	while(ASSR & (1<<TCN2UB)) {};

	TCCR2A = (0<<COM2A1) | (0<<COM2A0) | (0<<COM2B1) | (0<<COM2B0) | (1<<WGM21) | (0<<WGM20);
	TCCR2B = (0<<WGM22) |(1<<CS22)|(0<<CS21)|(1<<CS20); //prescale: 1024
	while(ASSR & (1<<TCR2BUB)) {}; // check if busy
}

/**
\brief Register a callback.

\param cb The function to be called when a compare event happens.
 */
void bsp_timer_set_callback(bsp_timer_cbt cb) {
	bsp_timer_vars.cb   = cb;
}

/**
\brief Reset the timer.

This function does not stop the timer, it rather resets the value of the
counter, and cancels a possible pending compare event.
 */
void bsp_timer_reset(){
	OCR2A  =  0;
	TCNT2 = 0;
	TIMSK2 = 0;
	TCCR2A = 0;
	// record last timer compare value
	bsp_timer_vars.last_compare_value =  0;
}

/**
\brief Schedule the callback to be called in some specified time.

The delay is expressed relative to the last compare event. It doesn't matter
how long it took to call this function after the last compare, the timer will
expire precisely delayTicks after the last one.

The only possible problem is that it took so long to call this function that
the delay specified is shorter than the time already elapsed since the last
compare. In that case, this function triggers the interrupt to fire right away.

This means that the interrupt may fire a bit off, but this inaccuracy does not
propagate to subsequent timers.

\param delayTicks Number of ticks before the timer expired, relative to the
                  last compare event.
 */
void bsp_timer_scheduleIn(PORT_TIMER_WIDTH delayTicks){
	PORT_TIMER_WIDTH newCompareValue;
	PORT_TIMER_WIDTH temp_last_compare_value;
	PORT_TIMER_WIDTH current_value;

	temp_last_compare_value = bsp_timer_vars.last_compare_value;

	newCompareValue      =  bsp_timer_vars.last_compare_value + delayTicks/1024 - 1;
	bsp_timer_vars.last_compare_value   =  newCompareValue;

	current_value = bsp_timer_get_currentValue();

	printf("\n\n");
	printf("delayTicks: %lu\n", delayTicks);
	printf("current value: %lu\n", current_value);
	printf("newCompareValue: %lu\n", newCompareValue);
	printf("temp_last_compare_value: %lu\n", temp_last_compare_value);
	PORT_TIMER_WIDTH passed_time = current_value - temp_last_compare_value;
	printf("passed_time: %lu\n",passed_time);

	if (current_value > temp_last_compare_value && delayTicks < current_value - temp_last_compare_value) {
	      // we're already too late, schedule the ISR right now manually
	      // setting the interrupt flag triggers an interrupt
		printf("Too late, triggered interrupt \n");
		bsp_timer_isr();

	} else {
		printf("Set compare, enabled interrupt\n");
		OCR2A  =  newCompareValue;
		TIMSK2 |= (1 << OCIE2A); // Enable TIMER2 output compare interrupt
	}
}

/**
\brief Cancel a running compare.
 */
void bsp_timer_cancel_schedule(){
	OCR2A  =  0;
	TIMSK2 &= ~(1 << OCIE2A); // Disable TIMER2 output compare interrupt
}

/**
\brief Return the current value of the timer's counter.

\returns The current value of the timer's counter.
 */
PORT_TIMER_WIDTH   bsp_timer_get_currentValue(){
	return TCNT2;
}

//=========================== private =========================================

//=========================== interrupt handlers ===============================
kick_scheduler_t   bsp_timer_isr(){
	// call the callback
	bsp_timer_vars.cb();
	// kick the OS
	return 1;
}
