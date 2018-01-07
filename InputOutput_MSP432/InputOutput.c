// InputOutput.c
// Runs on MSP432
// Test the switch initialization functions by setting the LED
// color according to the status of the switches.
// Daniel and Jonathan Valvano
// July 19, 2016

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
#include "../inc/msp432p401r.h"

#define RED       0x01
#define GREEN     0x02
#define BLUE      0x04

void Port1_Init(void){
  P1SEL0 &= ~0x12;
	// 1110 1101
  P1SEL1 &= ~0x12;    // 1) configure P1.4 and P1.1 as GPIO
  P1DIR &= ~0x12;     // 2) make P1.4 and P1.1 in
  P1REN |= 0x12;      // 3) enable pull resistors on P1.4 and P1.1
  P1OUT |= 0x12;      //    P1.4 and P1.1 are pull-up
}
uint8_t Port1_Input(void){
  return (P1IN&0x12);   // read P1.4,P1.1 inputs
}

uint8_t Port1_PinRead(uint16_t pin) {
	return (P1IN&(1<<pin));
}

void Port2_Init(void){
  P2SEL0 &= ~0x07;
	// 1111 1000
  P2SEL1 &= ~0x07;    // 1) configure P2.2-P2.0 as GPIO
  P2DIR |= 0x07;      // 2) make P2.2-P2.0 out
  P2DS |= 0x07;       // 3) activate increased drive strength
  P2OUT &= ~0x07;     //    all LEDs off
}
#define SIZE 10
uint32_t DataBuf[SIZE];  // debugging dump
uint32_t Last=0xFFFF;    // pattern that will not match
uint32_t Index=0;
void Dump(uint32_t x){   // filter:save data only if different
  if((x!=Last)&&(Index<SIZE)){
    DataBuf[Index] = x;  // save 
    Last = x;            // set up for next time
    Index++;
  }
}
void Port2_Output(uint8_t data){  // write three outputs bits of P2
  P2OUT = (P2OUT&0xF8)|data;
  Dump(((P1IN&0x12)<<8)+(P2OUT&0x07));   // debugging dump
}

 uint8_t status;

int main(void){
  Port1_Init();              // initialize P1.1 and P1.4 and make them inputs
  Port2_Init();              // initialize P2.2-P2.0 and make them outputs (P2.2-P2.0 built-in LEDs)
  while(1){
    //status = Port1_Input();
		status = Port1_PinRead(1) | Port1_PinRead(4);
    switch(status){                     // switches are negative logic on P1.1 and P1.4
      case 0x10: Port2_Output(BLUE); break;   // SW1 pressed
      case 0x02: Port2_Output(RED); break;    // SW2 pressed
      case 0x00: Port2_Output(GREEN); break;  // both switches pressed
      case 0x12: Port2_Output(0); break;      // neither switch pressed
    }
  }
}
// Color    LED(s) Port2
// dark     ---    0
// red      R--    0x01
// blue     --B    0x04
// green    -G-    0x02
// yellow   RG-    0x03
// sky blue -GB    0x06
// white    RGB    0x07
// pink     R-B    0x05

