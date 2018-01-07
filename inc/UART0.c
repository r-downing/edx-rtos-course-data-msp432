// UART0.c
// Runs on MSP432
// Busy-wait device driver for the UART UCA0.
// Daniel Valvano
// May 24, 2015
// Modified by EE345L students Charlie Gough && Matt Hawk
// Modified by EE345M students Agustinus Darmawan && Mingjie Qiu

/* This example accompanies the book
   "Embedded Systems: Introduction to the MSP432 Microcontroller",
   ISBN: 978-1512185676, Jonathan Valvano, copyright (c) 2016

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

// UCA0RXD (VCP receive) connected to P1.2
// UCA0TXD (VCP transmit) connected to P1.3

#include <stdint.h>
#include "UART0.h"
#include "../inc/msp432p401r.h"

//------------UART0_Init------------
// Initialize the UART for 115,200 baud rate (assuming 12 MHz SMCLK clock),
// 8 bit word length, no parity bits, one stop bit
// Input: none
// Output: none
void UART0_Init(void){
  UCA0CTLW0 = 0x0001;                   // hold the USCI module in reset mode
  // bit15=0,      no parity bits
  // bit14=x,      not used when parity is disabled
  // bit13=0,      LSB first
  // bit12=0,      8-bit data length
  // bit11=0,      1 stop bit
  // bits10-8=000, asynchronous UART mode
  // bits7-6=11,   clock source to SMCLK
  // bit5=0,       reject erroneous characters and do not set flag
  // bit4=0,       do not set flag for break characters
  // bit3=0,       not dormant
  // bit2=0,       transmit data, not address (not used here)
  // bit1=0,       do not transmit break (not used here)
  // bit0=1,       hold logic in reset state while configuring
  UCA0CTLW0 = 0x00C1;
                                        // set the baud rate
                                        // N = clock/baud rate = 12,000,000/115,200 = 104.1667
  UCA0BRW = 104;                        // UCBR = baud rate = int(N) = 104
  UCA0MCTLW &= ~0xFFF1;                 // clear first and second modulation stage bit fields
                                        // configure second modulation stage select (from Table 22-4 on p731 of datasheet)
//  UCA0MCTLW |= (0<<8);                  // UCBRS = N - int(N) = 0.0417; plug this in Table 22-4
                                        // configure first modulation stage select (ignored when oversampling disabled)
//  UCA0MCTLW |= (10<<4);                 // UCBRF = int(((N/16) - int(N/16))*16) = 10
//  UCA0MCTLW |= 0x0001;                  // enable oversampling mode
  P1SEL0 |= 0x0C;
  P1SEL1 &= ~0x0C;                      // configure P1.3 and P1.2 as primary module function
  UCA0CTLW0 &= ~0x0001;                 // enable the USCI module
  UCA0IE &= ~0x000F;                    // disable interrupts (transmit ready, start received, transmit empty, receive full)
}

//------------UART0_InChar------------
// Wait for new serial port input
// Input: none
// Output: ASCII code for key typed
char UART0_InChar(void){
  while((UCA0IFG&0x01) == 0);
  return((char)(UCA0RXBUF));
}

//------------UART0_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void UART0_OutChar(char letter){
  while((UCA0IFG&0x02) == 0);
  UCA0TXBUF = letter;
}

//------------UART0_OutString------------
// Output String (NULL termination)
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
void UART0_OutString(char *pt){
  while(*pt){
    UART0_OutChar(*pt);
    pt++;
  }
}

//------------UART0_InUDec------------
// InUDec accepts ASCII input in unsigned decimal format
//     and converts to a 32-bit unsigned number
//     valid range is 0 to 4294967295 (2^32-1)
// Input: none
// Output: 32-bit unsigned number
// If you enter a number above 4294967295, it will return an incorrect value
// Backspace will remove last digit typed
uint32_t UART0_InUDec(void){
uint32_t number=0, length=0;
char character;
  character = UART0_InChar();
  while(character != CR){ // accepts until <enter> is typed
// The next line checks that the input is a digit, 0-9.
// If the character is not 0-9, it is ignored and not echoed
    if((character>='0') && (character<='9')) {
      number = 10*number+(character-'0');   // this line overflows if above 4294967295
      length++;
      UART0_OutChar(character);
    }
// If the input is a backspace, then the return number is
// changed and a backspace is outputted to the screen
    else if((character==BS) && length){
      number /= 10;
      length--;
      UART0_OutChar(character);
    }
    character = UART0_InChar();
  }
  return number;
}

//-----------------------UART0_OutUDec-----------------------
// Output a 32-bit number in unsigned decimal format
// Input: 32-bit number to be transferred
// Output: none
// Variable format 1-10 digits with no space before or after
void UART0_OutUDec(uint32_t n){
// This function uses recursion to convert decimal number
//   of unspecified length as an ASCII string
  if(n >= 10){
    UART0_OutUDec(n/10);
    n = n%10;
  }
  UART0_OutChar(n+'0'); /* n is between 0 and 9 */
}
uint32_t Messageindexb;
char Messageb[8];
void static fillmessageb(uint32_t n){
// This function uses recursion to convert decimal number
//   of unspecified length as an ASCII string
  if(n >= 10){
    fillmessageb(n/10);
    n = n%10;
  }
  Messageb[Messageindexb] = (n+'0'); /* n is between 0 and 9 */
  if(Messageindexb<7)Messageindexb++;
}
void static fillmessage4b(uint32_t n){
  if(n>=1000){  // 1000 to 9999
    Messageindexb = 0;
  } else if(n>=100){  // 100 to 999
    Messageb[0] = ' ';
    Messageindexb = 1;
  }else if(n>=10){ //
    Messageb[0] = ' '; /* n is between 10 and 99 */
    Messageb[1] = ' ';
    Messageindexb = 2;
  }else{
    Messageb[0] = ' '; /* n is between 0 and 9 */
    Messageb[1] = ' ';
    Messageb[2] = ' ';
    Messageindexb = 3;
  }
  fillmessageb(n);
}
void static fillmessage5b(uint32_t n){
  if(n>99999)n=99999;
  if(n>=10000){  // 10000 to 99999
    Messageindexb = 0;
  } else if(n>=1000){  // 1000 to 9999
    Messageb[0] = ' ';
    Messageindexb = 1;
  }else if(n>=100){  // 100 to 999
    Messageb[0] = ' ';
    Messageb[1] = ' ';
    Messageindexb = 2;
  }else if(n>=10){ //
    Messageb[0] = ' '; /* n is between 10 and 99 */
    Messageb[1] = ' ';
    Messageb[2] = ' ';
    Messageindexb = 3;
  }else{
    Messageb[0] = ' '; /* n is between 0 and 9 */
    Messageb[1] = ' ';
    Messageb[2] = ' ';
    Messageb[3] = ' ';
    Messageindexb = 4;
  }
  fillmessageb(n);
}
//-----------------------UART0_OutUDec4-----------------------
// Output a 32-bit number in unsigned decimal format
// Input: 32-bit number to be transferred
// Output: none
// Variable format 1-10 digits with no space before or after
void UART0_OutUDec4(uint32_t n){
  if(n>9999){
    UART0_OutString("****");
  }else{
    Messageindexb = 0;
    fillmessage4b(n);
    Messageb[Messageindexb] = 0; // terminate
    UART0_OutString(Messageb);
  }
}
//-----------------------UART0_OutUDec5-----------------------
// Output a 32-bit number in unsigned decimal format
// Input: 32-bit number to be transferred
// Output: none
// Variable format 1-10 digits with no space before or after
void UART0_OutUDec5(uint32_t n){
  if(n>99999){
    UART0_OutString("*****");
  }else{
    Messageindexb = 0;
    fillmessage5b(n);
    Messageb[Messageindexb] = 0; // terminate
    UART0_OutString(Messageb);
  }
}
//-----------------------UART0_OutUFix1-----------------------
// Output a 32-bit number in unsigned decimal format
// Input: 32-bit number to be transferred
// Output: none
// Variable format 1-10 digits with no space before or after
void UART0_OutUFix1(uint32_t n){
  UART0_OutUDec(n/10);
  UART0_OutChar('.');
  UART0_OutUDec(n%10);
//  UART0_OutChar('%');
}
//---------------------UART0_InUHex----------------------------------------
// Accepts ASCII input in unsigned hexadecimal (base 16) format
// Input: none
// Output: 32-bit unsigned number
// No '$' or '0x' need be entered, just the 1 to 8 hex digits
// It will convert lower case a-f to uppercase A-F
//     and converts to a 16 bit unsigned number
//     value range is 0 to FFFFFFFF
// If you enter a number above FFFFFFFF, it will return an incorrect value
// Backspace will remove last digit typed
uint32_t UART0_InUHex(void){
uint32_t number=0, digit, length=0;
char character;
  character = UART0_InChar();
  while(character != CR){
    digit = 0x10; // assume bad
    if((character>='0') && (character<='9')){
      digit = character-'0';
    }
    else if((character>='A') && (character<='F')){
      digit = (character-'A')+0xA;
    }
    else if((character>='a') && (character<='f')){
      digit = (character-'a')+0xA;
    }
// If the character is not 0-9 or A-F, it is ignored and not echoed
    if(digit <= 0xF){
      number = number*0x10+digit;
      length++;
      UART0_OutChar(character);
    }
// Backspace outputted and return value changed if a backspace is inputted
    else if((character==BS) && length){
      number /= 0x10;
      length--;
      UART0_OutChar(character);
    }
    character = UART0_InChar();
  }
  return number;
}

//--------------------------UART0_OutUHex----------------------------
// Output a 32-bit number in unsigned hexadecimal format
// Input: 32-bit number to be transferred
// Output: none
// Variable format 1 to 8 digits with no space before or after
void UART0_OutUHex(uint32_t number){
// This function uses recursion to convert the number of
//   unspecified length as an ASCII string
  if(number >= 0x10){
    UART0_OutUHex(number/0x10);
    UART0_OutUHex(number%0x10);
  }
  else{
    if(number < 0xA){
      UART0_OutChar(number+'0');
     }
    else{
      UART0_OutChar((number-0x0A)+'A');
    }
  }
}
//--------------------------UART0_OutUHex2----------------------------
// Output a 32-bit number in unsigned hexadecimal format
// Input: 32-bit number to be transferred
// Output: none
// Fixed format 2 digits with no space before or after
void outnibble(uint32_t n){
    if(n < 0xA){
   UART0_OutChar(n+'0');
  }
  else{
    UART0_OutChar((n-0x0A)+'A');
  }
}
void UART0_OutUHex2(uint32_t number){
  outnibble(number/0x10); // ms digit
  outnibble(number%0x10); // ls digit
}
//------------UART0_InString------------
// Accepts ASCII characters from the serial port
//    and adds them to a string until <enter> is typed
//    or until max length of the string is reached.
// It echoes each character as it is inputted.
// If a backspace is inputted, the string is modified
//    and the backspace is echoed
// terminates the string with a null character
// uses busy-waiting synchronization on RDRF
// Input: pointer to empty buffer, size of buffer
// Output: Null terminated string
// -- Modified by Agustinus Darmawan + Mingjie Qiu --
void UART0_InString(char *bufPt, uint16_t max) {
int length=0;
char character;
  character = UART0_InChar();
  while(character != CR){
    if(character == BS){
      if(length){
        bufPt--;
        length--;
        UART0_OutChar(BS);
      }
    }
    else if(length < max){
      *bufPt = character;
      bufPt++;
      length++;
      UART0_OutChar(character);
    }
    character = UART0_InChar();
  }
  *bufPt = 0;
}

