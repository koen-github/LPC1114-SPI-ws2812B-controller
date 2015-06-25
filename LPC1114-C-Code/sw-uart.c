// implementation of the uart functions
// Comments added by Koen

#include "LPC11xx.h"
#include "sw-uart.h"
#include "timer.h"

//USED SOURCE: WOUTER @Voti, HWCPP
   
// UART line status register (LSR) bit definitions 
static const unsigned int LSR_RDR   = 0x01;
static const unsigned int LSR_THRE  = 0x20;
static const unsigned int LSR_TEMT  = 0x40;

static const int MHz  = 1000 * 1000;
static const unsigned int clock_frequency = 48 * (1000 * 1000); //fast clock, for fast processing so we can use the SPI clock as input for the WS2812B

//! set the baudrate to baud
static void baudrate_set( unsigned int baud ){
	uint32_t Fdiv;
	LPC_SYSCON->UARTCLKDIV     = 0x1;     // divided by 1 
	Fdiv = clock_frequency / ( LPC_SYSCON->SYSAHBCLKDIV * 16 * baud );
	LPC_UART->DLM = Fdiv / 256;
	LPC_UART->DLL = Fdiv % 256;    
}

//! Specify clock frequentie for LPC1114
//! Not allowed to call outside this file
static void initialize_clock(){
	if( clock_frequency == 12 * MHz ){
		// nothing required, this is the default
	} else {
		// static bool initialize_clock_done = false;
		//if( initialize_clock_done ) return;
		//initialize_clock_done = true;

		if( clock_frequency == 48 * MHz ){

			// as is, this can be done only once, 
			// otherwise the CPU will lock up

			LPC_SYSCON->SYSAHBCLKDIV             = 0x1;       //set clock divider for core to 1
			LPC_SYSCON->MAINCLKSEL               &= ~(0x03);  //set “main clock” to IRC oscillator, if not system will lock up when PLL turns off!(sec. 3.5.11)
			LPC_SYSCON->MAINCLKUEN               &= ~(1);     //write a zero to the MAINCLKUEN register (sec. 3.5.12), necessary for MAINCLKSEL to update
			LPC_SYSCON->MAINCLKUEN               |= 1;        //write a one to the MAINCLKUEN register (sec. 3.5.12), necessary for MAINCLKSEL to update
			LPC_SYSCON->PDRUNCFG                 |= (1<<7);   //power down the PLL before changing divider values
			LPC_SYSCON->SYSPLLCTRL               = 0x23;      //set MSEL = 0x00011 and PSEL = 0x01 (table 46 of sec. 3.11.4.1)
			LPC_SYSCON->PDRUNCFG                 &= ~(1<<7);  //power up PLL after divider values changed as per sec. 3.11.4
			while((LPC_SYSCON->SYSPLLSTAT & 1) == 0);         //wait for PLL to lock
			LPC_SYSCON->MAINCLKSEL               = 0x03;      //set system oscillator to the output of the PLL (sec. 3.5.11)
			LPC_SYSCON->MAINCLKUEN               &= ~(1);     //write a zero to the MAINCLKUEN register (sec. 3.5.12), necessary for MAINCLKSEL to update
			LPC_SYSCON->MAINCLKUEN               |= 1;        //write a one to the MAINCLKUEN register (sec. 3.5.12)
		}
	}
}


//! initialize the UART 
//
//! Call this function to initialize the UART pins.
void uart_init( void ){    
	initialize_clock();    
	// Not using interrupts
	NVIC_DisableIRQ(UART_IRQn);

	// enable IO config
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<16);     //enable IOCON  
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<12);     //enable UART 

	// UART I/O config
	LPC_IOCON->PIO1_6 &= ~0x07;
	LPC_IOCON->PIO1_6 |= 0x01;     // UART RXD
	LPC_IOCON->PIO1_7 &= ~0x07;
	LPC_IOCON->PIO1_7 |= 0x01;     // UART TXD 

	// Enable UART clock 
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<12);

	LPC_UART->LCR = 0x83;             // 8 bits, no Parity, 1 Stop bit 
	baudrate_set( BMPTK_BAUDRATE ); //baudrate specified by BMPTK
	LPC_UART->LCR = 0x03;   // DLAB = 0 
	LPC_UART->FCR = 0x07;   // Enable and reset TX and RX FIFO. 

	// Read to clear the line status. 
	(void)LPC_UART->LSR;
	
	// Ensure a clean start, no data in either TX or RX FIFO. 
	while ( (LPC_UART->LSR & (LSR_THRE|LSR_TEMT)) != (LSR_THRE|LSR_TEMT) );
	while ( LPC_UART->LSR & LSR_RDR )
	{
		(void)LPC_UART->RBR; // Dump data from RX FIFO 
	} 
}

//! receives a single character
//
//! This function can receive a single char from via the RX, specify a pointer to a char in your local system
void receive_char(unsigned char *out_c){
    while (!(LPC_UART->LSR & 0x01));    //RDR: Receiver Data Ready
                                        //0: RBR is empty, 1: U0RBR contains valid data
    *out_c = LPC_UART->RBR;
}

//! write a single character
//
//! This function writes a single character using the standard UART
//! output pin of the chip (GP1_7) and the baudrate BMPTK_BAUDRATE,
//! which is passed as a command line argument during compilation.
void uart_put_char( char c ){
	 while(( LPC_UART->LSR & ( 1 << 6 )) == 0 );
	 LPC_UART->THR = c;
}

//! write a character string
//
//! This function writes the characters of a standard 0-terminated string
//! using uart_put_char().
void uart_put_string( char *s ){
   while( *s != '\0' ){
      uart_put_char( *s );
      s++;
   }      
}


//! write negative int
//
//! This function writes a negative int number to UART
//! using uart_put_char().
void uart_put_int_negative( int x ){
   int xx = x / 10;
   if( xx != 0 ){
      uart_put_int_negative( xx );
   }
   uart_put_char( '0' - ( x % 10 ));
}


//! write decimal number
//
//! This function writes a decimal number to UART
//! using uart_put_char().
void uart_put_int_decimal( int x ){
   if( x < 0 ){
      uart_put_char( '-' );
   } else {   
      x = - x;      
   } 
   uart_put_int_negative( x );
}   

//! write hexadecimal number
//
//! This function writes a hexadecimal digit to the UART
//! using uart_put_char().
void uart_put_int_hexadecimal_digit( int x ){
   x = x & 0x0F;
   if( x < 10 ){
      uart_put_char( x + '0' );
   } else {
      uart_put_char( ( x + 'A' ) - 10 );
   }      
}

//! writes int number as hexadecimal
//
//! This function writes int number as hexadecimal to UART
//! using uart_put_char().
void uart_put_int_hexadecimal( unsigned int x, int n ){
   while( n > 0 ){
      uart_put_int_hexadecimal_digit( x >> (( n - 1 ) * 4 ) );
      n--;
   }
}



		
