#include "LPC11xx.h"


//! Configure pin as input
//
//! This function sets the given pin as input pin.
void pin_configure_as_input( int port, int pin ){
    if(port){
        LPC_GPIO1->DIR = LPC_GPIO1->DIR & ~ ( 1 << pin );
        
    }
    else{
        LPC_GPIO0->DIR = LPC_GPIO0->DIR & ~ ( 1 << pin );
        
    }
}


//! Configure pin as output
//
//! This function sets the given pin as output pin.
void pin_configure_as_output( int port, int pin ){
    if(port){
        LPC_GPIO1->DIR = LPC_GPIO1->DIR | ( 1 << pin );
        
    }
    else{
        LPC_GPIO0->DIR = LPC_GPIO0->DIR | ( 1 << pin );
        
    }
}


//! Set pin low or high
//
//! This function sets the given pin to low or high
void pin_set( int port, int pin, unsigned char x ){
    if( x ){
        if(port){
            LPC_GPIO1->DATA = LPC_GPIO1->DATA | ( 1 << pin );
            
        }
        else{
            LPC_GPIO0->DATA = LPC_GPIO0->DATA | ( 1 << pin );
            
        }
        
	} else {
        if(port){
            LPC_GPIO1->DATA = LPC_GPIO1->DATA & ~ ( 1 << pin );
            
        }
        else{
            LPC_GPIO0->DATA = LPC_GPIO0->DATA & ~ ( 1 << pin );
            
        }
        
    }
    
}


//! Return the pin status
//
//! This function returns low or high with the given pin number
unsigned char pin_get( int port, int pin ){
    if(port){
        return ( LPC_GPIO1->DATA & ( 1 << pin ) ) == 0;
        
    } else
    {
        return ( LPC_GPIO0->DATA & ( 1 << pin ) ) == 0;
        
    }
}
