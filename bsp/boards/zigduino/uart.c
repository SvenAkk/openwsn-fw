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
    UBRR1 = ((F_CPU/16)/BAUD) - 1;

    // Enable receive and transmit
    UCSR1B = (1 << RXEN1) | (1 << TXEN1);

    // Set 8 data bits (UCSZ12:10 = 011) and 2 stop bits (USBS1 = 1)
    UCSR1C = (1 << UCSZ11) | (1 << UCSZ10) | (1 << USBS1);

    // Configure the MAX3221E: PD7 = !FORCEOFF, PD6 = FORCEON, PD4 = !EN
    DDRD = (1 << PD7) | (1 << PD6) | (1 << PD4);

    // Force the MAX3221E on: !FORCEOFF = 1, FORCEON = 1, !EN = 0
    PORTD = (1 << PD7) | (1 << PD6);

//	PRR1 &= ~(1<<PRUSART1);
//
//	/* Set baud rate */
//	UBRR0H = (unsigned char)(CLOCKGEN>>8);
//	UBRR0L = (unsigned char) CLOCKGEN;
//	/* Enable receiver and transmitter */
//	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
//	/* Set frame format: 8data, 2stop bit */
//	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

void uart_setCallbacks(uart_tx_cbt txCb, uart_rx_cbt rxCb) {
   uart_vars.txCb = txCb;
   uart_vars.rxCb = rxCb;
}

void    uart_enableInterrupts(){
}

void    uart_disableInterrupts(){
}

void    uart_clearRxInterrupts(){
}

void    uart_clearTxInterrupts(){
}

void    uart_writeByte(uint8_t byteToWrite){
}

uint8_t uart_readByte(){
  return 0;
}

//=========================== private =========================================

//=========================== interrupt handlers ==============================

kick_scheduler_t uart_tx_isr() {
   uart_clearTxInterrupts(); // TODO: do not clear, but disable when done
   uart_vars.txCb();
   return DO_NOT_KICK_SCHEDULER;
}

kick_scheduler_t uart_rx_isr() {
   uart_clearRxInterrupts(); // TODO: do not clear, but disable when done
   uart_vars.rxCb();
   return DO_NOT_KICK_SCHEDULER;
}
