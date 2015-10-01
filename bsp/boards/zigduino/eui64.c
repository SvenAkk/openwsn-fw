/**
\brief Zigduino definition of the "eui64" bsp module.

\author Sven Akkermans <sven.akkermans@cs.kuleuven.be>, September 2015.
*/

#include "string.h"
#include "eui64.h"


//=========================== defines =========================================

//Changed 0x0028002F and others to 0x0028006F
#define BSP_EUI64_ADDRESS_HI_H      ( 0x0028006F )
#define BSP_EUI64_ADDRESS_HI_L      ( 0x0028006C )
#define BSP_EUI64_ADDRESS_LO_H      ( 0x0028006B )
#define BSP_EUI64_ADDRESS_LO_L      ( 0x00280068 )

//=========================== variables =======================================

//=========================== prototypes ======================================

//=========================== public ==========================================

void eui64_get(uint8_t* addressToWrite) {
   uint8_t* eui64_flash;

   eui64_flash = (uint8_t*)BSP_EUI64_ADDRESS_LO_H;
   while(eui64_flash >= (uint8_t*)BSP_EUI64_ADDRESS_LO_L) {
      *addressToWrite++ = *eui64_flash--;
   }

   eui64_flash = (uint8_t*)BSP_EUI64_ADDRESS_HI_H;
   while(eui64_flash >= (uint8_t*)BSP_EUI64_ADDRESS_HI_L) {
      *addressToWrite++ = *eui64_flash--;
   }
}

//=========================== private =========================================

