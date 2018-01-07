// TimerA1.c
// Runs on MSP432
// Use Timer A1 in periodic mode to request interrupts at a particular
// period.
// Daniel Valvano
// July 20, 2015

/* This example accompanies the books
   "Embedded Systems: Introduction to MSP432 Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015
   Volume 1, Program 9.8

   "Embedded Systems: Real-Time Interfacing to the MSP432 Microcontroller",
   ISBN: 978-1514676585, Jonathan Valvano, copyright (c) 2015
   Programs 6.7 and 6.8

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

// ***************** TimerA1_Init ****************
// Activate Timer A1 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/SMCLK), 16 bits
// Outputs: none
void TimerA1_Init(void(*task)(void), uint16_t period){long sr;
  sr = StartCritical();
  PeriodicTask = task;  // user function
  TA1CTL &= ~0x0030;    // halt Timer A1
  // bits15-10=XXXXXX, reserved
  // bits9-8=10,       clock source to SMCLK
  // bits7-6=00,       input clock divider /1
  // bits5-4=00,       stop mode
  // bit3=X,           reserved
  // bit2=0,           set this bit to clear
  // bit1=0,           no interrupt on timer
  // bit0=0,           clear interrupt pending
  TA1CTL = 0x0200;
  TA1EX0 &= ~0x0007;  // configure for input clock divider /1
  // bits15-14=00,     no capture mode
  // bits13-12=XX,     capture/compare input select
  // bit11=X,          synchronize capture source
  // bit10=X,          synchronized capture/compare input
  // bit9=X,           reserved
  // bit8=0,           compare mode
  // bits7-5=XXX,      output mode
  // bit4=1,           enable capture/compare interrupt on CCIFG
  // bit3=X,           read capture/compare input from here
  // bit2=0,           output this value in output mode 0
  // bit1=X,           capture overflow status
  // bit0=0,           clear capture/compare interrupt pending
  TA1CCTL0 = 0x0010;
  TA1CCR0 = (period - 1);  // compare match value
// interrupts enabled in the main program after all devices initialized
  NVIC_IPR2 = (NVIC_IPR2&0xFF00FFFF)|0x00400000; // priority 2
  NVIC_ISER0 = 0x00000400; // enable interrupt 10 in NVIC
  TA1CTL |= 0x0014;        // reset and start Timer A1 in up mode
  EndCritical(sr);
}

void TA1_0_IRQHandler(void){
  TA1CCTL0 &= ~0x0001;           // acknowledge capture/compare interrupt 0
  (*PeriodicTask)();             // execute user task
}


// Volume 2, program 6.8
#define P40OUT (*((volatile uint8_t *)(0x42000000+32*0x4C23+4*0)))
#define P41OUT (*((volatile uint8_t *)(0x42000000+32*0x4C23+4*1)))
#define P42OUT (*((volatile uint8_t *)(0x42000000+32*0x4C23+4*2)))

#define N1 1000
void Task1(void){
 P40OUT ^= 0x01;  // every 1ms
}
#define N2 1500
void Task2(void){
 P41OUT ^= 0x01; // every 1.5ms
}
#define N3 10000
void Task3(void){
 P42OUT ^= 0x01; // every 10ms
}
void TimerA0_Init(void){
  P4DIR |= 0x07; // debugging
  TA0CTL &= ~0x0030;       // 0) halt Timer A0
  TA0CTL = 0x0240;         // 1) SMCLK, divide by 2
  TA0EX0 = 0x0005;         //    divide by 6
  TA0CCTL1 = 0x0010;       // 2) compare mode, arm CCIFG
  TA0CCTL2 = 0x0010;       //    compare mode, arm CCIFG
  TA0CCTL3 = 0x0010;       //    compare mode, arm CCIFG
  TA0CCR1 = N1/2;          // 3) time of first interrupt
  TA0CCR2 = N2/2;          //
  TA0CCR3 = N3/2;          //
  NVIC_IPR2 = (NVIC_IPR2&0xFFFF00FF)|0x00004000; // 4) priority 2
  NVIC_ISER0 = 0x00000200; // 5) enable interrupt 9 in NVIC
  TA0CTL |= 0x0024;        // 6) reset and start in continuous mode
  EnableInterrupts(); }    // 7) enable interrupts
void TA0_N_IRQHandler(void){
  if(TA0CCTL1&0x0001){
    TA0CCTL1 &= ~0x0001;   // acknowledge compare interrupt 1
    TA0CCR1 = TA0CCR1+N1;  // set up for next time
    Task1();}              // execute user task
  if(TA0CCTL2&0x0001){
    TA0CCTL2 &= ~0x0001;   // acknowledge compare interrupt 2
    TA0CCR2 = TA0CCR2+N2;  // set up for next time
    Task2();}              // execute user task
  if(TA0CCTL3&0x0001){
    TA0CCTL3 &= ~0x0001;   // acknowledge compare interrupt 3
    TA0CCR3 = TA0CCR3+N3;  // set up for next time
    Task3();}              // execute user task
}

void TimerA0_Initb(void){
  P4DIR |= 0x07; // debugging
  P2SEL0 |= 0x70;    // Timer A0
  P2SEL1 &= ~0x70;   // see Table 2.3
  P2DIR |= 0x70;     // output
  TA0CTL &= ~0x0030;       // 0) halt Timer A0
  TA0CTL = 0x0240;         // 1) SMCLK, divide by 2
  TA0EX0 = 0x0005;         //    divide by 6
  TA0CCTL1 = 0x0090;       // 2) compare mode, OUTMOD=4, arm CCIFG
  TA0CCTL2 = 0x0090;       //    compare mode, OUTMOD=4, arm CCIFG
  TA0CCTL3 = 0x0090;       //    compare mode, OUTMOD=4, arm CCIFG
  TA0CCR1 = N1/2;          // 3) time of first interrupt
  TA0CCR2 = N2/2;          //
  TA0CCR3 = N3/2;          //
  NVIC_IPR2 = (NVIC_IPR2&0xFFFF00FF)|0x00004000; // 4) priority 2
  NVIC_ISER0 = 0x00000200; // 5) enable interrupt 9 in NVIC
  TA0CTL |= 0x0024;        // 6) reset and start in continuous mode
  EnableInterrupts(); }    // 7) enable interrupts

