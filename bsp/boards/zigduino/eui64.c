/**
\brief Zigduino definition of the "eui64" bsp module.

\author Sven Akkermans <sven.akkermans@cs.kuleuven.be>, September 2015.
 */

#include "string.h"
#include "eui64.h"

#include <avr/pgmspace.h>
#include <avr/fuse.h>
#include <avr/eeprom.h>
#include <util/delay.h>

//=========================== defines =========================================

#define EUI64_RANDOM_NB_ADDRESS 0x10ee
//=========================== variables =======================================

//=========================== prototypes ======================================
uint8_t
rng_get_uint8(void) {
	uint8_t temp = TRX_STATE;
	TRX_STATE = (TRX_STATE & 0xE0) | RX_ON; // Ensure radio can generate rnd values.
	_delay_us(200);

	uint8_t j = 0;
	uint8_t i = 0;
	for(; i < 4; i ++){
		_delay_us(2);
		j = j * 4;
		j += ((PHY_RSSI&0x60)>>5);
	}
	printf("rng issues %u\n",j);

	TRX_STATE = temp; //Restore radio to previous state

	return j;
}
//=========================== public ==========================================

void eui64_get(uint8_t* addressToWrite) {

	addressToWrite[0] = 0x14;   // OpenWSN OUI
	addressToWrite[1] = 0x15;
	addressToWrite[2] = 0x92;
	addressToWrite[3] = 0xe3;
	addressToWrite[4] = 0xff; // ffff is zigduino
	addressToWrite[5] = 0xff;

	uint8_t rng_id1;
	uint8_t rng_id2;

	if(eeprom_read_byte(EUI64_RANDOM_NB_ADDRESS) == 0){
		rng_id1 = rng_get_uint8();
		rng_id2 = rng_get_uint8();

		eeprom_write_byte(EUI64_RANDOM_NB_ADDRESS,rng_id1);
		eeprom_write_byte(EUI64_RANDOM_NB_ADDRESS+0x04,rng_id2);

	} else {
		rng_id1 = eeprom_read_byte(EUI64_RANDOM_NB_ADDRESS);
		rng_id2 = eeprom_read_byte(EUI64_RANDOM_NB_ADDRESS+0x04);
	}

	addressToWrite[6] = rng_id1;
	addressToWrite[7] = rng_id2;
}
//=========================== private =========================================
