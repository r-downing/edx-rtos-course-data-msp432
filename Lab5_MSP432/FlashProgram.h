// FlashProgram.h
// Runs on MSP432
// Provide functions that initialize the flash memory, write
// 32-bit data to flash, write an array of 32-bit data to flash,
// and erase a 4 KB block.
// Daniel Valvano
// April 21, 2016

/* This example accompanies the book
   "Embedded Systems: Real-Time Interfacing to the MSP432 Microcontroller",
   ISBN: 978-1514676585, Jonathan Valvano, copyright (c) 2016

   "Embedded Systems: Real-Time Operating Systems for ARM Cortex-M Microcontrollers",
   ISBN: 978-1466468863, Jonathan Valvano, copyright (c) 2016

 Copyright 2016 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

#define ERROR                   1           // Value returned if failure
#define NOERROR                 0           // Value returned if success

//------------Flash_Init------------
// This function was critical to the write and erase
// operations of the flash memory on the LM3S811
// microcontroller.  But newer processors work slightly
// differently, and for the MSP432 the timing parameters
// for the flash and EEPROM memories are configured along
// with the Clock System.  This function prototype is
// preserved to try to make it easier to reuse program
// code between the LM3S811, TM4C123, and TM4C1294.
// Input: systemClockFreqMHz  system clock frequency (units of MHz)
// Output: none
void Flash_Init(uint8_t systemClockFreqMHz);

//------------Flash_Write------------
// Write 32-bit data to flash at given address.  Parameter
// 'addr' must be in flash Bank 1, and this function must be in
// flash Bank 0.
// Input: addr 4-byte aligned flash memory address to write
//        data 32-bit data
// Output: 'NOERROR' if successful, 'ERROR' if fail (defined in FlashProgram.h)
// Note: This function is not interrupt safe.
int Flash_Write(uint32_t addr, uint32_t data);

//------------Flash_WriteArray------------
// Write an array of 32-bit data to flash starting at given address.
// Parameter 'addr' must be in flash Bank 1.
// Input: source pointer to array of 32-bit data
//        addr   4-byte aligned flash memory address to start writing
//        count  number of 32-bit writes
// Output: number of successful writes; return value == count if completely successful
// Note: at 48 MHz, it takes 612 usec to write 10 words
// Note: This function is not interrupt safe.
int Flash_WriteArray(uint32_t *source, uint32_t addr, uint16_t count);

//------------Flash_FastWrite------------
// Write an array of 32-bit data to flash starting at given address.
// This is twice as fast as Flash_WriteArray(), but the address has
// to be 128-byte aligned, and the count has to be <= 16.  Parameter
// 'addr' must be in flash Bank 1, and this function must be in
// flash Bank 0.
// Input: source pointer to array of 32-bit data
//        addr   128-byte aligned flash memory address to start writing
//        count  number of 32-bit writes (<=16)
// Output: number of successful writes; return value == min(count, 16) if completely successful
// Note: at 48 MHz, it takes 97 usec to write 10 words
// Note: This function is not interrupt safe.
int Flash_FastWrite(uint32_t *source, uint32_t addr, uint16_t count);

//------------Flash_Erase------------
// Erase 4 KB block of flash.  Parameter 'addr' must be in
// flash Bank 1, and this function must be in flash Bank 0.
// Input: addr 4-KB aligned flash memory address to erase
// Output: 'NOERROR' if successful, 'ERROR' if fail (defined in FlashProgram.h)
// Note: This function is not interrupt safe.
int Flash_Erase(uint32_t addr);
