// InputOutput.c
// Runs on MSP432
// Falling edge interrupts on two LaunchPad switches.
// Daniel and Jonathan Valvano
// May 24, 2016

/* This example accompanies the book
   "Embedded Systems: Introduction to the MSP432 Microcontroller",
   ISBN: 978-1512185676, Jonathan Valvano, copyright (c) 2016
   Section 4.2    Program 4.1

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

// built-in LED1 connected to P1.0
// negative logic built-in Button 1 connected to P1.1
// negative logic built-in Button 2 connected to P1.4
// built-in red LED connected to P2.0
// built-in green LED connected to P2.1
// built-in blue LED connected to P2.2

#include <stdint.h>
#include "../inc/CortexM.h"
#include "../inc/msp432p401r.h"

void Port2_Init(void){
  P2SEL0 &= ~0x07;
  P2SEL1 &= ~0x07;    // 1) configure P2.2-P2.0 as GPIO
  P2DIR |= 0x07;      // 2) make P2.2-P2.0 out
  P2DS |= 0x07;       // 3) activate increased drive strength
  P2OUT &= ~0x07;     //    all LEDs off
}
void Port2_Output(uint8_t data){  // write three outputs bits of P2
  P2OUT = (P2OUT&0xF8)|data;
}
void OS_Signal(int32_t *p){
  (*p)++; // not the real OS, but similar in behavior
}
void EnableInterrupts(void);

int32_t SW1,SW2;
void Switch_Init(void){
  SW1 = SW2 = 0;                // (a) initialize semaphores
  P1SEL1 &= ~0x12;              // (b) configure P1.1, P1.4 as GPIO
  P1SEL0 &= ~0x12;              //     built-in Buttons 1 and 2
  P1DIR &= ~0x12;               //     make P1.1, P1.4 in
  P1REN |= 0x12;                //     enable pull resistors
  P1OUT |= 0x12;                //     P1.1, P1.4 is pull-up
  P1IES |= 0x12;                // (c) P1.1, P1.4 is falling edge event
  P1IFG &= ~0x12;               // (d) clear flag1 and flag4
  P1IE |= 0x12;                 // (e) arm interrupt on P1.1, P1.4
  NVIC_IPR8 = (NVIC_IPR8&0x00FFFFFF)|0x40000000; // (f) priority 2
  NVIC_ISER1 = 0x00000008;      // (g) enable interrupt 35 in NVIC
}
void PORT1_IRQHandler(void){ uint8_t status;
  status = P1IV; // 4 for P1.1 and 10 for P1.4
  if(status == 4){
    OS_Signal(&SW1);// SW1 occurred
  }
  if(status == 10){
    OS_Signal(&SW2);// SW2 occurred
  }
}
int main(void){
  DisableInterrupts();
  Switch_Init();
  Port2_Init();
  EnableInterrupts();
  while(1){
    if(SW1){
      SW1 = 0;
      Port2_Output(1);
    }
    if(SW2){
      SW2 = 0;
      Port2_Output(0);
    }
  }
}
