// Timer32.h
// Runs on MSP432
// Use Timer32 in periodic mode to request interrupts at a particular
// period.
// Daniel Valvano
// July 20, 2015

/* This example accompanies the book
   "Embedded Systems: Introduction to MSP432 Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
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

#ifndef __TIMER32INTS_H__ // do not include more than once
#define __TIMER32INTS_H__

enum timer32divider{
  T32DIV1 = 0x00000000,            // maximum period of about 89 seconds at 48 MHz
  T32DIV16 = 0x00000004,           // maximum period of about 23 minutes at 48 MHz
  T32DIV256 = 0x00000008           // maximum period of about 6 hours, 21 minutes at 48 MHz
};

// ***************** Timer32_1_Init ****************
// Activate Timer32 Timer 1 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/(bus clock)/div), 32 bits
//          div is clock divider for Timer32 Timer 1
//            T32DIV1   for input clock divider /1
//            T32DIV16  for input clock divider /16
//            T32DIV256 for input clock divider /256
// Outputs: none
void Timer32_1_Init(void(*task)(void), uint32_t period, enum timer32divider div);

#endif // __TIMER32INTS_H__
