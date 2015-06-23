//! @file
//! This library can be used for shift registers, to extend the IO-pins on the microcontroller

//! initialize the shift_register
//
//! Call this function to initialize the Shift-register pins, port, data, shift-clock, store-clock
void init_shift_reg(int p, int da, int shi, int str);

//! Move one data-bit
//
//! This moves one setted data-bit (call set_data_bit first)
void move_bit();

//! Enable all shifted bits
//
//! This is used to enable all stored data bits in all the shift-registers (connected to cascade output)
void shift_all_bits();

//! Set one data-bit
//
//! Function to set one bit in the storage of the shift-register (high/low)
void set_data_bit(unsigned int x);

//! Write 8-bits number to shift-register
//
//! Function to write a one byte number to the shift-register
void write_number_to_reg_8(int num);

//! Write 12-bits number to shift-register, without SHIFT_ALL
//
//! Function to write a one byte number to the shift-register
void write_array_to_reg_12(int shift_array[12]);

//! Write 16-bits number to shift-register
//
//! Function to write a two byte number to the shift-register
void write_number_to_reg_16(int num);

//! Write 16-bits array to shift-register
//
//! Function to write a two byte array to the shift-register
void write_array_to_reg(int shift_array[16]);