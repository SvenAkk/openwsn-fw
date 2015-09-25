/**
\brief atmega128rfa1-specific board information bsp module.

This module file defines board-related element, but which are applicable only
to this board.

\author Sven Akkermans <sven.akkermans@cs.kuleuven.be>, September 2015.
*/

// Based on the description: CC2538-specific board information bsp module.

#ifndef __BOARD_INFO_H
#define __BOARD_INFO_H


#include <stdint.h>
#include <string.h>

//=========================== defines =========================================

//===== interrupt state

#define INTERRUPT_DECLARATION()
#define DISABLE_INTERRUPTS() IntMasterDisable()

#define ENABLE_INTERRUPTS() IntMasterEnable()

//===== timer

#define PORT_TIMER_WIDTH                    uint32_t
#define PORT_RADIOTIMER_WIDTH               uint32_t

#define PORT_SIGNED_INT_WIDTH               int32_t
#define PORT_TICS_PER_MS                    33

// on GINA, we use the comparatorA interrupt for the OS
#define SCHEDULER_WAKEUP()
#define SCHEDULER_ENABLE_INTERRUPT()

// this is a workaround from the fact that the interrupt pin for the GINA radio
// is not connected to a pin on the MSP which allows time capture.
#define CAPTURE_TIME()

/* sleep timer interrupt */
#define HAL_INT_PRIOR_ST        (4 << 5)
/* MAC interrupts */
#define HAL_INT_PRIOR_MAC       (4 << 5)
/* UART interrupt */
#define HAL_INT_PRIOR_UART      (5 << 5)

//===== pinout

// [P4.7] radio SLP_TR_CNTL
#define PORT_PIN_RADIO_SLP_TR_CNTL_HIGH()
#define PORT_PIN_RADIO_SLP_TR_CNTL_LOW()
// radio reset line
// on cc2538, the /RST line is not connected to the uC
#define PORT_PIN_RADIO_RESET_HIGH()    // nothing
#define PORT_PIN_RADIO_RESET_LOW()     // nothing

//===== IEEE802154E timing
#ifdef GOLDEN_IMAGE_ROOT
// time-slot related
#define PORT_TsSlotDuration                 328   // counter counts one extra count, see datasheet
// execution speed related
#define PORT_maxTxDataPrepare               10    //  305us (measured  82us)
#define PORT_maxRxAckPrepare                10    //  305us (measured  83us)
#define PORT_maxRxDataPrepare                4    //  122us (measured  22us)
#define PORT_maxTxAckPrepare                10    //  122us (measured  94us)
// radio speed related
#ifdef L2_SECURITY_ACTIVE
#define PORT_delayTx                         7    //  366us (measured xxxus)
#else
#define PORT_delayTx                        12    //  366us (measured xxxus)
#endif
#define PORT_delayRx                         0    //    0us (can not measure)
// radio watchdog
#else
// time-slot related
#define PORT_TsSlotDuration                 492   // counter counts one extra count, see datasheet
// execution speed related
#define PORT_maxTxDataPrepare               66    // 2014us (measured 746us)
#define PORT_maxRxAckPrepare                10    //  305us (measured  83us)
#define PORT_maxRxDataPrepare               33    // 1007us (measured  84us)
#define PORT_maxTxAckPrepare                22    //  305us (measured 219us)
// radio speed related
#define PORT_delayTx                        12    //  214us (measured 219us)
#define PORT_delayRx                        0     //    0us (can not measure)
// radio watchdog
#endif

//===== adaptive_sync accuracy

#define SYNC_ACCURACY                       1     // ticks

//===== per-board number of sensors

#define NUMSENSORS 7

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
