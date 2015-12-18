/**
\brief atmega128rfa1-specific board information bsp module.

This module file defines board-related element, but which are applicable only
to this board.

\author Sven Akkermans <sven.akkermans@cs.kuleuven.be>, September 2015.
*/

// Based on the description: atmega128RFA1-specific board information bsp module.

#ifndef __BOARD_INFO_H
#define __BOARD_INFO_H

#include <avr/io.h>
#include <avr/iom128rfa1.h>

#include <stdint.h>
#include <string.h>


//=========================== defines =========================================
#define	F_CPU 16000000UL // The clock frequency
#define BAUD 115200 //The baud rate you want.

#include <util/setbaud.h>
#include <util/delay.h>

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
#define INTERRUPT_DECLARATION()          unsigned short s;
#define ENABLE_INTERRUPTS()					 __asm__ __volatile__ ("sei" ::: "memory")
#define DISABLE_INTERRUPTS()				 __asm__ __volatile__ ("cli" ::: "memory")

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


//The timer runs off a 62.5KHZ clock. Define a prescale to make it seem like it is a 32.768KHz clock.
#define TIMER_PRESCALE 1.90734863f // = 62500/32768

#define DEBUG_PRINT_ENABLED 0

#if DEBUG_PRINT_ENABLED == 1
#define print_debug printf
#define print_debug2 printf

#elif DEBUG_PRINT_ENABLED == 2
#define print_debug
#define print_debug2 printf
#else
#define print_debug
#define print_debug2
#endif
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
