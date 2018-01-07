// Profile.h
// Runs on MSP432
// Implement the profiling interface on the MSP432.  This
// header file is very hardware-specific because it defines the
// Profile pin set, clear, and toggle functions as macros.
// Macros have the advantage of potentially being faster.
// Daniel and Jonathan Valvano
// June 12, 2016

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
// J4.35/P6.7 Profile 6
// If it turns out that these pins are needed by another
// BoosterPack, such as the Bluetooth one, then the
// following definitions will need to be modified.
#define PROFILE0  (*((volatile uint8_t *)(0x42000000+32*0x4C23+4*5)))  /* Port 4.5 Output */
#define PROFILE0_BIT 0x01
#define PROFILE1  (*((volatile uint8_t *)(0x42000000+32*0x4C23+4*7)))  /* Port 4.7 Output */
#define PROFILE1_BIT 0x01
#define PROFILE2  (*((volatile uint8_t *)(0x42000000+32*0x4C42+4*4)))  /* Port 5.4 Output */
#define PROFILE2_BIT 0x01
#define PROFILE3  (*((volatile uint8_t *)(0x42000000+32*0x4C42+4*5)))  /* Port 5.5 Output */
#define PROFILE3_BIT 0x01
#define PROFILE4  (*((volatile uint8_t *)(0x42000000+32*0x4C22+4*0)))  /* Port 3.0 Output */
#define PROFILE4_BIT 0x01
#define PROFILE5  (*((volatile uint8_t *)(0x42000000+32*0x4C43+4*6)))  /* Port 6.6 Output */
#define PROFILE5_BIT 0x01
#define PROFILE6  (*((volatile uint8_t *)(0x42000000+32*0x4C43+4*7)))  /* Port 6.7 Output */
#define PROFILE6_BIT 0x01

// ------------Profile_Init------------
// Initialize all Profile pins for GPIO output,
// which correspond with unused BoosterPack pins.
// The Profile pins can be set, cleared, or toggled at
// strategic times and observed with a logic analyzer.
// This function should be called once at the beginning.
// Input: none
// Output: none
void Profile_Init(void);

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
uint8_t Profile_Get(void);

// ------------Profile_Set0------------
// Set the first Profile pin to 1.  This function is
// thread safe.  Uses bit-specific addressing and
// macros to improve speed and interrupt safety.
// Requires about 5 bus cycles to complete.
// Input: none
// Output: none
// Assumes: Profile_Init() has been called
#define Profile_Set0() (PROFILE0 = PROFILE0_BIT)

// ------------Profile_Clear0------------
// Clear the first Profile pin to 0.  This function
// is thread safe.  Uses bit-specific addressing and
// macros to improve speed and interrupt safety.
// Requires about 5 bus cycles to complete.
// Input: none
// Output: none
// Assumes: Profile_Init() has been called
#define Profile_Clear0() (PROFILE0 = 0x00)

// ------------Profile_Toggle0------------
// Toggle the first Profile pin.  This function is
// thread safe.  Uses bit-specific addressing and
// macros to improve speed and interrupt safety.
// Requires about 6 bus cycles to complete.
// Input: none
// Output: none
// Assumes: Profile_Init() has been called
#define Profile_Toggle0() (PROFILE0 ^= PROFILE0_BIT)

// ------------Profile_Set1------------
// Set the second Profile pin to 1.  This function is
// thread safe.  Uses bit-specific addressing and
// macros to improve speed and interrupt safety.
// Requires about 5 bus cycles to complete.
// Input: none
// Output: none
// Assumes: Profile_Init() has been called
#define Profile_Set1() (PROFILE1 = PROFILE1_BIT)

// ------------Profile_Clear1------------
// Clear the second Profile pin to 0.  This function
// is thread safe.  Uses bit-specific addressing and
// macros to improve speed and interrupt safety.
// Requires about 5 bus cycles to complete.
// Input: none
// Output: none
// Assumes: Profile_Init() has been called
#define Profile_Clear1() (PROFILE1 = 0x00)

// ------------Profile_Toggle1------------
// Toggle the second Profile pin.  This function is
// thread safe.  Uses bit-specific addressing and
// macros to improve speed and interrupt safety.
// Requires about 6 bus cycles to complete.
// Input: none
// Output: none
// Assumes: Profile_Init() has been called
#define Profile_Toggle1() (PROFILE1 ^= PROFILE1_BIT)

// ------------Profile_Set2------------
// Set the third Profile pin to 1.  This function is
// thread safe.  Uses bit-specific addressing and
// macros to improve speed and interrupt safety.
// Requires about 5 bus cycles to complete.
// Input: none
// Output: none
// Assumes: Profile_Init() has been called
#define Profile_Set2() (PROFILE2 = PROFILE2_BIT)

// ------------Profile_Clear2------------
// Clear the third Profile pin to 0.  This function
// is thread safe.  Uses bit-specific addressing and
// macros to improve speed and interrupt safety.
// Requires about 5 bus cycles to complete.
// Input: none
// Output: none
// Assumes: Profile_Init() has been called
#define Profile_Clear2() (PROFILE2 = 0x00)

// ------------Profile_Toggle2------------
// Toggle the third Profile pin.  This function is
// thread safe.  Uses bit-specific addressing and
// macros to improve speed and interrupt safety.
// Requires about 6 bus cycles to complete.
// Input: none
// Output: none
// Assumes: Profile_Init() has been called
#define Profile_Toggle2() (PROFILE2 ^= PROFILE2_BIT)

// ------------Profile_Set3------------
// Set the fourth Profile pin to 1.  This function is
// thread safe.  Uses bit-specific addressing and
// macros to improve speed and interrupt safety.
// Requires about 5 bus cycles to complete.
// Input: none
// Output: none
// Assumes: Profile_Init() has been called
#define Profile_Set3() (PROFILE3 = PROFILE3_BIT)

// ------------Profile_Clear3------------
// Clear the fourth Profile pin to 0.  This function
// is thread safe.  Uses bit-specific addressing and
// macros to improve speed and interrupt safety.
// Requires about 5 bus cycles to complete.
// Input: none
// Output: none
// Assumes: Profile_Init() has been called
#define Profile_Clear3() (PROFILE3 = 0x00)

// ------------Profile_Toggle3------------
// Toggle the fourth Profile pin.  This function is
// thread safe.  Uses bit-specific addressing and
// macros to improve speed and interrupt safety.
// Requires about 6 bus cycles to complete.
// Input: none
// Output: none
// Assumes: Profile_Init() has been called
#define Profile_Toggle3() (PROFILE3 ^= PROFILE3_BIT)

// ------------Profile_Set4------------
// Set the fifth Profile pin to 1.  This function is
// thread safe.  Uses bit-specific addressing and
// macros to improve speed and interrupt safety.
// Requires about 5 bus cycles to complete.
// Input: none
// Output: none
// Assumes: Profile_Init() has been called
#define Profile_Set4() (PROFILE4 = PROFILE4_BIT)

// ------------Profile_Clear4------------
// Clear the fifth Profile pin to 0.  This function
// is thread safe.  Uses bit-specific addressing and
// macros to improve speed and interrupt safety.
// Requires about 5 bus cycles to complete.
// Input: none
// Output: none
// Assumes: Profile_Init() has been called
#define Profile_Clear4() (PROFILE4 = 0x00)

// ------------Profile_Toggle4------------
// Toggle the fifth Profile pin.  This function is
// thread safe.  Uses bit-specific addressing and
// macros to improve speed and interrupt safety.
// Requires about 6 bus cycles to complete.
// Input: none
// Output: none
// Assumes: Profile_Init() has been called
#define Profile_Toggle4() (PROFILE4 ^= PROFILE4_BIT)

// ------------Profile_Set5------------
// Set the sixth Profile pin to 1.  This function is
// thread safe.  Uses bit-specific addressing and
// macros to improve speed and interrupt safety.
// Requires about 5 bus cycles to complete.
// Input: none
// Output: none
// Assumes: Profile_Init() has been called
#define Profile_Set5() (PROFILE5 = PROFILE5_BIT)

// ------------Profile_Clear5------------
// Clear the sixth Profile pin to 0.  This function
// is thread safe.  Uses bit-specific addressing and
// macros to improve speed and interrupt safety.
// Requires about 5 bus cycles to complete.
// Input: none
// Output: none
// Assumes: Profile_Init() has been called
#define Profile_Clear5() (PROFILE5 = 0x00)

// ------------Profile_Toggle5------------
// Toggle the sixth Profile pin.  This function is
// thread safe.  Uses bit-specific addressing and
// macros to improve speed and interrupt safety.
// Requires about 6 bus cycles to complete.
// Input: none
// Output: none
// Assumes: Profile_Init() has been called
#define Profile_Toggle5() (PROFILE5 ^= PROFILE5_BIT)

// ------------Profile_Set6------------
// Set the seventh Profile pin to 1.  This function
// is thread safe.  Uses bit-specific addressing and
// macros to improve speed and interrupt safety.
// Requires about 5 bus cycles to complete.
// Input: none
// Output: none
// Assumes: Profile_Init() has been called
#define Profile_Set6() (PROFILE6 = PROFILE6_BIT)

// ------------Profile_Clear6------------
// Clear the seventh Profile pin to 0.  This function
// is thread safe.  Uses bit-specific addressing and
// macros to improve speed and interrupt safety.
// Requires about 5 bus cycles to complete.
// Input: none
// Output: none
// Assumes: Profile_Init() has been called
#define Profile_Clear6() (PROFILE6 = 0x00)

// ------------Profile_Toggle6------------
// Toggle the seventh Profile pin.  This function is
// thread safe.  Uses bit-specific addressing and
// macros to improve speed and interrupt safety.
// Requires about 6 bus cycles to complete.
// Input: none
// Output: none
// Assumes: Profile_Init() has been called
#define Profile_Toggle6() (PROFILE6 ^= PROFILE6_BIT)
