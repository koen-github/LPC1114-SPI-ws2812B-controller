// implementation of the uart functions
// Comments added by Koen

#include "LPC11xx.h"
#include "sw-uart.h"
#include "timer.h"

#define BIT_TIME ( (1000 * 1000) / (BMPTK_BAUDRATE) )


//! Set TXD line low
//
//! Call this function to set the txd line to low.
void txd_low( void ){
   LPC_GPIO1->DATA = LPC_GPIO1->DATA & ~ ( 1 << 7 );
}


//! Set TXD line high
//
//! Call this function to set the txd line to high.
void txd_high( void ){
   LPC_GPIO1->DATA = LPC_GPIO1->DATA | ( 1 << 7 );
}

//! initialize the UART 
//
//! Call this function to initialize the UART pins.
void uart_init( void ){
   LPC_GPIO1->DIR = LPC_GPIO1->DIR | ( 1 << 7 ); // make GP1_7 output
   txd_high();
}

//! write a single character
//
//! This function writes a single character using the standard UART
//! output pin of the chip (GP1_7) and the baudrate BMPTK_BAUDRATE,
//! which is passed as a command line argument during compilation.
void uart_put_char( char c ){
   unsigned int t = now();
   int i;
   
   // start bit
   txd_low();
   t += BIT_TIME;
   await( t );
   
   for( i = 0; i < 8; i++ ){
      // one data bit
      if( ( c & 0x01 ) == 0 ){ 
          txd_low(); 
      } else { 
          txd_high(); 
      }   
      c = c >> 1;
      t += BIT_TIME;
      await( t );       
   }
   
   // stop bits
   txd_high();
   t += 2 * BIT_TIME;
   await( t );   
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



		
