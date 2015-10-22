/**
\brief Zigduino definition of the "leds" bsp module.

\author Sven Akkermans <sven.akkermans@cs.kuleuven.be>, September 2015.
 */

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/iom128rfa1.h>

#include "watchdog.h"

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

//From contiki
#define PRINTF(FORMAT,args...) printf_P(PSTR(FORMAT),##args)

#define ANNOUNCE_BOOT 1    //adds about 600 bytes to program size
#if ANNOUNCE_BOOT
#define PRINTA(FORMAT,args...) printf_P(PSTR(FORMAT),##args)
#else
#define PRINTA(...)
#endif

#define DEBUG 1
#if DEBUG
#define PRINTD(FORMAT,args...) printf_P(PSTR(FORMAT),##args)
#else
#define PRINTD(...)
#endif

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
	  watchdog_init();
	  watchdog_start(); // Sven: Watchdog is disabled.

	  // Print reboot reason
	  if(mcusr_backup & (1<<PORF )) PRINTD("Power-on reset.\n");
	  if(mcusr_backup & (1<<EXTRF)) PRINTD("External reset!\n");
	  if(mcusr_backup & (1<<BORF )) PRINTD("Brownout reset!\n");
	  if(mcusr_backup & (1<<WDRF )) PRINTD("Watchdog reset!\n");
	  if(mcusr_backup & (1<<JTRF )) PRINTD("JTAG reset!\n");

	PRINTA("\n*******Booting Zigduino with OpenWSN*******\n");

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
	TRXPR = 1 << SLPTR; // sent transceiver to sleep
	set_sleep_mode(SLEEP_MODE_PWR_SAVE); // Power save mode to allow Timer/counter2 interrupts, see pg 162
	sleep_mode();
}

void board_reset() {
	watchdog_reboot(); //rebooting the wd, resets the board
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
	uart_tx_isr(); // doing nothing w/ return value
}

ISR(USART0_TX_vect) {
	uart_rx_isr();
}
// radio interrupt(s)
// pass to radio_isr
ISR(TRX24_RX_START_vect) {
	//radio_rx_start_isr(); // doing nothing w/ return value
}

ISR(TRX24_RX_END_vect) {
	//radio_trx_end_isr();
}
ISR(TRX24_TX_END_vect) {
	//radio_trx_end_isr();
}

// MAC symbol counter interrupt compare 1
// pass to bsp_timer_isr
//SVEN CHANGED
ISR(SCNT_CMP1_vect) {
	bsp_timer_isr();

	//radiotimer_compare_isr();

}

//MAC symbol counter interrupt compare 2/3
// pass to radiotimer_isr //SVEN Changed
ISR(TIMER2_COMPA_vect) {
	bsp_timer_isr();
}

ISR(SCNT_CMP3_vect) {
	//radiotimer_overflow_isr();
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
