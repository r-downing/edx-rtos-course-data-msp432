// TimerA0.c
// Runs on MSP432
// Use Timer A0 in periodic mode to request interrupts at a particular
// period.
// Daniel Valvano
// May 21, 2015

/* This example accompanies the books
   "Embedded Systems: Introduction to MSP432 Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015
   Volume 1, Program 9.8


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

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
void (*PeriodicTask)(void);   // user function

// ***************** TimerA0_Init ****************
// Activate Timer A0 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/SMCLK), 16 bits
// Outputs: none
void TimerA0_Init(void(*task)(void), uint16_t period){long sr;
  sr = StartCritical();
  PeriodicTask = task;             // user function
  TA0CTL &= ~0x0030;               // halt Timer A0
  // bits15-10=XXXXXX, reserved
  // bits9-8=10,       clock source to SMCLK
  // bits7-6=00,       input clock divider /1
  // bits5-4=00,       stop mode
  // bit3=X,           reserved
  // bit2=0,           set this bit to clear
  // bit1=1,           interrupt enable
  // bit0=0,           clear interrupt pending
  TA0CTL = 0x0202;
  // bits15-14=00,     no capture mode
  // bits13-12=XX,     capture/compare input select
  // bit11=X,          synchronize capture source
  // bit10=X,          synchronized capture/compare input
  // bit9=X,           reserved
  // bit8=0,           compare mode
  // bits7-5=XXX,      output mode
  // bit4=1,           enable capture/compare interrupt
  // bit3=X,           read capture/compare input from here
  // bit2=0,           output this value in output mode 0
  // bit1=X,           capture overflow status
  // bit0=0,           clear capture/compare interrupt pending
  TA0CCTL0 = 0x0010;
  TA0CCR0 = (period - 1);          // compare match value
  TA0EX0 &= ~0x0007;               // configure for input clock divider /1
  NVIC_IPR2 = (NVIC_IPR2&0xFFFFFF00)|0x00000040; // priority 2
// interrupts enabled in the main program after all devices initialized
  NVIC_ISER0 = 0x00000100;         // enable interrupt 8 in NVIC
  TA0CTL |= 0x0014;                // reset and start Timer A0 in up mode
  EndCritical(sr);
}

void TA0_0_IRQHandler(void){
  TA0CCTL0 &= ~0x0001;           // acknowledge capture/compare interrupt 0
  (*PeriodicTask)();             // execute user task
}
