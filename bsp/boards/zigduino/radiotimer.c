/**
\brief Zigduino definition of the "eui64" bsp module.

\author Sven Akkermans <sven.akkermans@cs.kuleuven.be>, September 2015.
*/

#include <avr/pgmspace.h>
#include <avr/fuse.h>
#include <avr/eeprom.h>

#include "radiotimer.h"
#include "leds.h"

//=========================== variables =======================================

typedef struct {
   radiotimer_compare_cbt    overflowCb;
   radiotimer_compare_cbt    compareCb;
   radiotimer_capture_cbt    startFrameCb;
   radiotimer_capture_cbt    endFrameCb;
   uint8_t                   f_SFDreceived;
} radiotimer_vars_t;

radiotimer_vars_t radiotimer_vars;

//=========================== prototypes ======================================

//=========================== public ==========================================

//===== admin

void radiotimer_init() {
   // clear local variables
   memset(&radiotimer_vars,0,sizeof(radiotimer_vars_t));
}

void radiotimer_setOverflowCb(radiotimer_compare_cbt cb) {
   radiotimer_vars.overflowCb     = cb;
}

void radiotimer_setCompareCb(radiotimer_compare_cbt cb) {
   radiotimer_vars.compareCb      = cb;
}

void radiotimer_setStartFrameCb(radiotimer_capture_cbt cb) {
   radiotimer_vars.startFrameCb   = cb;
}

void radiotimer_setEndFrameCb(radiotimer_capture_cbt cb) {
   radiotimer_vars.endFrameCb     = cb;
}

void radiotimer_start(PORT_RADIOTIMER_WIDTH period) {
}

//===== direct access

PORT_RADIOTIMER_WIDTH radiotimer_getValue() {
   return 0;
}

void radiotimer_setPeriod(PORT_RADIOTIMER_WIDTH period) {
}

PORT_RADIOTIMER_WIDTH radiotimer_getPeriod() {
   return 0;
}

//===== compare

void radiotimer_schedule(PORT_RADIOTIMER_WIDTH offset) {
}

void radiotimer_cancel() {
}

//===== capture

inline PORT_RADIOTIMER_WIDTH radiotimer_getCapturedTime() {
   // this should never happpen!

   // we can not print from within the BSP. Instead:
   // blink the error LED
   leds_error_blink();
   // reset the board
   board_reset();

   return 0;// this line is never reached, but here to satisfy compiler
}

//=========================== private =========================================

//=========================== interrupt handlers ==============================

/**
\brief TimerB CCR1-6 interrupt service routine
*/
kick_scheduler_t radiotimer_isr() {
   return DO_NOT_KICK_SCHEDULER;
}
