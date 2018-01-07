// Profile.c
// Runs on MSP432
// Implement the Profile pin initialization and Profile pin
// packaged read on the
// Daniel and Jonathan Valvano
// June 14, 2016

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

// The following pins were selected to be Profile pins:
// J3.27/P4.5 Profile 0
// J3.28/P4.7 Profile 1
// J3.29/P5.4 Profile 2
// J3.30/P5.5 Profile 3
// J2.18/P3.0 Profile 4
// J4.36/P6.6 Profile 5
// J4.34/P2.3 Profile 6
// If it turns out that these pins are needed by another
// BoosterPack, such as the Bluetooth one, then the
// Profile initialization and get functions will need to
// be modified.

#include <stdint.h>
#include "../inc/msp432p401r.h"

// ------------Profile_Init------------
// Initialize all Profile pins for GPIO output,
// which correspond with unused BoosterPack pins.
// The Profile pins can be set, cleared, or toggled at
// strategic times and observed with a logic analyzer.
// This function should be called once at the beginning.
// Input: none
// Output: none
void Profile_Init(void){
  P2SEL0 &= ~0x08;
  P2SEL1 &= ~0x08;                 // configure P2.3 as GPIO
  P2DIR |= 0x08;                   // make P2.3 out
  P2REN &= ~0x08;                  // disable pull resistor on P2.3
  P2OUT &= ~0x08;                  // initialize P2.3 low
  P3SEL0 &= ~0x01;
  P3SEL1 &= ~0x01;                 // configure P3.0 as GPIO
  P3DIR |= 0x01;                   // make P3.0 out
  P3REN &= ~0x01;                  // disable pull resistor on P3.0
  P3OUT &= ~0x01;                  // initialize P3.0 low
  P4SEL0 &= ~0xA0;
  P4SEL1 &= ~0xA0;                 // configure P4.7 and P4.5 as GPIO
  P4DIR |= 0xA0;                   // make P4.7 and P4.5 out
  P4REN &= ~0xA0;                  // disable pull resistor on P4.7 and P4.5
  P4OUT &= ~0xA0;                  // initialize P4.7 and P4.5 low
  P5SEL0 &= ~0x30;
  P5SEL1 &= ~0x30;                 // configure P5.5 and P5.4 as GPIO
  P5DIR |= 0x30;                   // make P5.5 and P5.4 out
  P5REN &= ~0x30;                  // disable pull resistor on P5.5 and P5.4
  P5OUT &= ~0x30;                  // initialize P5.5 and P5.4 low
  P6SEL0 &= ~0x40;
  P6SEL1 &= ~0x40;                 // configure P6.6 as GPIO
  P6DIR |= 0x40;                   // make P6.6 out
  P6REN &= ~0x40;                  // disable pull resistor on P6.6
  P6OUT &= ~0x40;                  // initialize P6.6 low
}

// ------------Profile_Get------------
// Return the current status of all Profile pins.
// Profile 0 is in bit 0. Profile 1 is in bit 1, and
// so on.
// The most significant bit is set to tell the PC
// that these values are data and not standard ASCII
// characters.
// Input: none
// Output: 7-bit status of all Profile pins
// Assumes: Profile_Init() has been called
uint8_t Profile_Get(void){
  return (((P4OUT)&0x20)>>5) |     // Profile 0
         (((P4OUT)&0x80)>>6) |     // Profile 1
         (((P5OUT)&0x30)>>2) |     // Profiles 2, 3
         (((P3OUT)&0x01)<<4) |     // Profile 4
         (((P6OUT)&0x40)>>1) |     // Profile 5
         (((P2OUT)&0x08)<<3) |     // Profile 6
         0x80;                     // set most significant bit
}
