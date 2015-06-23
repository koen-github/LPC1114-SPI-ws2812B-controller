
//! @file
//! A lot of used pin functions so you can easily configure pins as input or output.


//! Configure pin as input
//
//! This function sets the given pin as input pin. 
void pin_configure_as_input( int port, int pin );


//! Configure pin as output
//
//! This function sets the given pin as output pin. 
void pin_configure_as_output( int port, int pin );

//! Set pin low or high
//
//! This function sets the given pin to low or high
void pin_set( int port, int pin, unsigned char x );

//! Return the pin status
//
//! This function returns low or high with the given pin number
unsigned char pin_get( int port, int pin );