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
#include <avr/wdt.h>
#include <avr/interrupt.h>


//=========================== define ==========================================

//Macros for watchdog timer (modified from wdt.h)
#define wdt_enable_int_only(value)   \
		__asm__ __volatile__ (  \
				"in __tmp_reg__,__SREG__" "\n\t"    \
				"cli" "\n\t"    \
				"wdr" "\n\t"    \
				"sts %0,%1" "\n\t"  \
				"out __SREG__,__tmp_reg__" "\n\t"   \
				"sts %0,%2" "\n\t" \
				: /* no outputs */  \
		: "M" (_SFR_MEM_ADDR(_WD_CONTROL_REG)), \
		  "r" (_BV(_WD_CHANGE_BIT) | _BV(WDE)), \
		  "r" ((uint8_t) ((value & 0x08 ? _WD_PS3_MASK : 0x00) | \
				  _BV(WDIE) | (value & 0x07)) ) \
				  : "r0"  \
		)
//=========================== variables =======================================

typedef struct {
	bsp_timer_cbt    cb;
	PORT_TIMER_WIDTH time_set;
	PORT_TIMER_WIDTH time_left;
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
	// clear local variables
	memset(&bsp_timer_vars,0,sizeof(bsp_timer_vars_t));

	MCUSR &=~(1<<WDRF);	//  Clear startup bit and disable the wdt, whether or not it will be used.
	WDTCSR = (0<<WDIE);	// Disable watchdog timeout interrupt enable

	//	PRR0 &= ~(1<<PRTIM0); // turn on timer 2 for crystal
	//
	//	/* Disable TIMER2 interrupt */
	//	TIMSK2 &= ~(1 << OCIE2A);
	//	ASSR = 0x00;
	//	TCNT2 = 0;	// Set counter to zero
	//	while(ASSR & (1<<TCN2UB)) {};
	//
	//	TCCR2A = (0<<COM2A1) | (0<<COM2A0) | (0<<COM2B1) | (0<<COM2B0) | (1<<WGM21) | (0<<WGM20);
	//	TCCR2B = (0<<WGM22) |(1<<CS22)|(1<<CS21)|(1<<CS20); //prescale: 128 for timer2
	//	while(ASSR & (1<<TCR2BUB)) {}; // check if busy

	//	TCNT0 = 0;
	//	TCCR0A = (1 << WGM01);
	//	TCCR0B =  (1<<CS02)|(0<<CS01)|(1<<CS00); //prescale: 1024 for timer0
	//	/* Clear interrupt flag register */ \
	//	TIFR0 = TIFR0; \

	//	PRR0 &= ~(1<<PRTIM2); // turn on timer 2 for crystal
	//	SCCR0 = (SCCR0 | 0b00110000) & 0b11111110; // enable symbol counter, 32KHz clock, absolute compare 1
	//	SCCR1 = 0; // no backoff slot counter
	//	ASSR |= (1<<AS2); // enable 32KHz crystal
	//
	//	//set compare1 registers
	//	SCOCR1HH = SCOCR1HL = SCOCR1LH = SCOCR1LL = 0;
	//
	//	// don't enable interrupts until first compare is set
	//
	//	// wait for register writes
	//	while(SCSR & 0x01);
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
	wdt_reset();
	// record last timer compare value
	bsp_timer_vars.time_left =  0;
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
//	printf("bsp_timer_vars.time_set: %lu \n", bsp_timer_vars.time_set);
//	printf("bsp_timer_vars.time_left: %lu \n", bsp_timer_vars.time_left);
//	printf("delayTicks: %lu \n", delayTicks);

	if(bsp_timer_vars.time_set == 0){
		bsp_timer_vars.time_set = delayTicks;
	} else {
		delayTicks = bsp_timer_vars.time_set - delayTicks; //regain remaining delayticks.
	}
	if(delayTicks <= 2048){ //wdt can't count this small, trigger immediate
		printf("Trigger immediate! \n");
		bsp_timer_isr();
	} else{
		uint32_t countedCycles = 2048; //smallest possible cycles
		int wdt_count = 0;
		while(countedCycles*2 <= delayTicks && countedCycles != 1048576){ //1048576 is the max time the wdt can count
//			printf("countedCycles: %lu \n", countedCycles);
			wdt_count++;
			countedCycles = countedCycles*2;
		}
//		printf("countedCycles: %lu \n", countedCycles);
//		printf("delayTicks: %lu \n", delayTicks);
		bsp_timer_vars.time_left = delayTicks - countedCycles;

//		printf("timeleft: %lu \n", bsp_timer_vars.time_left);
//		printf("wdt_count:  %d \n", wdt_count);

		wdt_enable_int_only(wdt_count); //Macro sets things up for a reset
		WDTCSR |= (1 << WDIE);
	}
	//	PORT_TIMER_WIDTH newCompareValue;
	//	PORT_TIMER_WIDTH temp_last_compare_value;
	//	PORT_TIMER_WIDTH current_value;
	//
	//	temp_last_compare_value = bsp_timer_vars.last_compare_value;
	//
	//	newCompareValue      =  bsp_timer_vars.last_compare_value + delayTicks;
	//	bsp_timer_vars.last_compare_value   =  newCompareValue;
	//
	//	current_value = bsp_timer_get_currentValue();
	//
	//	printf("\n\n");
	//	printf("delayTicks: %lu\n", delayTicks);
	//	printf("current value: %lu\n", current_value);
	//	printf("newCompareValue: %lu\n", newCompareValue);
	//	printf("temp_last_compare_value: %lu\n", temp_last_compare_value);
	//	PORT_TIMER_WIDTH passed_time = current_value - temp_last_compare_value;
	//	printf("passed_time: %lu\n",passed_time);
	//
	//	if (current_value > temp_last_compare_value && delayTicks < current_value - temp_last_compare_value) {
	//		// we're already too late, schedule the ISR right now manually
	//		// setting the interrupt flag triggers an interrupt
	//		printf("Too late, triggered interrupt \n");
	//		bsp_timer_isr();
	//
	//	} else {
	//		printf("Set compare, enabled interrupt\n");
	//		//		OCR2A  =  newCompareValue;
	//		//		TIMSK2 |= (1 << OCIE2A); // Enable TIMER2 output compare interrupt
	//		//		//  while(ASSR & (1<<OCR2AUB));
	//
	//		//		OCR0A  =  newCompareValue;
	//		//		TIMSK0 |= (1 << OCIE0A); // Enable TIMER0 output compare interrupt
	//
	//		// this is the normal case, have timer expire at newCompareValue
	//		SCOCR1HH  =  (uint8_t)(newCompareValue>>24);
	//		SCOCR1HL  =  (uint8_t)(newCompareValue>>16);
	//		SCOCR1LH  =  (uint8_t)(newCompareValue>>8);
	//		SCOCR1LL  =  (uint8_t)(newCompareValue);
	//		// enable interrupts
	//		SCIRQM |= 0x01;
	//
	//	}
}

/**
\brief Cancel a running compare.
 */
void bsp_timer_cancel_schedule(){
	bsp_timer_vars.time_set = 0;
	bsp_timer_vars.time_left = 0;
	wdt_disable();
}

/**
\brief Return the current value of the timer's counter.

\returns The current value of the timer's counter.
 */
PORT_TIMER_WIDTH   bsp_timer_get_currentValue(){
	return bsp_timer_vars.time_set-bsp_timer_vars.time_left;
}

//=========================== private =========================================

//=========================== interrupt handlers ===============================
kick_scheduler_t   bsp_timer_isr(){
	bsp_timer_cancel_schedule();
	// call the callback
	bsp_timer_vars.cb();
	// kick the OS
	return 1;
}


