#include "LPC11xx.h"
#include "sw-uart.h"
#include "timer.h"

//Examples for led strips:
//to send a 1, the line must be high for 800ns, and low for 450ns
//to send a 0, the line must be high for 450ns and low for 800ns

//SPI LED_PATTERN
#define led_one 15 //1111, including default low time, good 800ns high, and enough low
#define led_zero 12 //1100, 450ns high, 800 low (more in this case)

//Number of leds
#define NUMBER_OF_LEDS 59

/* SSP Busy Bit */
#define SSPSR_BSY       (0x1<<4)

//master mode
#define SSPCR1_SSE      (0x1<<1)

void config_SSP()
{
	//disable interrupts
	NVIC_DisableIRQ(SSP0_IRQn); //disable interrupts, so possible to send constant stream of SPI bytes
    //enable clocks for the SSP
    LPC_SYSCON->PRESETCTRL |= (0x1<<0);	//reset de-asserted
    LPC_SYSCON->SYSAHBCLKCTRL |= (1UL << 11);
	LPC_SYSCON->SYSAHBCLKCTRL |= (1UL << 16);
    LPC_SYSCON->SSP0CLKDIV = 0x01; //48 / 1 == 48
	
    //IO/config SPI
    LPC_IOCON->PIO0_8 &= ~0x07; //SSP I/O config
    LPC_IOCON->PIO0_8 |= 0x01; //SSP MISO
	
	//MOSI, and PULL DOWN (so mosi idle state is low)
    LPC_IOCON->PIO0_9 &= ~0x07;
	LPC_IOCON->PIO0_9 |= 0x01 | (0x01 << 3);
	
	LPC_IOCON->SCK_LOC = 0x02;
    LPC_IOCON->PIO0_6 = 0x02;
    LPC_IOCON->PIO0_2 &= ~0x07;
    LPC_IOCON->PIO0_2 |= 0x01;

      //Set DSS data to 4-bit, Frame format SPI, CPOL = 0, CPHA = 0, and SCR is 7
    LPC_SSP0->CR0 = 0x0307;

      //SSPCPSR clock prescale register, master mode, minimum divisor is 0x02
    LPC_SSP0->CPSR = 2;

      //Master mode
    LPC_SSP0->CR1 = SSPCR1_SSE;
}


void convert_char_to_spi(unsigned char theColor){
	int i = 0;
	for( i = 0; i < 8; i++ ){
		if(( theColor & 0x80 ) != 0 ){ //if it' s a binair one, write a 15 to SPI
			LPC_SSP0->DR = led_one;
			while ( LPC_SSP0->SR & SSPSR_BSY ); //wait until bit is out
		} else{ //binair zero, send 12
			LPC_SSP0->DR = led_zero;
			while ( LPC_SSP0->SR & SSPSR_BSY );
		}
		theColor = theColor << 1;
	}
}

void convert_full_ledstrip(unsigned char *theColors){
	int x;
	for(x = 0; x < (NUMBER_OF_LEDS*3); x++){
		convert_char_to_spi(theColors[x]); //send every color to byte SPI stream
	}
	delay(1);
}

void receive_GRB_value(){
	unsigned char IO_char = 'A'; //default value, some random letter
	int numberOfLED = 0;
	unsigned char theColors[(NUMBER_OF_LEDS*3)];
	for(;;){
		if(numberOfLED < (NUMBER_OF_LEDS*3)){ //receive 177 bytes, for 59 leds one byte per color
			receive_char(&IO_char);
			theColors[numberOfLED] = IO_char;
			numberOfLED++;

		}
		else{
			break;
		}
	} 
	
	//uart_put_int_decimal(theColors[0][0]);
	numberOfLED = 0;
	convert_full_ledstrip(theColors);
	delay(50); //led reset time
	//uart_put_string("\nSUCCESS");
}


int main()
{
	uart_init();
	timer_init();
    config_SSP();
	unsigned char theColors[(NUMBER_OF_LEDS*3)] = {0};
	convert_full_ledstrip(theColors);
	uart_put_string("\nLPC1114");

	while(1){
		receive_GRB_value();
	}
 
	
	return 0;
	
}

