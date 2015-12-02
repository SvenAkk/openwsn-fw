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

// we can not print from within the BSP normally, but this is now enabled.
// To separate functionality, we made these 'redundant' functions.
#if DEBUG_PRINT_ENABLED

#include <stdio.h>

void uart_putchar(char c, FILE *stream);
char uart_getchar(FILE *stream);
extern FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
extern FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

void uart_putchar(char c, FILE *stream) {
	if (c == '\n') {
		uart_putchar('\r', stream);
	}
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
	_delay_us(400); //this is bad but only relevant in debugging.
}
char uart_getchar(FILE *stream) {
	loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data exists. */
	return UDR0;
}
#endif

//=========================== variables =======================================

typedef struct {
	uart_tx_cbt txCb;
	uart_rx_cbt rxCb;
} uart_vars_t;

uart_vars_t uart_vars;

//=========================== prototypes ======================================

//=========================== public ==========================================

void uart_init() {
	//PRR0 &= ~(1<<PRUSART0); //enable usart0, according to pg 343

	// reset local variables
	memset(&uart_vars,0,sizeof(uart_vars_t));

	UBRR0H =  UBRRH_VALUE; 	//UBRRnH contains the baud rate
	UBRR0L = UBRRL_VALUE;

	if(USE_2X){
		UCSR0A |= (1<<U2X0);
	}

	UCSR0B = (1<<RXCIE0) | (1<<TXCIE0) // Enable rx&tx interrupt,
							| (1<< RXEN0) | (1<<TXEN0);	// enable rx&tx
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);	// async usart, no parity, 1-bit stop, 8-bit mode


#if DEBUG_PRINT_ENABLED
	stdout  = &uart_output;
	stdin = &uart_input;
#endif

	if(USE_2X){
		print_debug("USE_2X used");
	}
}

void uart_setCallbacks(uart_tx_cbt txCb, uart_rx_cbt rxCb) {
	uart_vars.txCb = txCb;
	uart_vars.rxCb = rxCb;
}

void    uart_enableInterrupts(){
	UCSR0B |= (1<<RXCIE0) | (1<<TXCIE0); // == 0xC0
}

void    uart_disableInterrupts(){
	UCSR0B &= ~((1<<RXCIE0) | (1<<TXCIE0)); // == ~0xC0
}

void    uart_clearRxInterrupts(){
	UCSR0A |= (1<<TXC0);
}

void    uart_clearTxInterrupts(){
	UCSR0A |=  (1<<TXC0);
}

void    uart_writeByte(uint8_t byteToWrite){
	UDR0 = byteToWrite;
}

uint8_t uart_readByte(){
//	loop_until_bit_is_set(UCSR0A,UDRE0);
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
	while (UCSR0A & (1<<RXC0)) {dummy = UDR0;}
	return 0;
}
