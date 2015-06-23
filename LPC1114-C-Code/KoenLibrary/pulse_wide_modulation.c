#include "pin_functions.h"
#include "timer.h"
#include "LPC11xx.h"

//only for GPIO1_1


/*
LPC_SYSCON->SYSAHBCLKCTRL v
LPC_IOCON->R_PIO1_1 v

LPC_TMR32B1->CTCR v
LPC_TMR32B1->PR  v
LPC_TMR32B1->TCR v

LPC_TMR32B1->PWMC v
LPC_TMR32B1->MR3 v
LPC_TMR32B1->EMR 
LPC_TMR32B1->MCR v
LPC_TMR32B1->MR0 v

*/



void init_pwm(unsigned int c){ //give 100
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 10); //enable timer1 

	LPC_IOCON->R_PIO1_1 = 0x03; //set pin config
	/*TIMER*/
	
	LPC_TMR32B1->CTCR = 0x00;                // use AHB clock
	LPC_TMR32B1->PR    = 11;                  // 12 MHz -> 1 count/usec
	LPC_TMR32B1->TCR  = 0x01;                // enable the timer

	LPC_TMR32B1->PWMC |= (1 << 0);
	LPC_TMR32B1->MCR = (1 << 10);
	
	LPC_TMR32B1->MR3 = c; //how much
	LPC_TMR32B1->MR0 = 50; //use as start cycle value/
	
	
	LPC_TMR32B1->EMR |= (1 << 4);
	
	
}

void set_output_duty_cycle(unsigned int c){
	LPC_TMR32B1->MR0 = c; //cycle how long low.
}

