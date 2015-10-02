/**
\brief Zigduino definition of the "uart" bsp module.

\author Sven Akkermans <sven.akkermans@cs.kuleuven.be>, September 2015.
 */
#include <avr/pgmspace.h>
#include <avr/fuse.h>
#include <avr/eeprom.h>
#include <avr/io.h>
#include <avr/iom128rfa1.h> //Sven: this is advised against, but works.


#include "uart.h"
#include "board.h"


//=========================== defines =========================================
#define	F_CPU 16000000UL // The clock frequesncy
#define BAUD 19200 //The baud rate you want

//this next line calculates the values for the Baud Rate Generator
#define CLOCKGEN (((unsigned long)FOSC/(16UL*(unsigned long)BAUD))-1)
//=========================== variables =======================================

//FUSES = {
//  .low = FUSE_CKSEL3,
//  .high = (FUSE_SPIEN & FUSE_EESAVE),
//  .extended = (FUSE_BODLEVEL1 & ~_BV(3)), /* 128rfa1 has an unused extended fuse bit which is immutable */
//};

typedef struct {
	uart_tx_cbt txCb;
	uart_rx_cbt rxCb;
} uart_vars_t;

uart_vars_t uart_vars;

//=========================== prototypes ======================================

//=========================== public ==========================================

void uart_init() {
	PRR0 &= ~(1<<PRUSART0); //According to pg 343

	// Asynchronous normal mode (U2X1 = 0): (Fosc/(16 * Baud)) - 1
	UBRR0 = ((F_CPU/16)/BAUD) - 1;

	// Enable receive and transmit
	UCSR0B = (1 << RXEN1) | (1 << TXEN1);

	// Set 8 data bits (UCSZ12:10 = 011) and 2 stop bits (USBS1 = 1)
	UCSR0C = (1 << UCSZ11) | (1 << UCSZ10) | (1 << USBS1);
}

void uart_setCallbacks(uart_tx_cbt txCb, uart_rx_cbt rxCb) {
	uart_vars.txCb = txCb;
	uart_vars.rxCb = rxCb;
}

void    uart_enableInterrupts(){
	UCSR0B |= 0xC0;
}

void    uart_disableInterrupts(){
	UCSR0B &= ~0xC0;
}

void    uart_clearRxInterrupts(){
	UCSR0A |= 0x20;
}

void    uart_clearTxInterrupts(){
	UCSR0A |= 0x40;
}

void    uart_writeByte(uint8_t byteToWrite){
	while((UCSR0A & _BV(UDRE0))==0);
	UDR0 = byteToWrite;
}

uint8_t uart_readByte(){
	return 0;
}

//=========================== private =========================================

//=========================== interrupt handlers ==============================

kick_scheduler_t uart_tx_isr() {
	if(uart_vars.txCb)
		uart_vars.txCb();
	return 0;
}

kick_scheduler_t uart_rx_isr() {
	char dummy;
	if (uart_vars.rxCb)
		uart_vars.rxCb();
	// make sure buffer was read
	if (RXC0) {dummy = UDR0;}
	return 0;
}
