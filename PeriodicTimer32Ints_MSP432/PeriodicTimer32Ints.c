// PeriodicTimer32Ints.c
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

// oscilloscope or LED connected to P2.2-P2.0 for period measurement
// When using the color wheel, the blue LED on P2.2 is on for four
// consecutive interrupts then off for four consecutive interrupts.
// Blue is off for: dark, red, yellow, green
// Blue is on for: light blue, blue, purple, white
// Therefore, the frequency of the pulse measured on P2.2 is 1/8 of
// the frequency of the Timer32 interrupts.

#include <stdint.h>
#include "..\inc\msp432p401r.h"
#include "ClockSystem.h"
#include "Timer32.h"

#define RED       0x01
#define GREEN     0x02
#define BLUE      0x04
#define WHEELSIZE 8           // must be an integer power of 2
                              //    red, yellow,    green, light blue, blue, purple,   white,          dark
const long COLORWHEEL[WHEELSIZE] = {RED, RED+GREEN, GREEN, GREEN+BLUE, BLUE, BLUE+RED, RED+GREEN+BLUE, 0};

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

void UserTask(void){
  static int i = 0;
  P2OUT = (P2OUT&~0x07)|(COLORWHEEL[i&(WHEELSIZE-1)]);
//  P2OUT ^= 0x01; // toggle
  i = i + 1;
}
// if desired interrupt frequency is f, Timer32_1_Init parameter is (bus (master) clock (MCLK))/DIV/f
// if this number exceeds 32 bits, increase divider parameter: DIV (1, 16, or 256)
// bus (master) clock (MCLK) is 48 MHz after Clock_Init48MHz() is called
// P2.2 (built-in blue LED) frequency is (f/8)
#define DIV    256
#define F200HZ (48000000/200/DIV)
#define F20KHZ (48000000/20000/DIV)
#define F5SEC  (48000000/0.2/DIV)  // interrupt every 5 seconds (0.2 Hz)

//debug code
int main(void){
  Clock_Init48MHz();               // bus clock at 48 MHz
                                   // initialize Timer32 (20,000 Hz)
//  Timer32_1_Init(&UserTask, F20KHZ, T32DIV256);
                                   // initialize Timer32 (200 Hz)
//  Timer32_1_Init(&UserTask, F200HZ, T32DIV256);
                                   // initialize Timer32 (0.2 Hz)
  Timer32_1_Init(&UserTask, F5SEC, T32DIV256);
  // initialize P2.2-P2.0 and make them outputs (P2.2-P2.0 built-in RGB LEDs)
  P2SEL0 &= ~0x07;
  P2SEL1 &= ~0x07;                 // configure built-in RGB LEDs as GPIO
  P2DS |= 0x07;                    // make built-in RGB LEDs high drive strength
  P2DIR |= 0x07;                   // make built-in RGB LEDs out
  P2OUT &= ~0x07;                  // RGB = off
  P2DS |= 0x07;                    // 20mA max current
  EnableInterrupts();
  while(1){
    WaitForInterrupt();
  }
}
