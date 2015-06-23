// read a DS1990 identity button skeleton code
// use pin GP0_4 ('SCL')
//

#include "LPC11xx.h"
#include "timer.h"
#include "sw-uart.h"
#include "pin_functions.h"

static int PIN;
static int PORT;


void ds_pin_init(unsigned int port, unsigned int pin){
	PIN = pin;
	PORT = port;
	//kip
	//
}

void ds_pin_set( unsigned char x ){
   pin_configure_as_output(PORT, PIN);
   if( x ){
	 //  pin_set(PORT, PIN, 0);
	   //do nothing, let the resistor do his work.  
	  pin_set(PORT, PIN, 1);
	
	//pin_configure_as_input(PORT, PIN);
    // pin_configure_as_input(PORT, PIN);
   } else {
	    

	   pin_set(PORT, PIN, 0);
		//pin_configure_as_input(PORT, PIN);
   }      
}

unsigned char ds_pin_get(){
	pin_configure_as_input(PORT, PIN);
    return pin_get( PORT, PIN );
}

void ds_pin_pulse( unsigned long long int t ){
   ds_pin_set( 0 );
   delay( t );
   ds_pin_set( 1 );
}

unsigned char ds_present(){

	//ds_pin_get();
   // give the device time to get back to the 'rest' state
   ds_pin_set( 1 );
   delay( 2000 );

   // test for stuck-at-0
   if( ! ds_pin_get() ){ 
   uart_put_string("\ntest stuck at 0");
	return 0; 
   }
   
   // request a presence pulse
   ds_pin_pulse( 1000 );   
   
   // test for the presence pulse 
   delay( 30 );
   if( ds_pin_get() ){ 
	uart_put_string("\npresent pulse");
	return 0; 
   }
   
   // test whether the presence pulse ends
   delay( 1000 );
   if( ! ds_pin_get() ){
	   uart_put_string("\npresence pulse doesn't end.");
	   return 0; 
		
	}
   
   // all tests succeeded
   return 1;   
}

void ds_send_bit( unsigned char b ){
	if( b ) {
		ds_pin_pulse( 6 );
	} else {
		ds_pin_pulse( 90 );
	}
   
   delay( 10 );
}

void ds_send_byte( unsigned char b ){
             
   int i;
 
   for( i = 0; i < 8; i++ ){
      // one data bit
      if( ( b & 0x01 ) == 0 ){ 
          ds_send_bit(0);
      } else { 
       
          ds_send_bit(1);
      }   
      b = b >> 1;  
   }
  
     
}

unsigned char ds_receive_bit( void ){
   unsigned char x;
   ds_pin_set(0);
    
    
   x = ds_pin_get();
   delay( 100 );
   return x;
}

unsigned char ds_receive_byte(){
   int i;
   unsigned char d = 0;
   for( i = 0; i < 8; i++ ){
      d = d >> 1;
      if( ds_receive_bit() ){
         d = d | 0x80;   
      } else {
         d = d & ~ 0x80;
      }
   }
   return d;
}

void ds1990_id_print(){
   int i, d;
   ds_send_byte( 0x33 );
   for( i = 0; i < 8; i++ ){
      d = ds_receive_byte();
      uart_put_int_hexadecimal( d, 2 );
      uart_put_string( " " );
   }
   uart_put_string( "\n" );
}
		
