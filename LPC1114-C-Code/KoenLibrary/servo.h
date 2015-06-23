//! @file
//! Set servo to low
//
//! This function sets the servo to a low voltage.
void servo_low( void );

//! Set servo to high
//
//! This function sets the servo to a high voltage.
void servo_high( void );

//! Initialize the Servo
//
//! This function initializes the servo using a port and pin number
void servo_init( unsigned int port, unsigned int pin );

//! Control the servo 
//
//! This function initializes the servo using a port and pin number and can also use a position between 500 and 2500.
void servo_pulse( unsigned int port, unsigned int pin, unsigned int position );