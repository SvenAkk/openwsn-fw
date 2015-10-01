/**
\brief atmega128rfa1-specific board information bsp module.

This module file defines board-related element, but which are applicable only
to this board.

\author Sven Akkermans <sven.akkermans@cs.kuleuven.be>, September 2015.
*/

// Based on the description: CC2538-specific board information bsp module.

#ifndef __BOARD_INFO_H
#define __BOARD_INFO_H

#include <avr/io.h>
#include <avr/iom128rfa1.h>

#include <stdint.h>
#include <string.h>


//=========================== defines =========================================

#define PORT_SIGNED_INT_WIDTH				int32_t

//===== timer

#define PORT_TIMER_WIDTH                    uint32_t
#define PORT_RADIOTIMER_WIDTH               uint32_t

#define PORT_TICS_PER_MS                    33
/*
#define SCHEDULER_WAKEUP()                  radiotimer_isr()
#define SCHEDULER_ENABLE_INTERRUPT()        TIMSK2 |= (1<<OCIE2A)
*/
/*
#define ENABLE_INTERRUPTS()					 __asm__ __volatile__ ("sei" ::: "memory")
#define DISABLE_INTERRUPTS()				 __asm__ __volatile__ ("cli" ::: "memory")
*/
#define ENABLE_INTERRUPTS()
#define DISABLE_INTERRUPTS()

#define SCHEDULER_WAKEUP()                  //do nothing
#define SCHEDULER_ENABLE_INTERRUPT()        // do nothing

//===== IEEE802154E timing
//Sven: based on derfmega

// time-slot related
#define PORT_TsSlotDuration                 491   // counter counts one extra count, see datasheet
// execution speed related
#define PORT_maxTxDataPrepare               66    // 2014us (measured 746us)
#define PORT_maxRxAckPrepare                10    //  305us (measured  83us)
#define PORT_maxRxDataPrepare               33    // 1007us (measured  84us)
#define PORT_maxTxAckPrepare                10    //  305us (measured 219us)
// radio speed related
#define PORT_delayTx                        7     //  214us (measured 219us)
#define PORT_delayRx                        0     //    0us (can not measure)
// radio watchdog

//===== adaptive_sync accuracy

#define SYNC_ACCURACY                       1     // ticks


//=========================== typedef  ========================================

//=========================== variables =======================================

static const uint8_t rreg_uriquery[]        = "h=ucb";
static const uint8_t infoBoardname[]        = "Zigduino";
static const uint8_t infouCName[]           = "Atmega128RFA1";
static const uint8_t infoRadioName[]        = "Atmega128RFA1 SoC";

//=========================== prototypes ======================================

//=========================== public ==========================================

//=========================== private =========================================

#endif
