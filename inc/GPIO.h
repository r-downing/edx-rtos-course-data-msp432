// GPIO.h
// Runs on MSP432
// Digital input/output from LaunchPad to CC2650
// Jonathan Valvano
// September 10, 2016
#include "msp432p401r.h"

//#define DEFAULT 1
// Legend    TI part number
// CC2650BP  BOOSTXL-CC2650MA
// CC2650LP  LAUNCHXL-CC2650
// MSP432    MSP-EXP432P401R
// TM4C123   EK-TM4C123GXL
// MKII      BOOSTXL-EDUMKII

#ifdef DEFAULT
// Use this setup with CC2650BP without an MKII 
// Two board stack: CC2650BP+MSP432 
// Acceptable projects:
//     VerySimpleApplicationProcessor_MSP432
//     ApplicationProcessor_MSP432
// This runs with the default version of SNP that ships on the BOOSTXL-CC2650MA
// signal  MSP432                           CC2650BP       comment
//  3V3    J1.1  3.3V                       J1.1  3.3V     Power from MSP432 to CC2650BP 
//  GND    J2.20 ground                     J2.20 ground   Connect ground together
//  NRESET J4.35 TM4C123 PC6, MSP432 P6.7   J4.35          Reset from MSP432 to CC2650BP  
//  TX     J1.3  TM4C123 PB0, MSP432 P3.2   J1.3  DIO3_TXD UART from CC2650BP to MSP432  
//  RX     J1.4  TM4C123 PB1, MSP432 P3.3   J1.4  DIO2_RXD UART from MSP432 to CC2650BP
//  MRDY   J1.2  TM4C123 PB5, MSP432 P6.0   J1.2  IOID_7   Master ready from MSP432 to CC2650BP 
//  SRDY   J2.19 TM4C123 PB2, MSP432 P2.5   J2.19 IOID_8   Slave ready from CC2650BP to MSP432 

#define SetMRDY() (P6OUT |= 0x01)
#define ClearMRDY() (P6OUT &= ~0x01)
#define SetReset() (P6OUT |= 0x80)
#define ClearReset() (P6OUT &= ~0x80)
#define ReadSRDY() (P2IN&0x20)
#else
// This setup requires reprogramming the CC2650LP/CC2650BP or using a 7-wire tether
// Option 1) The CC2650BP is tethered to the MSP432 using 7 wires (no reprogramming CC2650 needed)
// Two board stack: MSP432+MKII  <---7 wires--->  CC2650BP
// signal  MSP432                           CC2650BP       comment
//  3V3    J1.1  3.3V                       J1.1  3.3V     Power from MSP432 to CC2650BP 
//  GND    J2.20 ground                     J2.20 ground   Connect ground together
//  NRESET J4.35 TM4C123 PC6, MSP432 P6.7   J4.35          Reset from MSP432 to CC2650BP  
//  TX     J1.3  TM4C123 PB0, MSP432 P3.2   J1.3  DIO3_TXD UART from CC2650BP to MSP432  
//  RX     J1.4  TM4C123 PB1, MSP432 P3.3   J1.4  DIO2_RXD UART from MSP432 to CC2650BP
//  MRDY   J2.14 TM4C123 PB6, MSP432 P1.7   J1.2  IOID_7   Master ready from MSP432 to CC2650BP 
//  SRDY   J2.12 TM4C123 PA3, MSP432 P5.2}  J2.19 IOID_8   Slave ready from CC2650BP to MSP432 

// Option 2) This version also works with a custom version of the SNP loaded onto the CC2650BP
// Program into CC2650BP: simple_np_cc2650bp_uart_pm_xsbl_mooc_custom.hex
// Three board stack: CC2650BP+MSP432+MKII (BOOSTXL-CC2650MA, MSP-EXP432P401R, and BOOSTXL-EDUMKII)
// signal  MSP432                           CC2650LP       comment
//  3V3    J1.1  3.3V                       J1.1  3.3V     Power from MSP432 to CC2650BP 
//  GND    J2.20 ground                     J2.20 ground   Connect ground together
//  NRESET J4.35 TM4C123 PC6, MSP432 P6.7   J4.35          Reset from MSP432 to CC2650BP  
//  TX     J1.3  TM4C123 PB0, MSP432 P3.2   J1.3  DIO3_TXD UART from CC2650BP to MSP432  
//  RX     J1.4  TM4C123 PB1, MSP432 P3.3   J1.4  DIO2_RXD UART from MSP432 to CC2650BP
//  MRDY   J2.14 TM4C123 PB6, MSP432 P1.7   J1.14 DIO8     Master ready from MSP432 to CC2650BP 
//  SRDY   J2.12 TM4C123 PA3, MSP432 P5.2}  J2.12 DIO14    Slave ready from CC2650BP to MSP432 

// Option 3) This version also works with a custom version of the SNP loaded onto the CC2650LP
// Program into CC2650LP: simple_np_cc2650lp_uart_pm_xsbl_mooc_custom.hex
// Remove Rx and Tx jumpers on CC2650LP
// Optional: remove two LED jumpers on Red, Green LEDs
// Place insulting tape over the set of 11 jumpers in middle, before stacking
// Three board stack: CC2650LP+MSP432+MKII 
// signal  MSP432                           CC2650LP       comment
//  3V3    J1.1  3.3V                       J1.1  3.3V     Power from MSP432 to CC2650LP 
//  GND    J2.20 ground                     J2.20 ground   Connect ground together
//  NRESET J4.35 TM4C123 PC6, MSP432 P6.7   J4.35          Reset from MSP432 to CC2650LP  
//  TX     J1.3  TM4C123 PB0, MSP432 P3.2   J1.3  DIO3_TXD UART from CC2650LP to MSP432  
//  RX     J1.4  TM4C123 PB1, MSP432 P3.3   J1.4  DIO2_RXD UART from MSP432 to CC2650LP
//  MRDY   J2.14 TM4C123 PB6, MSP432 P1.7   J1.14 DIO12    Master ready from MSP432 to CC2650LP 
//  SRDY   J2.12 TM4C123 PA3, MSP432 P5.2}  J2.12 DIO14    Slave ready from CC2650LP to MSP432 

#define SetMRDY() (P1OUT |= 0x80)
#define ClearMRDY() (P1OUT &= ~0x80)
#define SetReset() (P6OUT |= 0x80)
#define ClearReset() (P6OUT &= ~0x80)
#define ReadSRDY() (P5IN&0x04)
#endif


//------------GPIO_Init------------
// Initialize MRDY (out), SRDY (in), RESET (out) GPIO pins
// Input: none
// Output: none
void GPIO_Init(void);
