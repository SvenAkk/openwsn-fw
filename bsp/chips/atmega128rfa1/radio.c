/**
\brief Zigduino definition of the "zigduino" bsp module.

\author Sven Akkermans <sven.akkermans@cs.kuleuven.be>, September 2015.
*/
#include "board.h"
#include "radio.h"
#include "atmega128rfa1.h"
#include "spi.h"
#include "debugpins.h"
#include "leds.h"

//=========================== defines =========================================

//=========================== variables =======================================

typedef struct {
   radio_state_t   state;
} radio_vars_t;

radio_vars_t radio_vars;

//=========================== public ==========================================

//===== admin

void radio_init(void) {
   // clear variables
   memset(&radio_vars,0,sizeof(radio_vars_t));

   // change state
   radio_vars.state          = RADIOSTATE_STOPPED;

   // reset radio
   radio_reset();

   // change state
   radio_vars.state          = RADIOSTATE_RFOFF;

   // start radiotimer with dummy setting to activate SFD pin interrupt
   radiotimer_start(0xffff);
}

void radio_setOverflowCb(radiotimer_compare_cbt cb) {
   radiotimer_setOverflowCb(cb);
}

void radio_setCompareCb(radiotimer_compare_cbt cb) {
   radiotimer_setCompareCb(cb);
}

void radio_setStartFrameCb(radiotimer_capture_cbt cb) {
   radiotimer_setStartFrameCb(cb);
}

void radio_setEndFrameCb(radiotimer_capture_cbt cb) {
   radiotimer_setEndFrameCb(cb);
}

//===== reset

void radio_reset(void) {
   volatile uint16_t     delay;

   // set radio VREG pin high

   // set radio RESET pin low

   // set radio RESET pin high

   // disable address recognition

   // speed up time to TX

   // apply correction recommended in datasheet
}

//===== timer

void radio_startTimer(PORT_TIMER_WIDTH period) {
   radiotimer_start(period);
}

PORT_TIMER_WIDTH radio_getTimerValue(void) {
   return radiotimer_getValue();
}

void radio_setTimerPeriod(PORT_TIMER_WIDTH period) {
   radiotimer_setPeriod(period);
}

PORT_TIMER_WIDTH radio_getTimerPeriod(void) {
   return radiotimer_getPeriod();
}

//===== RF admin

void radio_setFrequency(uint8_t frequency) {

   // change state
   radio_vars.state = RADIOSTATE_SETTING_FREQUENCY;


   // change state
   radio_vars.state = RADIOSTATE_FREQUENCY_SET;
}

void radio_rfOn(void) {
}

void radio_rfOff(void) {

   // change state
   radio_vars.state = RADIOSTATE_TURNING_OFF;

   // poipoipoi wait until off

   // wiggle debug pin
   debugpins_radio_clr();
   leds_radio_off();

   // change state
   radio_vars.state = RADIOSTATE_RFOFF;
}

//===== TX

void radio_loadPacket(uint8_t* packet, uint8_t len) {
   // change state
   radio_vars.state = RADIOSTATE_LOADING_PACKET;


   // change state
   radio_vars.state = RADIOSTATE_PACKET_LOADED;
}

void radio_txEnable(void) {
   // change state
   radio_vars.state = RADIOSTATE_ENABLING_TX;

   // wiggle debug pin
   debugpins_radio_set();
   leds_radio_on();

   // I don't fully understand how the CC2420_STXCA the can be used here.

   // change state
   radio_vars.state = RADIOSTATE_TX_ENABLED;
}

void radio_txNow(void) {
   // change state
   radio_vars.state = RADIOSTATE_TRANSMITTING;

}

//===== RX

void radio_rxEnable(void) {
   // change state
   radio_vars.state = RADIOSTATE_ENABLING_RX;

   // put radio in reception mode

   // wiggle debug pin
   debugpins_radio_set();
   leds_radio_on();

   // busy wait until radio really listening

   // change state
   radio_vars.state = RADIOSTATE_LISTENING;
}

void radio_rxNow(void) {
   // nothing to do, the radio is already listening.
}

void radio_getReceivedFrame(
      uint8_t* bufRead,
      uint8_t* lenRead,
      uint8_t  maxBufLen,
      int8_t*  rssi,
      uint8_t* lqi,
      bool*    crc
   ) {

   // read the received packet from the RXFIFO

   // On reception, when MODEMCTRL0.AUTOCRC is set, the CC2420 replaces the
   // received CRC by:
   // - [1B] the rssi, a signed value. The actual value in dBm is that - 45.
   // - [1B] whether CRC checked (bit 7) and LQI (bit 6-0)
   *rssi  =  *(bufRead+*lenRead-2);
   *rssi -= 45;
   *crc   = ((*(bufRead+*lenRead-1))&0x80)>>7;
   *lqi   =  (*(bufRead+*lenRead-1))&0x7f;
}

//=========================== private =========================================

//=========================== callbacks =======================================
