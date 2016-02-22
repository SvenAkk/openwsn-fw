/**
\brief This project runs the full OpenWSN stack.

\author Thomas Watteyne <watteyne@eecs.berkeley.edu>, August 2010
*/

#include "board.h"
#include "crypto_engine.h"
#include "scheduler.h"
#include "openstack.h"
#include "opendefs.h"

int mote_main(void) {
   
   // initialize
//   lightweight_board_init();
   board_init();
   scheduler_init();
   lightweight_openstack_init();
   //openstack_init();
   // indicate
   mote_main2();
   // start
   scheduler_start();
   return 0; // this line should never be reached
}

#include "stdint.h"
#include "stdio.h"
#include "string.h"
// bsp modules required
#include "board.h"
#include "uart.h"
#include "bsp_timer.h"
#include "leds.h"

//=========================== defines =========================================

#define BSP_TIMER_PERIOD     0xffff // 0xffff@32kHz = 2s


//=========================== prototypes ======================================

void cb_compare(void);
void cb_uartTxDone(void);
void cb_uartRxCb(void);

//=========================== main ============================================

/**
\brief The program starts executing here.
*/
int mote_main2(void) {
   // setup UART
   uart_setCallbacks(cb_uartTxDone,cb_uartRxCb);
   uart_enableInterrupts();

   // setup BSP timer
   bsp_timer_set_callback(cb_compare);
   bsp_timer_scheduleIn(BSP_TIMER_PERIOD);

}

//=========================== callbacks =======================================

void cb_compare(void) {
   uart_writeByte('H');
   // schedule again
   bsp_timer_scheduleIn(BSP_TIMER_PERIOD);
}

void cb_uartTxDone(void) {
}

void cb_uartRxCb(void) {
   uint8_t byte;

   // toggle LED
   leds_all_toggle();

   // read received byte
   byte = uart_readByte();

   // echo that byte over serial
   uart_writeByte(byte);
}
