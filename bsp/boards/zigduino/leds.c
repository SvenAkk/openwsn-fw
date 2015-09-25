/**
\brief Zigduino definition of the "leds" bsp module.

\author Sven Akkermans <sven.akkermans@cs.kuleuven.be>, September 2015.
*/

#include <avr/pgmspace.h>
#include <avr/fuse.h>
#include <avr/eeprom.h>

#include "leds.h"

//=========================== defines =========================================

//=========================== variables =======================================

//=========================== prototypes ======================================

//=========================== public ==========================================

void    leds_init() {
}

void    leds_error_on() {
}
void    leds_error_off() {
}
void    leds_error_toggle() {
}
uint8_t leds_error_isOn() {
	   return 0;
}
void leds_error_blink() {
}

void    leds_radio_on() {
}
void    leds_radio_off() {
}
void    leds_radio_toggle() {
}
uint8_t leds_radio_isOn() {
	   return 0;
}

void    leds_sync_on() {
}
void    leds_sync_off() {
}
void    leds_sync_toggle() {
}
uint8_t leds_sync_isOn() {
	   return 0;
}

void    leds_debug_on() {
}
void    leds_debug_off() {
}
void    leds_debug_toggle() {
   // TelosB doesn't have a debug LED :(
}
uint8_t leds_debug_isOn() {
   return 0;
}

void    leds_all_on() {
}
void    leds_all_off() {
}
void    leds_all_toggle() {
}

void    leds_circular_shift() {
}

void    leds_increment() {
}

//=========================== private =========================================
