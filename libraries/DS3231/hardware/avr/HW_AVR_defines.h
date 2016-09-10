// *** Hardwarespecific defines ***
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))

#ifndef TWI_FREQ
	#define TWI_FREQ 400000L
#endif
