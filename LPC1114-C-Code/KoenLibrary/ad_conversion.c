#include "pin_functions.h"
#include "timer.h"
#include "LPC11xx.h"

static int PIN;
static int PORT;


/*
LPC_SYSCON->SYSAHBCLKCTRL
LPC_IOCON->R_PIO1_0
LPC_SYSCON->PDRUNCFG 
LPC_ADC->CR
LPC_ADC->GDR
*/

void init_ad_conversion(int port, int pin){
	PORT=port;
	PIN=pin;
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 16); //configure  

	LPC_IOCON->R_PIO1_0 = 0x02;


	//zet bit 13 aan
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 13);
  
  
	LPC_SYSCON->PDRUNCFG &= (0 << 4); //reset value is 1, om hem aan te zetten moet hij op 0 komen. 1 && 0 == 0 
	LPC_ADC->CR = (1 << 1); //tweede bit op 1, want 0000010 is 0x2, hij begint vanaf 0 te tellen de bitshifter
	
	LPC_ADC->CR |= (4 << 8); //deze mag maximaal 4.5mhz zijn.
	LPC_ADC->CR |= (0 << 16); //BURST mode voor software conversie: reset value is 0, 0 || 0 == 0
	
	
}

void perform_ad_conversion(){
	LPC_ADC->CR |= (1 << 24); //zet bit 24 aan, deze staat standaard op 0 dus 1 || 0 == 1
}

unsigned int get_ad_value(){
//	if(((LPC_ADC->GDR[7]) & 0x8000000)){ //check if bit 31 is DONE -> set to 1
		return ((LPC_ADC->GDR >> 6) & 0x3ff); //what? wordt geAND met 1023
//	}
}