/**
\brief Zigduino definition of the "radiotimer" bsp module.

\author Sven Akkermans <sven.akkermans@cs.kuleuven.be>, September 2015.
 */

#include "board_info.h"
#include "radiotimer.h"

//=========================== variables =======================================

typedef struct {
	radiotimer_compare_cbt    overflow_cb;
	radiotimer_compare_cbt    compare_cb;
} radiotimer_vars_t;

radiotimer_vars_t radiotimer_vars;

//=========================== prototypes ======================================

kick_scheduler_t radiotimer_overflow_isr();
kick_scheduler_t radiotimer_compare_isr();
//=========================== public ==========================================

//===== admin

void radiotimer_init() {
	memset(&radiotimer_vars,0,sizeof(radiotimer_vars_t));    // clear local variables
}

void radiotimer_setOverflowCb(radiotimer_compare_cbt cb) {
	radiotimer_vars.overflow_cb     = cb;
}

void radiotimer_setCompareCb(radiotimer_compare_cbt cb) {
	radiotimer_vars.compare_cb      = cb;
}

void radiotimer_setStartFrameCb(radiotimer_capture_cbt cb) {
	while(1);
}

void radiotimer_setEndFrameCb(radiotimer_capture_cbt cb) {
	while(1);
}

void radiotimer_start(PORT_RADIOTIMER_WIDTH period) {
	SCIRQM &= ~(1<<IRQMCP2) | ~(1<<IRQMCP3);		// disable interrupts
	SCIRQS |= (1<<IRQMCP2) | (1<<IRQMCP3);		   // reset pending interrupts

	SCCR0 |= (1<<SCEN) | (0 << SCCKSEL); // enable symbol counter, 16MHz clock
	SCCR0 |= (1 << SCCMP3) | (1 << SCCMP2); // relative compare
	SCCR0 |= (1 << SCMBTS);
	SCCR1 = 0; // no backoff slot counter

	*((PORT_RADIOTIMER_WIDTH *)(&SCOCR2LL)) = 0;
	SCOCR2LL = 0;	//set compare registers


	SCCNTHH = SCCNTHL = SCCNTLH = 0;
	SCCNTLL = 0;	// reset timer value

	radiotimer_setPeriod(period);	//set period
	SCCR0 |= (1 << SCMBTS); // "reset" radiotimer

	while(SCSR & 0x01);	// wait for register writes


	SCIRQM |=  (1<<IRQMCP3);		// enable interrupts from 2nd and 3rd compare.
}

//===== direct access

PORT_RADIOTIMER_WIDTH radiotimer_getValue() {
	return radiotimer_getCapturedTime();
}

void radiotimer_setPeriod(PORT_RADIOTIMER_WIDTH period) {
	SCOCR3HH = (uint8_t)(period>>24);
	SCOCR3HL = (uint8_t)(period>>16);
	SCOCR3LH = (uint8_t)(period>>8);
	SCOCR3LL = (uint8_t)period;
}

PORT_RADIOTIMER_WIDTH radiotimer_getPeriod() {
	return *((PORT_RADIOTIMER_WIDTH *)(&SCOCR3LL));
}

//===== compare

void radiotimer_schedule(PORT_RADIOTIMER_WIDTH offset) {
	SCOCR2HH = (uint8_t)(offset>>24);
	SCOCR2HL = (uint8_t)(offset>>16);
	SCOCR2LH = (uint8_t)(offset>>8);
	SCOCR2LL = (uint8_t)offset;	// offset when to fire


	SCIRQS |= (1 << IRQSCP2);	// reset pending interrupts

	SCIRQM |= (1 << IRQMCP2); //enable 2nd compare interrupt
}

void radiotimer_cancel() {
	*((PORT_RADIOTIMER_WIDTH *)(&SCOCR2LL)) = 0; 	// reset value
	SCOCR2LL = 0;

	SCIRQM &= ~(1<< IRQMCP2); //disable 2nd compare interrupt
}

////===== capture

inline PORT_RADIOTIMER_WIDTH radiotimer_getCapturedTime() {
	return *((PORT_RADIOTIMER_WIDTH *)(&SCCNTLL)) - *((PORT_RADIOTIMER_WIDTH *)(&SCBTSRLL));
}

//=========================== private =========================================

//=========================== interrupt handlers ==============================

/**
\brief TimerB CCR1-6 interrupt service routine
 */
kick_scheduler_t radiotimer_isr() {
	while(1);
}

kick_scheduler_t radiotimer_compare_isr() {
//	print_debug("SCCNTHL %lu \n", radiotimer_getValue());
//	print_debug("SCOCR2 %lu \n", *((PORT_RADIOTIMER_WIDTH *)(&SCOCR2LL)));
//	print_debug("Beacon Timestamp %lu \n",*((PORT_RADIOTIMER_WIDTH *)(&SCBTSRLL)));

	if (radiotimer_vars.compare_cb!=NULL) {
		// call the callback
		radiotimer_vars.compare_cb();
		// kick the OS
		return KICK_SCHEDULER;
	}
	return DO_NOT_KICK_SCHEDULER;
}

kick_scheduler_t radiotimer_overflow_isr() {
//	print_debug("SCCNTHL %lu \n", radiotimer_getValue());
//	print_debug("SCOCR3 %lu \n", radiotimer_getPeriod());
//	print_debug("Beacon Timestamp %lu \n",*((PORT_RADIOTIMER_WIDTH *)(&SCBTSRLL)));

	SCCR0 |= (1 << SCMBTS); // Write 1 to SCMBTS captures the SCCNTH
							// and stores it in the beacon timestamp register
	if (radiotimer_vars.overflow_cb!=NULL) {
		// call the callback
		radiotimer_vars.overflow_cb();
		// kick the OS
		return KICK_SCHEDULER;
	}
	return DO_NOT_KICK_SCHEDULER;
}
