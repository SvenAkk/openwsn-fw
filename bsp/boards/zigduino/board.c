/**
\brief Zigduino definition of the "leds" bsp module.

\author Sven Akkermans <sven.akkermans@cs.kuleuven.be>, September 2015.
 */

#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/iom128rfa1.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>


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
	// disable watchdog timer
	wdt_reset();
	wdt_disable();

	// Print reboot reason
	if(mcusr_backup & (1<<PORF )) printf("Power-on reset.\n");
	if(mcusr_backup & (1<<EXTRF)) printf("External reset!\n");
	if(mcusr_backup & (1<<BORF )) printf("Brownout reset!\n");
	if(mcusr_backup & (1<<WDRF )) printf("Watchdog reset!\n");
	if(mcusr_backup & (1<<JTRF )) printf("JTAG reset!\n");

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

	print_debug("\n*******Booted Zigduino with OpenWSN*******\n");

}

// Uses high-level functions from avr/sleep.h
void board_sleep() {
	//	TRXPR = (0 << SLPTR); // sent transceiver to sleep
	set_sleep_mode(SLEEP_MODE_PWR_SAVE); // Power save mode
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
#define ISR(vector, ...)            \
		void vector (void) __attribute__ ((signal,__INTR_ATTRS)) __VA_ARGS__; \
		void vector (void)

// UART0 interrupt
// pass to uart_isr_rx/tx
ISR(USART0_RX_vect) {
	//	print_debug("USART0_RX_vect ISR raised. \n");
    debugpins_isr_set();
    uart_rx_isr(); // doing nothing w/ return value
    debugpins_isr_clr();
}

ISR(USART0_TX_vect) {
	//	print_debug("USART0_TX_vect ISR raised. \n");
    debugpins_isr_set();
	uart_tx_isr(); // doing nothing w/ return value
    debugpins_isr_clr();
}
// radio interrupt(s)
// pass to radio_isr
ISR(TRX24_RX_START_vect) {
//	print_debug("TRX24_RX_START_vect ISR raised. \n");
	//debugpins_isr_set();
	radio_rx_start_isr(); // doing nothing w/ return value
	//debugpins_isr_clr();
}

ISR(TRX24_RX_END_vect) {
//	print_debug("TRX24_RX_END_vect ISR raised. \n");
	//debugpins_isr_set();
	radio_trx_end_isr();
	//debugpins_isr_clr();
}
ISR(TRX24_TX_END_vect) {
//	print_debug("TRX24_TX_END_vect ISR raised. \n");
	//debugpins_isr_set();
	radio_trx_end_isr();
	//debugpins_isr_clr();
}

ISR(SCNT_CMP1_vect) {
	//	print_debug("SCNT_CMP1_vect ISR raised. \n");
	//debugpins_isr_set();
	bsp_timer_isr();
	//debugpins_isr_clr();
}

ISR(SCNT_CMP2_vect) {
	//	print_debug("SCNT_CMP2_vect ISR raised. \n");
	//debugpins_isr_set();
	radiotimer_compare_isr();
	//debugpins_isr_clr();
}

ISR(SCNT_CMP3_vect) {
	//	print_debug("SCNT_CMP3_vect ISR raised. \n");
	//debugpins_isr_set();
	radiotimer_overflow_isr();
	//debugpins_isr_clr();
}

ISR(SCNT_OVFL_vect) {
	//print_debug("SCNT_OVFL_vect ISR raised. Should be unused. \n");
	//debugpins_isr_set();
	//radiotimer_overflow_isr();
	//debugpins_isr_clr();
}

/* Hang on any unsupported interrupt */
/* Useful for diagnosing unknown interrupts that reset the mcu.
 * Currently set up for 12mega128rfa1.
 * For other mcus, enable all and then disable the conflicts.
 */
ISR( _VECTOR(0)) {print_debug("0 ISR raised. \n");}
ISR( _VECTOR(1)) {print_debug("1 ISR raised. \n");}
ISR( _VECTOR(2)) {print_debug("2 ISR raised. \n");}
ISR( _VECTOR(3)) {print_debug("3 ISR raised. \n");}
ISR( _VECTOR(4)) {print_debug("4 ISR raised. \n");}
ISR( _VECTOR(5)) {print_debug("5 ISR raised. \n");}
ISR( _VECTOR(6)) {print_debug("6 ISR raised. \n");}
ISR( _VECTOR(7)) {print_debug("7 ISR raised. \n");}
ISR( _VECTOR(8)) {print_debug("8 ISR raised. \n");}
ISR( _VECTOR(9)) {print_debug("9 ISR raised. \n");}
ISR( _VECTOR(10)) {print_debug("10 ISR raised. \n");}
ISR( _VECTOR(11)) {print_debug("11 ISR raised. \n");}
ISR( _VECTOR(12)) {print_debug("12 ISR raised. \n");}
ISR( _VECTOR(13)) {print_debug("13 ISR raised. \n");}
ISR( _VECTOR(14)) {print_debug("14 ISR raised. \n");}
ISR( _VECTOR(15)) {print_debug("15 ISR raised. \n");}
ISR( _VECTOR(16)) {print_debug("16 ISR raised. \n");}
ISR( _VECTOR(17)) {print_debug("17 ISR raised. \n");}
ISR( _VECTOR(18)) {print_debug("18 ISR raised. \n");}
ISR( _VECTOR(19)) {print_debug("19 ISR raised. \n");}
ISR( _VECTOR(20)) {print_debug("20 ISR raised. \n");}
ISR( _VECTOR(21)) {print_debug("21 ISR raised. \n");}
ISR( _VECTOR(22)) {print_debug("22 ISR raised. \n");}
ISR( _VECTOR(23)) {print_debug("23 ISR raised. \n");}
ISR( _VECTOR(24)) {print_debug("24 ISR raised. \n");}
//ISR( _VECTOR(25)) {print_debug("25 ISR raised. \n");}
ISR( _VECTOR(26)) {print_debug("26 ISR raised. \n");}
//ISR( _VECTOR(27)) {print_debug("27 ISR raised. \n");}
ISR( _VECTOR(28)) {print_debug("28 ISR raised. \n");}
ISR( _VECTOR(29)) {print_debug("29 ISR raised. \n");}
ISR( _VECTOR(30)) {print_debug("30 ISR raised. \n");}
ISR( _VECTOR(31)) {print_debug("31 ISR raised. \n");}
ISR( _VECTOR(32)) {print_debug("32 ISR raised. \n");}
ISR( _VECTOR(33)) {print_debug("33 ISR raised. \n");}
ISR( _VECTOR(34)) {print_debug("34 ISR raised. \n");}
ISR( _VECTOR(35)) {print_debug("35 ISR raised. \n");}
ISR( _VECTOR(36)) {print_debug("36 ISR raised. \n");}
ISR( _VECTOR(37)) {print_debug("37 ISR raised. \n");}
ISR( _VECTOR(38)) {print_debug("38 ISR raised. \n");}
ISR( _VECTOR(39)) {print_debug("39 ISR raised. \n");}
ISR( _VECTOR(40)) {print_debug("40 ISR raised. \n");}
ISR( _VECTOR(41)) {print_debug("41 ISR raised. \n");}
ISR( _VECTOR(42)) {print_debug("42 ISR raised. \n");}
ISR( _VECTOR(43)) {print_debug("43 ISR raised. \n");}
ISR( _VECTOR(44)) {print_debug("44 ISR raised. \n");}
ISR( _VECTOR(45)) {print_debug("45 ISR raised. \n");}
ISR( _VECTOR(46)) {print_debug("46 ISR raised. \n");}
ISR( _VECTOR(47)) {print_debug("47 ISR raised. \n");}
ISR( _VECTOR(48)) {print_debug("48 ISR raised. \n");}
ISR( _VECTOR(49)) {print_debug("49 ISR raised. \n");}
ISR( _VECTOR(50)) {print_debug("50 ISR raised. \n");}
ISR( _VECTOR(51)) {print_debug("51 ISR raised. \n");}
ISR( _VECTOR(52)) {print_debug("52 ISR raised. \n");}
ISR( _VECTOR(53)) {print_debug("53 ISR raised. \n");}
ISR( _VECTOR(54)) {print_debug("54 ISR raised. \n");}
ISR( _VECTOR(55)) {print_debug("55 ISR raised. \n");}
ISR( _VECTOR(56)) {print_debug("56 ISR raised. \n");}
ISR( _VECTOR(57)) {print_debug("57 ISR raised. \n");}
ISR( _VECTOR(58)) {print_debug("58 ISR raised. \n");}
//ISR( _VECTOR(59)) {print_debug("59 ISR raised. \n");}
//ISR( _VECTOR(60)) {print_debug("60 ISR raised. \n");}
ISR( _VECTOR(61)) {print_debug("61 ISR raised. \n");}
ISR( _VECTOR(62)) {print_debug("62 ISR raised. \n");}
//ISR( _VECTOR(63)) {print_debug("63 ISR raised. \n");}
ISR( _VECTOR(64)) {print_debug("64 ISR raised. \n");}
//ISR( _VECTOR(65)) {print_debug("65 ISR raised. \n");}
//ISR( _VECTOR(66)) {print_debug("66 ISR raised. \n");}
//ISR( _VECTOR(67)) {print_debug("67 ISR raised. \n");}
//ISR( _VECTOR(68)) {print_debug("68 ISR raised. \n");}
ISR( _VECTOR(69)) {print_debug("69 ISR raised. \n");}
ISR( _VECTOR(70)) {print_debug("70 ISR raised. \n");}
ISR( _VECTOR(71)) {print_debug("71 ISR raised. \n");}
ISR( _VECTOR(72)) {print_debug("72 ISR raised. \n");}
ISR( _VECTOR(73)) {print_debug("73 ISR raised. \n");}
ISR( _VECTOR(74)) {print_debug("74 ISR raised. \n");}
ISR( _VECTOR(75)) {print_debug("75 ISR raised. \n");}
ISR( _VECTOR(76)) {print_debug("76 ISR raised. \n");}
ISR( _VECTOR(77)) {print_debug("77 ISR raised. \n");}
ISR( _VECTOR(78)) {print_debug("78 ISR raised. \n");}
ISR( _VECTOR(79)) {print_debug("79 ISR raised. \n");}

// error
ISR(BADISR_vect) {
	//debugpins_isr_set();
	static const char msg[] = "BADISR\n";
	char c = 0;
	while(1) {
		for (c = 0; c < sizeof(msg); c++) {
			uart_writeByte(msg[c]);
		}
	}
	//debugpins_isr_clr();
}
