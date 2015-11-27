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
void bsp_timer_init(){
	memset(&bsp_timer_vars,0,sizeof(bsp_timer_vars_t));	// clear local variables

	SCIRQM &= ~(1<<IRQMCP1);		// disable interrupt
	SCIRQS |= (1<<IRQMCP1);		   // reset pending interrupt

	//Datasheet is vague/wrong: the symbol counter always runs at 62.5KHz
	SCCR0 = (1<<SCEN) | (1 << SCCKSEL); // enable RTC counter (used in sleep)
	ASSR |= (1<<AS2); // enable RTC


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
	delayTicks = delayTicks * 62500/32768; //Counter runs at 62.5KHz  and we want 32KHz = 1s
											// so roughly double the delay

	temp_last_compare_value = bsp_timer_vars.last_compare_value;

	newCompareValue      =  bsp_timer_vars.last_compare_value + delayTicks;
	bsp_timer_vars.last_compare_value   =  newCompareValue;

	current_value = bsp_timer_get_currentValue();

	if (current_value > temp_last_compare_value && delayTicks < current_value - temp_last_compare_value) {
		// we're already too late, schedule the ISR right now manually
		// setting the interrupt flag triggers an interrupt
		//print_debug("Too late, triggered interrupt \n");
		bsp_timer_isr();

	} else {
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


