//! @file
//! This library implements bit-banged low-level I2C routines
//! using the standard SCL and SDA pins

#include "LPC11xx.h"
#include "i2c.h"
#include "timer.h"

void i2c_init( void ){
	timer_init();
   // no special initialization needed
}

void i2c_pause( void ){
   await( now() + 5 );
}

void i2c_scl_set( unsigned char x ){
   if( x ){
      LPC_GPIO0->DATA |= 0x10;   
   } else {
      LPC_GPIO0->DIR |= 0x10; 
      LPC_GPIO0->DATA &= ~ 0x10;   
   }    
}

void i2c_sda_set( unsigned char x ){
   if( x ){
      LPC_GPIO0->DIR &= ~ 0x20; 
   } else {
      LPC_GPIO0->DIR |= 0x20; 
      LPC_GPIO0->DATA &= ~ 0x20;
   }    
}

unsigned char i2c_sda_get( void ){
   return ( LPC_GPIO0->DATA & 0x20 ) != 0;
}

void i2c_start( void ){
   i2c_sda_set( 0 );
   i2c_pause();
   i2c_scl_set( 0 );
   i2c_pause();
}

void i2c_stop(){
   i2c_scl_set( 0 );
   i2c_pause();
   i2c_sda_set( 0 );
   i2c_pause();
   i2c_scl_set( 1 );
   i2c_pause();
   i2c_sda_set( 1 );
   i2c_pause();
}

void i2c_put_bit( unsigned char x ){
   i2c_scl_set( 0 );
   i2c_pause();
   i2c_sda_set( x );
   i2c_scl_set( 1 );
   i2c_pause();
}

unsigned char i2c_get_bit( void ){
   unsigned char x;
   i2c_scl_set( 0 );
   i2c_pause();
   i2c_sda_set( 1 );
   i2c_scl_set( 1 );
   i2c_pause();
   x = i2c_sda_get();
   i2c_pause();
   return x;
}

unsigned char i2c_ack_get( void ){
   unsigned char x;
   x = ! i2c_get_bit();
   return x;
}

void i2c_put_ack( void ){
   i2c_put_bit( 0 );
}

void i2c_put_nack( void ){
   i2c_put_bit( 1 );
}

void i2c_put_byte( unsigned char x ){
   int i;
   for( i = 0; i < 8; i++ ){
      i2c_put_bit( ( x & 0x80 ) != 0 );
      x = x << 1;
   }
}

int i2c_put_array_at_register(int data[8]){
	int i;
	for( i = 0; i < 8; i++ ){
      i2c_put_bit( data[i] );
	}
	i2c_pause();//wait for ack bit
	return i2c_ack_get();
}


int i2c_put_byte_at_register( int reg[8], int data[8] ){
	i2c_start();  
	int control_byte[8] = {0,1,0,0,1,1,1,0}; //control byte, write operation, CHANGE TO SOMETHING ELSE TO USE ON ANOTHER CHIP
	int a = i2c_put_array_at_register(control_byte);
	int b = i2c_put_array_at_register(reg);
	int c = i2c_put_array_at_register(data);

	i2c_stop(); 
	if(a && b && c){
		return 1; //everything succes;
	} else{
		return 0; //something went wrong.
	}
}

unsigned char i2c_read_byte_from_register(int reg[8]){
	i2c_start();  
	int control_byte_write[8] = {0,1,0,0,1,1,1,0};
	i2c_put_array_at_register(control_byte_write);
	i2c_put_array_at_register(reg);
	i2c_stop(); 
	
	//reading
	i2c_start();
	int control_byte_read[8] = {0,1,0,0,1,1,1,1}; //control byte, read operation
	i2c_put_array_at_register(control_byte_read);
	char data_from_reg = i2c_get_byte();
	i2c_put_ack();
	i2c_stop(); 
	//if(a && b){
		return data_from_reg; //everything succes, return data;
	//} else{
//		return 0; //something went wrong.
	//}
}



unsigned char i2c_get_byte( void ){
   int i;
   unsigned char result = 0;
   for( i = 0; i < 8; i++ ){
      result = result << 1;
      if( i2c_get_bit() ){
         result |= 0x01;
      }
      
   }
   return result;
}

