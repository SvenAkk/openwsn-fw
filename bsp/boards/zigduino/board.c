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
	//	TRXPR = (0 << SLPTR); // sent transceiver to sleep
	set_sleep_mode(SLEEP_MODE_PWR_SAVE); // Power save mode to allow Timer/counter2 interrupts, see pg 162
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
	//	printf("USART0_RX_vect ISR raised. \n");
	uart_rx_isr(); // doing nothing w/ return value
}

ISR(USART0_TX_vect) {
	//	printf("USART0_TX_vect ISR raised. \n");
	uart_tx_isr(); // doing nothing w/ return value
}
// radio interrupt(s)
// pass to radio_isr
ISR(TRX24_RX_START_vect) {
//	printf("TRX24_RX_START_vect ISR raised. \n");
	radio_rx_start_isr(); // doing nothing w/ return value
}

ISR(TRX24_RX_END_vect) {
//	printf("TRX24_RX_END_vect ISR raised. \n");
	radio_trx_end_isr();
}
ISR(TRX24_TX_END_vect) {
//	printf("TRX24_TX_END_vect ISR raised. \n");
	radio_trx_end_isr();
}

ISR(SCNT_CMP1_vect) {
	//	printf("SCNT_CMP1_vect ISR raised. \n");
	bsp_timer_isr();
}

ISR(SCNT_CMP2_vect) {
	//	printf("SCNT_CMP2_vect ISR raised. \n");
	radiotimer_compare_isr();
}

ISR(SCNT_CMP3_vect) {
	//	printf("SCNT_CMP3_vect ISR raised. \n");
	radiotimer_overflow_isr();
}

ISR(SCNT_OVFL_vect) {
	printf("SCNT_OVFL_vect ISR raised. Should be unused. \n");
	//radiotimer_overflow_isr();
}

/* Hang on any unsupported interrupt */
/* Useful for diagnosing unknown interrupts that reset the mcu.
 * Currently set up for 12mega128rfa1.
 * For other mcus, enable all and then disable the conflicts.
 */
ISR( _VECTOR(0)) {printf("0 ISR raised. \n");}
ISR( _VECTOR(1)) {printf("1 ISR raised. \n");}
ISR( _VECTOR(2)) {printf("2 ISR raised. \n");}
ISR( _VECTOR(3)) {printf("3 ISR raised. \n");}
ISR( _VECTOR(4)) {printf("4 ISR raised. \n");}
ISR( _VECTOR(5)) {printf("5 ISR raised. \n");}
ISR( _VECTOR(6)) {printf("6 ISR raised. \n");}
ISR( _VECTOR(7)) {printf("7 ISR raised. \n");}
ISR( _VECTOR(8)) {printf("8 ISR raised. \n");}
ISR( _VECTOR(9)) {printf("9 ISR raised. \n");}
ISR( _VECTOR(10)) {printf("10 ISR raised. \n");}
ISR( _VECTOR(11)) {printf("11 ISR raised. \n");}
ISR( _VECTOR(12)) {printf("12 ISR raised. \n");}
ISR( _VECTOR(13)) {printf("13 ISR raised. \n");}
ISR( _VECTOR(14)) {printf("14 ISR raised. \n");}
ISR( _VECTOR(15)) {printf("15 ISR raised. \n");}
ISR( _VECTOR(16)) {printf("16 ISR raised. \n");}
ISR( _VECTOR(17)) {printf("17 ISR raised. \n");}
ISR( _VECTOR(18)) {printf("18 ISR raised. \n");}
ISR( _VECTOR(19)) {printf("19 ISR raised. \n");}
ISR( _VECTOR(20)) {printf("20 ISR raised. \n");}
ISR( _VECTOR(21)) {printf("21 ISR raised. \n");}
ISR( _VECTOR(22)) {printf("22 ISR raised. \n");}
ISR( _VECTOR(23)) {printf("23 ISR raised. \n");}
ISR( _VECTOR(24)) {printf("24 ISR raised. \n");}
//ISR( _VECTOR(25)) {printf("25 ISR raised. \n");}
ISR( _VECTOR(26)) {printf("26 ISR raised. \n");}
//ISR( _VECTOR(27)) {printf("27 ISR raised. \n");}
ISR( _VECTOR(28)) {printf("28 ISR raised. \n");}
ISR( _VECTOR(29)) {printf("29 ISR raised. \n");}
ISR( _VECTOR(30)) {printf("30 ISR raised. \n");}
ISR( _VECTOR(31)) {printf("31 ISR raised. \n");}
ISR( _VECTOR(32)) {printf("32 ISR raised. \n");}
ISR( _VECTOR(33)) {printf("33 ISR raised. \n");}
ISR( _VECTOR(34)) {printf("34 ISR raised. \n");}
ISR( _VECTOR(35)) {printf("35 ISR raised. \n");}
ISR( _VECTOR(36)) {printf("36 ISR raised. \n");}
ISR( _VECTOR(37)) {printf("37 ISR raised. \n");}
ISR( _VECTOR(38)) {printf("38 ISR raised. \n");}
ISR( _VECTOR(39)) {printf("39 ISR raised. \n");}
ISR( _VECTOR(40)) {printf("40 ISR raised. \n");}
ISR( _VECTOR(41)) {printf("41 ISR raised. \n");}
ISR( _VECTOR(42)) {printf("42 ISR raised. \n");}
ISR( _VECTOR(43)) {printf("43 ISR raised. \n");}
ISR( _VECTOR(44)) {printf("44 ISR raised. \n");}
ISR( _VECTOR(45)) {printf("45 ISR raised. \n");}
ISR( _VECTOR(46)) {printf("46 ISR raised. \n");}
ISR( _VECTOR(47)) {printf("47 ISR raised. \n");}
ISR( _VECTOR(48)) {printf("48 ISR raised. \n");}
ISR( _VECTOR(49)) {printf("49 ISR raised. \n");}
ISR( _VECTOR(50)) {printf("50 ISR raised. \n");}
ISR( _VECTOR(51)) {printf("51 ISR raised. \n");}
ISR( _VECTOR(52)) {printf("52 ISR raised. \n");}
ISR( _VECTOR(53)) {printf("53 ISR raised. \n");}
ISR( _VECTOR(54)) {printf("54 ISR raised. \n");}
ISR( _VECTOR(55)) {printf("55 ISR raised. \n");}
ISR( _VECTOR(56)) {printf("56 ISR raised. \n");}
ISR( _VECTOR(57)) {printf("57 ISR raised. \n");}
ISR( _VECTOR(58)) {printf("58 ISR raised. \n");}
//ISR( _VECTOR(59)) {printf("59 ISR raised. \n");}
//ISR( _VECTOR(60)) {printf("60 ISR raised. \n");}
ISR( _VECTOR(61)) {printf("61 ISR raised. \n");}
ISR( _VECTOR(62)) {printf("62 ISR raised. \n");}
//ISR( _VECTOR(63)) {printf("63 ISR raised. \n");}
ISR( _VECTOR(64)) {printf("64 ISR raised. \n");}
//ISR( _VECTOR(65)) {printf("65 ISR raised. \n");}
//ISR( _VECTOR(66)) {printf("66 ISR raised. \n");}
//ISR( _VECTOR(67)) {printf("67 ISR raised. \n");}
//ISR( _VECTOR(68)) {printf("68 ISR raised. \n");}
ISR( _VECTOR(69)) {printf("69 ISR raised. \n");}
ISR( _VECTOR(70)) {printf("70 ISR raised. \n");}
ISR( _VECTOR(71)) {printf("71 ISR raised. \n");}
ISR( _VECTOR(72)) {printf("72 ISR raised. \n");}
ISR( _VECTOR(73)) {printf("73 ISR raised. \n");}
ISR( _VECTOR(74)) {printf("74 ISR raised. \n");}
ISR( _VECTOR(75)) {printf("75 ISR raised. \n");}
ISR( _VECTOR(76)) {printf("76 ISR raised. \n");}
ISR( _VECTOR(77)) {printf("77 ISR raised. \n");}
ISR( _VECTOR(78)) {printf("78 ISR raised. \n");}
ISR( _VECTOR(79)) {printf("79 ISR raised. \n");}

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
