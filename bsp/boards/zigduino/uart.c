/**
\brief Zigduino definition of the "uart" bsp module.

\author Sven Akkermans <sven.akkermans@cs.kuleuven.be>, September 2015.
 */
#include <avr/pgmspace.h>
#include <avr/fuse.h>
#include <avr/eeprom.h>
#include <avr/io.h>
#include <avr/iom128rfa1.h> //Sven: this is advised against, but works.
#include <stdio.h>
#include <util/delay.h>


#include "uart.h"
#include "board.h"


//=========================== defines =========================================
#define	F_CPU 16000000UL // The clock frequency
#define BAUD 57600 //The baud rate you want

#include <util/setbaud.h>

// we can not print from within the BSP normally, but this is now enabled.
// To separate functionality, we made these 'redundant' functions.
void uart_putchar(char c, FILE *stream);
char uart_getchar(FILE *stream);
extern FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
extern FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

//=========================== variables =======================================

typedef struct {
	uart_tx_cbt txCb;
	uart_rx_cbt rxCb;
} uart_vars_t;

uart_vars_t uart_vars;

//=========================== prototypes ======================================

//=========================== public ==========================================

void uart_init() {
	PRR0 &= ~(1<<PRUSART0); //enable usart0, according to pg 343

	// reset local variables
	memset(&uart_vars,0,sizeof(uart_vars_t));

	UBRR0H =  UBRRH_VALUE; 	//UBRRnH contains the baud rate
	UBRR0L = UBRRL_VALUE;

    UCSR0A |= (1 << U2X0);	//use 2x
	UCSR0C = 0b00000110;	// enable async usart, disabled parity, 1-bit stop, 8-bit mode and async
	UCSR0B = 0b11011000;	// Enable rx&tx interrupt, disable empty interrupt, enable rx&tx

	// we can not print from within the BSP normally, but this is now enabled.
	// To separate functionality, we made these 'redundant' functions.
    stdout = &uart_output;
    stdin  = &uart_input;
}

void uart_setCallbacks(uart_tx_cbt txCb, uart_rx_cbt rxCb) {
	uart_vars.txCb = txCb;
	uart_vars.rxCb = rxCb;
}

void    uart_enableInterrupts(){
	UCSR0B |= 0b11000000; // == 0xC0
}

void    uart_disableInterrupts(){
	UCSR0B &= ~0b11000000; // == ~0xC0
}

void    uart_clearRxInterrupts(){
	UCSR0A |= 0x20;
}

void    uart_clearTxInterrupts(){
	UCSR0A |= 0x40;
}

void    uart_writeByte(uint8_t byteToWrite){
	//while((UCSR0A & (1 << UDRE0))==0); /* Wait until data register empty. */
	loop_until_bit_is_set(UCSR0A,UDRE0);
	UDR0 = byteToWrite;

	loop_until_bit_is_set(UCSR0A, TXC0);
	UCSR0A |= _BV(TXC0);
}

uint8_t uart_readByte(){
	loop_until_bit_is_set(UCSR0A,UDRE0);
	return UDR0;
}

// we can not print from within the BSP normally, but this is now enabled.
// To separate functionality, we made these 'redundant' functions.
void uart_putchar(char c, FILE *stream) {
    if (c == '\n') {
        uart_putchar('\r', stream);
    }
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
    _delay_us(4000); //this is bad but only relevant in debugging.
}
char uart_getchar(FILE *stream) {
    loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data exists. */
    return UDR0;
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
