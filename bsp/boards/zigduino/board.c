/**
\brief Zigduino definition of the "leds" bsp module.

\author Sven Akkermans <sven.akkermans@cs.kuleuven.be>, September 2015.
*/

#include <avr/pgmspace.h>
#include <avr/fuse.h>
#include <avr/eeprom.h>

#include "board.h"
// bsp modules
#include "debugpins.h"
#include "leds.h"
#include "uart.h"
#include "spi.h"
#include "bsp_timer.h"
#include "radio.h"
#include "radiotimer.h"


//=========================== variables =======================================

//=========================== prototypes ======================================

//=========================== main ============================================

extern int mote_main(void);

int main(void) {
   return mote_main();
}

//=========================== public ==========================================

void board_init() {
   // disable watchdog timer

   // setup clock speed

   // initialize pins

   // initialize bsp modules
   debugpins_init();
   leds_init();
   uart_init();
   spi_init();
   bsp_timer_init();
   radio_init();
   radiotimer_init();

   // enable interrupts
}

void board_sleep() {
}

void board_reset() {
}

//=========================== private =========================================

//=========================== interrupt handlers ==============================

// DACDMA_VECTOR

// PORT2_VECTOR


// PORT1_VECTOR

// TIMERA1_VECTOR


// ADC12_VECTOR

// USART0TX_VECTOR


// WDT_VECTOR

// TIMERB0_VECTOR

// NMI_VECTOR
