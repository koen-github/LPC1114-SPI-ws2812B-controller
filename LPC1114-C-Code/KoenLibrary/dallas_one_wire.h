
//! @file
//! Configure dallas one wire pin
//
//! This function sets the using pin number for the dallas one wire.
void ds_pin_init(unsigned int port, unsigned int pin);

//! Make dallas pin high or low
//
//! This function set the dallas pin high or low.
void ds_pin_set( unsigned char x );

//! Return value of dallas pin
//
//! This function can return the value of the dallas pin.
unsigned char ds_pin_get();

//! Give pulse of low and high
//
//! This function can give a pulse to the output pin, using a specific time.
void ds_pin_pulse( unsigned long long int t );

//! Check to see if dallas is present
//
//! This function can check if the dallas one wire is present. 
unsigned char ds_present();

//! Dallas send bit
//
//! This function sends a specific bit to the dallas one wire.
void ds_send_bit( unsigned char b );

//! Dallas send byte
//
//! This function sends a byte to the dallas one wire. 
void ds_send_byte( unsigned char b );

//! Receive dallas one bit
//
//! This function receives a bit of the dallas one wire.
unsigned char ds_receive_bit( void );

//! Receive byte of dallas one wire.
//
//! This function can receive a byte from the dallas one wire.
unsigned char ds_receive_byte();


//! Print id of one wire
//
//! This function can receive the serial number of the dallas one.
void ds1990_id_print();