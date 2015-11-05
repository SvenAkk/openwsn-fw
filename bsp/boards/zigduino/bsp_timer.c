/**
 * Author: Sven Akkermans (sven.akkermans@cs.kuleuven.be)
 * Date:   September 2015
 * Description: atmega128rfa1-specific definition of the "bsp_timer" bsp module.
 */

#include "bsp_timer.h"
#include "board.h"
#include "board_info.h"

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
//The watchdog timer can count up to a precision of 16ms and arbitrarily high.
void bsp_timer_init(){
	memset(&bsp_timer_vars,0,sizeof(bsp_timer_vars_t));	// clear local variables

	SCCR0 = (1<<SCEN) | (0 << SCCKSEL); // enable symbol counter, 16Khz clock, absolute compare 1


	SCOCR1LL = 0; //set compare1 registers
	SCCNTLL = 0; //reset counter

	// don't enable interrupts until first compare is set

	// wait for register writes
	while(SCSR & 0x01);
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
	SCIRQM &= (1<<IRQMCP1);	// disable interrupts
	SCCNTLL = 0;	// reset timer
	bsp_timer_vars.last_compare_value =  0;	// record last timer compare value
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

	newCompareValue      =  bsp_timer_vars.last_compare_value + delayTicks;
	bsp_timer_vars.last_compare_value   =  newCompareValue;

	current_value = bsp_timer_get_currentValue();

//	printf("\n delayTicks: %lu\n", delayTicks);
//	printf("current value: %lu\n", current_value);
//	printf("newCompareValue: %lu\n", newCompareValue);
//	printf("temp_last_compare_value: %lu\n", temp_last_compare_value);
//	PORT_TIMER_WIDTH passed_time = current_value - temp_last_compare_value;
//	printf("passed_time: %lu\n",passed_time);

	if (current_value > temp_last_compare_value && delayTicks < current_value - temp_last_compare_value) {
		// we're already too late, schedule the ISR right now manually
		// setting the interrupt flag triggers an interrupt
//		printf("Too late, triggered interrupt \n");
		bsp_timer_isr();

	} else {
//		printf("Set compare, enabled interrupt\n");
		// this is the normal case, have timer expire at newCompareValue
		SCOCR1HH  =  (uint8_t)(newCompareValue>>24);
		SCOCR1HL  =  (uint8_t)(newCompareValue>>16);
		SCOCR1LH  =  (uint8_t)(newCompareValue>>8);
		SCOCR1LL  =  (uint8_t)(newCompareValue);

		SCIRQM |= (1<<IRQMCP1);		// enable interrupts
	}
}

/**
\brief Cancel a running compare.
 */
void bsp_timer_cancel_schedule(){
	SCIRQM &= (1<<IRQMCP1);
}

/**
\brief Return the current value of the timer's counter.

\returns The current value of the timer's counter.
 */
PORT_TIMER_WIDTH   bsp_timer_get_currentValue(){
	PORT_TIMER_WIDTH retval = SCCNTLL;
	retval |= (PORT_TIMER_WIDTH)SCCNTLH << 8;
	retval |= (PORT_TIMER_WIDTH)SCCNTHL << 16;
	retval |= (PORT_TIMER_WIDTH)SCCNTHH << 24;
	return retval;
}

//=========================== private =========================================

//=========================== interrupt handlers ===============================
kick_scheduler_t   bsp_timer_isr(){
	// call the callback
	bsp_timer_vars.cb();
	// kick the OS
	return 1;
}


