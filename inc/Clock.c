// Clock.c
// Runs on the MSP432 
// Daniel and Jonathan Valvano
// September 20, 2016

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


#include <stdint.h>
#include "../inc/CortexM.h"
#include "../inc/msp432p401r.h"

static uint32_t ClockFrequency = 3000000; // cycles/second
//static uint32_t SubsystemFrequency = 3000000; // cycles/second


// ------------Clock_InitFastest------------
// Configure the system clock to run at the fastest
// and most accurate settings.  For example, if the
// LaunchPad has a crystal, it should be used here.
// Call BSP_Clock_GetFreq() to get the current system
// clock frequency for the LaunchPad.
// Input: none
// Output: none
uint32_t Prewait = 0;                   // loops between BSP_Clock_InitFastest() called and PCM idle (expect 0)
uint32_t CPMwait = 0;                   // loops between Power Active Mode Request and Current Power Mode matching requested mode (expect small)
uint32_t Postwait = 0;                  // loops between Current Power Mode matching requested mode and PCM module idle (expect about 0)
uint32_t IFlags = 0;                    // non-zero if transition is invalid
uint32_t Crystalstable = 0;             // loops before the crystal stabilizes (expect small)
void Clock_InitFastest(void){
  // wait for the PCMCTL0 and Clock System to be write-able by waiting for Power Control Manager to be idle
  while(PCMCTL1&0x00000100){
    Prewait = Prewait + 1;
    if(Prewait >= 100000){
      return;                           // time out error
    }
  }
  // request power active mode LDO VCORE1 to support the 48 MHz frequency
  PCMCTL0 = (PCMCTL0&~0xFFFF000F) |     // clear PCMKEY bit field and AMR bit field
            0x695A0000 |                // write the proper PCM key to unlock write access
            0x00000001;                 // request power active mode LDO VCORE1
  // check if the transition is invalid (see Figure 7-3 on p344 of datasheet)
  if(PCMIFG&0x00000004){
    IFlags = PCMIFG;                    // bit 2 set on active mode transition invalid; bits 1-0 are for LPM-related errors; bit 6 is for DC-DC-related error
    PCMCLRIFG = 0x00000004;             // clear the transition invalid flag
    // to do: look at CPM bit field in PCMCTL0, figure out what mode you're in, and step through the chart to transition to the mode you want
    // or be lazy and do nothing; this should work out of reset at least, but it WILL NOT work if Clock_Int32kHz() or Clock_InitLowPower() has been called
    return;
  }
  // wait for the CPM (Current Power Mode) bit field to reflect a change to active mode LDO VCORE1
  while((PCMCTL0&0x00003F00) != 0x00000100){
    CPMwait = CPMwait + 1;
    if(CPMwait >= 500000){
      return;                           // time out error
    }
  }
  // wait for the PCMCTL0 and Clock System to be write-able by waiting for Power Control Manager to be idle
  while(PCMCTL1&0x00000100){
    Postwait = Postwait + 1;
    if(Postwait >= 100000){
      return;                           // time out error
    }
  }
  // initialize PJ.3 and PJ.2 and make them HFXT (PJ.3 built-in 48 MHz crystal out; PJ.2 built-in 48 MHz crystal in)
  PJSEL0 |= 0x0C;
  PJSEL1 &= ~0x0C;                      // configure built-in 48 MHz crystal for HFXT operation
//  PJDIR |= 0x08;                        // make PJ.3 HFXTOUT (unnecessary)
//  PJDIR &= ~0x04;                       // make PJ.2 HFXTIN (unnecessary)
  CSKEY = 0x695A;                       // unlock CS module for register access
  CSCTL2 = (CSCTL2&~0x00700000) |       // clear HFXTFREQ bit field
           0x00600000 |                 // configure for 48 MHz external crystal
           0x00010000 |                 // HFXT oscillator drive selection for crystals >4 MHz
           0x01000000;                  // enable HFXT
  CSCTL2 &= ~0x02000000;                // disable high-frequency crystal bypass
  // wait for the HFXT clock to stabilize
  while(CSIFG&0x00000002){
    CSCLRIFG = 0x00000002;              // clear the HFXT oscillator interrupt flag
    Crystalstable = Crystalstable + 1;
    if(Crystalstable > 100000){
      return;                           // time out error
    }
  }
  // configure for 2 wait states (minimum for 48 MHz operation) for flash Bank 0
  FLCTL_BANK0_RDCTL = (FLCTL_BANK0_RDCTL&~0x0000F000)|FLCTL_BANK0_RDCTL_WAIT_2;
  // configure for 2 wait states (minimum for 48 MHz operation) for flash Bank 1
  FLCTL_BANK1_RDCTL = (FLCTL_BANK1_RDCTL&~0x0000F000)|FLCTL_BANK1_RDCTL_WAIT_2;
  CSCTL1 = 0x20000000 |                 // configure for SMCLK divider /4
           0x00100000 |                 // configure for HSMCLK divider /2
           0x00000200 |                 // configure for ACLK sourced from REFOCLK
           0x00000050 |                 // configure for SMCLK and HSMCLK sourced from HFXTCLK
           0x00000005;                  // configure for MCLK sourced from HFXTCLK
  CSKEY = 0;                            // lock CS module from unintended access
  ClockFrequency = 48000000;
//  SubsystemFrequency = 12000000;
}

// ------------Clock_GetFreq------------
// Return the current system clock frequency for the
// LaunchPad.
// Input: none
// Output: system clock frequency in cycles/second
uint32_t Clock_GetFreq(void){
  return ClockFrequency;
}