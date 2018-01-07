// SysTickInts.c
// Runs on MSP432
// Use the SysTick timer to request interrupts at a particular period.
// Daniel Valvano, Jonathan Valvano
// February 9, 2016

/* This example accompanies the books
   "Embedded Systems: Introduction to MSP432 Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2016
   Volume 1 Program 9.7

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

// oscilloscope or LED connected to J3.27/P4.5 for period measurement

#include <stdint.h>
#include "../inc/CortexM.h"
#include "../inc/Profile.h"

// **************SysTick_Init*********************
// Initialize SysTick periodic interrupts
// Input: interrupt period
//        Units of period are 333ns (assuming 3 MHz clock)
//        Maximum is 2^24-1
//        Minimum is determined by length of ISR
// Output: none
volatile uint32_t Counts = 0;
void SysTick_Init(uint32_t period){ long sr;
  sr = StartCritical();
  Profile_Init();
  Counts = 0;
  STCTRL = 0;                      // 1) disable SysTick during setup
  STRELOAD = period - 1;           // 2) reload value sets period
  STCURRENT = 0;                   // 3) any write to current clears it
  SYSPRI3 = (SYSPRI3&0x00FFFFFF)|0x40000000; // priority 2
  STCTRL = 0x00000007;             // 4) enable SysTick with core clock and interrupts
  EndCritical(sr);
}
// Interrupt service routine
// Executed every 333ns*(period)
void SysTick_Handler(void){
  Profile_Toggle0();          // toggle bit
  Profile_Toggle0();          // toggle bit
  Counts = Counts + 1;
  Profile_Toggle0();          // toggle bit
}

int main(void){// bus clock at 3 MHz
  SysTick_Init(300000);       // initialize SysTick timer
  EnableInterrupts();
  while(1){                   // interrupts every 100ms, 5 Hz flash
    Profile_Toggle1();        // toggle bit
  }
}

