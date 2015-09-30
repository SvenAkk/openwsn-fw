/**
\brief Zigduino definition of the "leds" bsp module.

\author Sven Akkermans <sven.akkermans@cs.kuleuven.be>, September 2015.
 */

#include <avr/pgmspace.h>
#include <avr/fuse.h>
#include <avr/eeprom.h>
#include <avr/io.h>
#  include <avr/iom128rfa1.h>

#include "leds.h"

//=========================== defines =========================================
/*
 * PORT where LEDs are connected
 */
#define LED_PORT0                        (PORTB)
#define LED_PORT_DIR0                    (DDRB)
#define LED_PORT                        (PORTD)
#define LED_PORT_DIR                    (DDRD)

/*
 * PINs where LEDs are connected
 */
#define LED_PIN_0                       (PB1)
#define LED_PIN_1                       (PD5)
#define LED_PIN_2                       (PD6)

//=========================== variables =======================================

//=========================== prototypes ======================================

//=========================== public ==========================================
#define setb(X,Y) {X |= (1<<Y);}
#define clrb(X,Y) {X &= ~(1<<Y);}
#define togb(X,Y) {X ^= (1<<Y);}

void    leds_init() {
	LED_PORT0 |= (1 << LED_PIN_0);
	LED_PORT_DIR0 |= (1 << LED_PIN_0);
	LED_PORT |= (1 << LED_PIN_1);
	LED_PORT_DIR |= (1 << LED_PIN_1);
	LED_PORT |= (1 << LED_PIN_2);
	LED_PORT_DIR |= (1 << LED_PIN_2);
}

void    leds_error_on() {
	LED_PORT0 |= (1 << LED_PIN_0);
	LED_PORT_DIR0 |= (1 << LED_PIN_0);
	LED_PORT |= (1 << LED_PIN_1);
	LED_PORT_DIR |= (1 << LED_PIN_1);
	LED_PORT |= (1 << LED_PIN_2);
	LED_PORT_DIR |= (1 << LED_PIN_2);
}

void    leds_error_off() {
	LED_PORT0 &= ~(1 << LED_PIN_0);
	LED_PORT_DIR0 |= (1 << LED_PIN_0);
	LED_PORT &= ~(1 << LED_PIN_1);
	LED_PORT_DIR |= (1 << LED_PIN_1);
	LED_PORT &= ~(1 << LED_PIN_2);
	LED_PORT_DIR |= (1 << LED_PIN_2);
}

void    leds_error_toggle() {
	LED_PORT0 &= ~(1 << LED_PIN_0);
	LED_PORT_DIR0 |= (1 << LED_PIN_0);
	LED_PORT &= ~(1 << LED_PIN_1);
	LED_PORT_DIR |= (1 << LED_PIN_1);
	LED_PORT &= ~(1 << LED_PIN_2);
	LED_PORT_DIR |= (1 << LED_PIN_2);
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
