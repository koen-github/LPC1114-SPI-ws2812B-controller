// slowly rotate a servo connected to pin GP1_4
// 

#include "LPC11xx.h"
#include "timer.h"
#include "pin_functions.h"

static int PIN;
static int PORT;

void servo_low(){
	pin_set(PORT, PIN, 0);
}

void servo_high(){
	pin_set(PORT, PIN, 1);
}

void servo_init( unsigned int port, unsigned int pin ){
	PIN = pin;
	PORT = port;
	pin_configure_as_output(port, pin);
	servo_low();
}

void servo_pulse( unsigned int port, unsigned int pin, unsigned int position ) {
	PIN = pin;
	PORT = port;
	if(position < 500 || position > 2500){
	}
	else{
		servo_high();
		await( now() + position );   
		servo_low();
	}
}

/*
int main( void ){	
   unsigned int n;
   servo_init();
   timer_init();
   for(;;){
      for( n = 500; n < 2500; n = n + 10 ){
         servo_pulse( n );
         await( now() + 25 * 1000 );
      }          
   }
}*/