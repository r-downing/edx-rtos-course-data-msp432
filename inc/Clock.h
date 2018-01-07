// Clock.h
// Runs on the MSP432 
// Daniel and Jonathan Valvano
// July 21, 2016

/* This example accompanies the books
   "Embedded Systems: Introduction to the MSP432 Microcontroller",
   ISBN: 978-1512185676, Jonathan Valvano, copyright (c) 2016

   "Embedded Systems: Real-Time Interfacing to the MSP432 Microcontroller",
   ISBN: 978-1514676585, Jonathan Valvano, copyright (c) 2016

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


// ------------Clock_InitFastest------------
// Configure the system clock to run at the fastest
// and most accurate settings.  For example, if the
// LaunchPad has a crystal, it should be used here.
// Call BSP_Clock_GetFreq() to get the current system
// clock frequency for the LaunchPad.
// Input: none
// Output: none
void Clock_InitFastest(void);
 
// ------------Clock_GetFreq------------
// Return the current system clock frequency for the
// LaunchPad.
// Input: none
// Output: system clock frequency in cycles/second
uint32_t Clock_GetFreq(void);





