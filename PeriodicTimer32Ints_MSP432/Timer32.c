// Timer32.c
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

#include <stdint.h>
#include "..\inc\msp432p401r.h"
#include "Timer32.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
void (*PeriodicTask)(void);   // user function

// ***************** Timer32_1_Init ****************
// Activate Timer32 Timer 1 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/(bus clock)/div), 32 bits
//          div is clock divider for Timer32 Timer 1
//            T32DIV1   for input clock divider /1
//            T32DIV16  for input clock divider /16
//            T32DIV256 for input clock divider /256
// Outputs: none
void Timer32_1_Init(void(*task)(void), uint32_t period, enum timer32divider div){long sr;
  sr = StartCritical();
  PeriodicTask = task;             // user function
  TIMER32_LOAD1 = (period - 1);    // timer reload value
  TIMER32_INTCLR1 = 0x00000001;    // clear Timer32 Timer 1 interrupt
  // bits31-8=X...X,   reserved
  // bit7=1,           timer enable
  // bit6=1,           timer in periodic mode
  // bit5=1,           interrupt enable
  // bit4=X,           reserved
  // bits3-2=??,       input clock divider according to parameter
  // bit1=1,           32-bit counter
  // bit0=0,           wrapping mode
  TIMER32_CONTROL1 = 0x000000E2|div;
// interrupts enabled in the main program after all devices initialized
  NVIC_IPR6 = (NVIC_IPR6&0xFFFF00FF)|0x00004000; // priority 2
  NVIC_ISER0 = 0x02000000;         // enable interrupt 25 in NVIC
  EndCritical(sr);
}

void T32_INT1_IRQHandler(void){
  TIMER32_INTCLR1 = 0x00000001;    // acknowledge Timer32 Timer 1 interrupt
  (*PeriodicTask)();               // execute user task
}
