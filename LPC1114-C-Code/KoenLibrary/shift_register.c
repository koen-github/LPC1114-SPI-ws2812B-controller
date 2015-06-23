#include "LPC11xx.h"
#include "pin_functions.h"
#include "timer.h"
#include "sw-uart.h"

static int port;
static int data;
static int shift;
static int store;

void init_shift_reg(int p, int da, int shi, int str){
	port = p;
	data = da;
	shift = shi;
	store = str;
	
	pin_configure_as_output(port, shift); //shift
    pin_configure_as_output(port, store); //store
    pin_configure_as_output(port, data); //data
		
	pin_set(port, shift, 0); //default value
	pin_set(port, store, 0); 

}


void move_bit(){
   delay(1);
   pin_set(port, shift, 1);
   delay(1);
   pin_set(port, shift, 0);
   delay(1);
}

void shift_all_bits(){
	delay(1);
	pin_set(port, store, 1); //clock low
	delay(1);
	pin_set(port, store, 0); //clock low
	delay(1);
}		

void set_data_bit(unsigned int x){
	delay(1);
	pin_set(port, shift, 0); //make clock low
	delay(1);
	pin_set(port, data, x); //data pin to x
	move_bit();
}

void write_number_to_reg_8(int num){
	
	pin_set(port, store, 0);
	int i ;
	for (i=0; i<8;i++){
		set_data_bit(num & (1 << (8 - 1 - i)));
	}	
	shift_all_bits();
}

void write_array_to_reg_12(int shift_array[12]){
	int i;
	for (i=0; i<12;i++){
		//registers[i] = 
		set_data_bit(shift_array[i]);
	}		
}

void write_number_to_reg_16(int num){
	int i;
	for (i=0; i<16;i++){
		//registers[i] = 
		set_data_bit(num & (1 << (16 - 1 - i)));
	}		
	shift_all_bits();
}

void write_array_to_reg(int shift_array[16]){
	int i;
	for (i=0; i<16;i++){
		//registers[i] = 
		set_data_bit(shift_array[i]);
	}		
	shift_all_bits();
}
		
	