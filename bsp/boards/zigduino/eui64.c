/**
\brief Zigduino definition of the "eui64" bsp module.

\author Sven Akkermans <sven.akkermans@cs.kuleuven.be>, September 2015.
*/

#include "string.h"
#include "eui64.h"

#include <avr/pgmspace.h>
#include <avr/fuse.h>
#include <avr/eeprom.h>

//=========================== defines =========================================

//=========================== variables =======================================

//=========================== prototypes ======================================
uint8_t
rng_get_uint8(void) {
	  uint8_t i,j;
	  ADCSRA=1<<ADEN;             //Enable ADC, not free running, interrupt disabled, fastest clock
	  for (i=0;i<4;i++) {
	    ADMUX = 0;                //toggle reference to increase noise
	    ADMUX =0x1E;              //Select AREF as reference, measure 1.1 volt bandgap reference.
	    ADCSRA|=1<<ADSC;          //Start conversion
	    while (ADCSRA&(1<<ADSC)); //Wait till done
		j = (j<<2) + ADC;
	  }
	  ADCSRA=0;                   //Disable ADC
	   printf("rng issues %u\n",j);
	  return j;}
//=========================== public ==========================================

void eui64_get(uint8_t* addressToWrite) {
	uint8_t rng_id1 = rng_get_uint8();
	uint8_t rng_id2 = rng_get_uint8();

    addressToWrite[0] = 0x14;   // OpenWSN OUI
    addressToWrite[1] = 0x15;
    addressToWrite[2] = 0x92;
    addressToWrite[3] = 0xe3;
    addressToWrite[4] = 0xff; // ffff is zigduino
    addressToWrite[5] = 0xff;
    addressToWrite[6] = rng_id1;
    addressToWrite[7] = rng_id2;

}
//=========================== private =========================================

