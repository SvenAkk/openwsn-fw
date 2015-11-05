/**
\brief Zigduino definition of the "leds" bsp module.

\author Sven Akkermans <sven.akkermans@cs.kuleuven.be>, September 2015.
 */

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/iom128rfa1.h>
#include <avr/wdt.h>


#include "board.h"

// bsp modules
#include "debugpins.h"
#include "leds.h"
#include "uart.h"
#include "spi.h"
#include "bsp_timer.h"
#include "radio.h"
#include "radiotimer.h"

//=========================== variables =======================================

//=========================== prototypes ======================================
extern uint8_t radio_rx_start_isr();
extern uint8_t radio_trx_end_isr();
extern uint8_t radiotimer_compare_isr();
extern uint8_t radiotimer_overflow_isr();

//=========================== main ============================================
uint8_t mcusr_backup;

extern int mote_main(void);

int main(void) {
	mcusr_backup = MCUSR;
	MCUSR = 0;
	return mote_main();
}

//=========================== public ==========================================

void board_init() {

	// Print reboot reason
	if(mcusr_backup & (1<<PORF )) printf("Power-on reset.\n");
	if(mcusr_backup & (1<<EXTRF)) printf("External reset!\n");
	if(mcusr_backup & (1<<BORF )) printf("Brownout reset!\n");
	if(mcusr_backup & (1<<WDRF )) printf("Watchdog reset!\n");
	if(mcusr_backup & (1<<JTRF )) printf("JTAG reset!\n");

	printf("\n*******Booting Zigduino with OpenWSN*******\n");

	// setup clock speed

	//	// initialize pins
	//	// turn off power to all periphrals ( will be enabled specifically later)
	//	PRR0 = 0x00;
	//	PRR1 = 0x00;
	// enable data retention
	//	DRTRAM0 |= 0x10;
	//	DRTRAM1 |= 0x10;
	//	DRTRAM2 |= 0x10;
	//	DRTRAM3 |= 0x10;

	//disable interrupts
	cli();

	wdt_reset();
	wdt_disable();

	// initialize bsp modules
	debugpins_init();
	leds_init();
	uart_init();
	spi_init();
	bsp_timer_init();
	radio_init();
	radiotimer_init();

	// enable interrupts
	sei();
}

// Uses high-level functions from avr/sleep.h
void board_sleep() {
	loop_until_bit_is_set(UCSR0A, TXC0);
	UCSR0A |= (1 << TXC0); //Ensure messages are sent before the sleep

	TRXPR = 1 << SLPTR; // sent transceiver to sleep
	set_sleep_mode(SLEEP_MODE_IDLE); // Power save mode to allow Timer/counter2 interrupts, see pg 162
	sleep_mode();
}

void board_reset() {
    MCUSR = 0;
    WDTCSR |= (1 << WDCE) | (1 << WDE);
    WDTCSR = 0x00;
}
//=========================== private =========================================

//=========================== interrupt handlers ==============================

// Based from the derfmega and from avr/interrupt.h because this macro
// is defined elsewhere also, this overrides all
#ifdef __cplusplus
#  define ISR(vector, ...)            \
		extern "C" void vector (void) __attribute__ ((signal,__INTR_ATTRS)) __VA_ARGS__; \
		void vector (void)
#else
#  define ISR(vector, ...)            \
		void vector (void) __attribute__ ((signal,__INTR_ATTRS)) __VA_ARGS__; \
		void vector (void)
#endif

// UART0 interrupt
// pass to uart_isr_rx/tx
ISR(USART0_RX_vect) {
	//printf("USART0_RX_vect ISR raised. \n");
	uart_rx_isr(); // doing nothing w/ return value

}

ISR(USART0_TX_vect) {
	//printf("USART0_TX_vect ISR raised. \n");
	uart_tx_isr(); // doing nothing w/ return value
}
// radio interrupt(s)
// pass to radio_isr
ISR(TRX24_RX_START_vect) {
	//printf("TRX24_RX_START_vect ISR raised. \n");
	radio_rx_start_isr(); // doing nothing w/ return value
}

ISR(TRX24_RX_END_vect) {
	//printf("TRX24_RX_END_vect ISR raised. \n");
	radio_trx_end_isr();
}
ISR(TRX24_TX_END_vect) {
	//printf("TRX24_TX_END_vect ISR raised. \n");
	radio_trx_end_isr();
}

ISR(SCNT_CMP1_vect) {
	printf("SCNT_CMP1_vect ISR raised. \n");
	radiotimer_compare_isr();
}

ISR(SCNT_CMP2_vect) {
	printf("SCNT_CMP2_vect ISR raised. \n");
	radiotimer_compare_isr();
}

ISR(SCNT_CMP3_vect) {
	printf("SCNT_CMP3_vect ISR raised. \n");
	radiotimer_overflow_isr();
}

ISR (TIMER1_COMPA_vect) {
	printf("TIMER1_COMPA_vect ISR raised. \n");
	//TIMSK1 &= ~((1<<ICIE1)|(1<<OCIE1A)|(1<<OCIE1B)|(1<<TOIE1));
	radiotimer_overflow_isr();
}

ISR (TIMER1_COMPB_vect) {
	printf("TIMER1_COMPB_vect ISR raised. \n");
	//TIMSK1 &= ~((1<<ICIE1)|(1<<OCIE1A)|(1<<OCIE1B)|(1<<TOIE1));
	radiotimer_compare_isr();
}

ISR(WDT_vect) {
	printf("WDT_vect ISR raised. \n");
	bsp_timer_scheduleIn(bsp_timer_get_currentValue()); // timer will subtract this from set time
}
// buttons (none)


// error
ISR(BADISR_vect) {
	static const char msg[] = "BADISR\n";
	char c = 0;
	while(1) {
		for (c = 0; c < sizeof(msg); c++) {
			uart_writeByte(msg[c]);
		}
	}
}
