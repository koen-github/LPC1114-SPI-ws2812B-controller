// header file for the timer functions
//! @file
//! Initialize the timer
//
//! This function can activate the onboard timer in your mircrocontroller.
void timer_init( void );

//! Return the current time
//
//! This can return the current time until the timer was started.
unsigned int now();

//! Wait until in milliseconds.
//
//! This can let the script wait until a specific time is passed.
void await( unsigned int t );


//! Wait until in milliseconds.
//
//! This can let the script wait until a specific time is passed, from the time it started.
void delay(unsigned int x); 