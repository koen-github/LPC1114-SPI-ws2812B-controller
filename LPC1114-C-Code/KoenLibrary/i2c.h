//! @file
//! This library implements bit-banged low-level I2C routines
//! using the standard SCL and SDA pins

#include "LPC11xx.h"

//! initialize the I2C bus
void i2c_init( void );

//! output an I2C start 
void i2c_start( void );

//! output an I2C stop
void i2c_stop();

//! get and return the ack condition
unsigned char i2c_ack_get( void );

//! output an I2C ack
void i2c_put_ack( void );

//! output an I2C nack
void i2c_put_nack( void );

//! output a byte on the i2c bus
void i2c_put_byte( unsigned char x );

//! Send data byte at register address, Please check control byte before using this
int i2c_put_byte_at_register( int reg[8], int data[8] );

//! Receive data byte at register address, 8-bit register. Please check control byte before using this
unsigned char i2c_read_byte_from_register(int reg[8]);


//! get a byte from the i2c bus and return it
unsigned char i2c_get_byte( void );

