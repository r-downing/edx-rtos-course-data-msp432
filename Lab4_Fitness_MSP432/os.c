// os.c
// Runs on LM4F120/TM4C123/MSP432
// A priority/blocking real-time operating system 
// Lab 4 starter file.
// Daniel Valvano
// September 13, 2016
// Hint: Copy solutions from Lab 3 into Lab 4

#include <stdint.h>
#include "os.h"
#include "CortexM.h"
#include "BSP.h"
#include "../inc/msp432p401r.h"

// function definitions in osasm.s
void StartOS(void);

#define NUMTHREADS  8        // maximum number of threads
#define NUMPERIODIC 2        // maximum number of periodic threads
#define STACKSIZE   100      // number of 32-bit words in stack per thread
struct tcb{
  int32_t *sp;       // pointer to stack (valid for threads not running
  struct tcb *next;  // linked-list pointer
	int32_t *blocked;   // nonzero if blocked on this semaphore
  int32_t sleep;// nonzero if this thread is sleeping
	uint8_t Priority;
//*FILL THIS IN****
};
typedef struct tcb tcbType;
tcbType tcbs[NUMTHREADS];
tcbType *RunPt;
int32_t Stacks[NUMTHREADS][STACKSIZE];
void static runperiodicevents(void);

// ******** OS_Init ************
// Initialize operating system, disable interrupts
// Initialize OS controlled I/O: periodic interrupt, bus clock as fast as possible
// Initialize OS global variables
// Inputs:  none
// Outputs: none
void OS_Init(void){
  DisableInterrupts();
  BSP_Clock_InitFastest();// set processor clock to fastest speed

// perform any initializations needed, 
// set up periodic timer to run runperiodicevents to implement sleeping
	BSP_PeriodicTask_Init(&runperiodicevents, 1000, 2);

}

void SetInitialStack(int i){
  // ****IMPLEMENT THIS**** 
  // **Same as Lab 2 and Lab 3****
  tcbs[i].sp = &Stacks[i][STACKSIZE-16]; // thread stack pointer
  Stacks[i][STACKSIZE-1] = 0x01000000;   // thumb bit
	//debugging values
  Stacks[i][STACKSIZE-3] = 0x14141414;   // R14
  Stacks[i][STACKSIZE-4] = 0x12121212;   // R12
  Stacks[i][STACKSIZE-5] = 0x03030303;   // R3
  Stacks[i][STACKSIZE-6] = 0x02020202;   // R2
  Stacks[i][STACKSIZE-7] = 0x01010101;   // R1
  Stacks[i][STACKSIZE-8] = 0x00000000;   // R0
  Stacks[i][STACKSIZE-9] = 0x11111111;   // R11
  Stacks[i][STACKSIZE-10] = 0x10101010;  // R10
  Stacks[i][STACKSIZE-11] = 0x09090909;  // R9
  Stacks[i][STACKSIZE-12] = 0x08080808;  // R8
  Stacks[i][STACKSIZE-13] = 0x07070707;  // R7
  Stacks[i][STACKSIZE-14] = 0x06060606;  // R6
  Stacks[i][STACKSIZE-15] = 0x05050505;  // R5
  Stacks[i][STACKSIZE-16] = 0x04040404;  // R4
}

//******** OS_AddThreads ***************
// Add eight main threads to the scheduler
// Inputs: function pointers to eight void/void main threads
//         priorites for each main thread (0 highest)
// Outputs: 1 if successful, 0 if this thread can not be added
// This function will only be called once, after OS_Init and before OS_Launch
int OS_AddThreads(void(*thread0)(void), uint32_t p0,
                  void(*thread1)(void), uint32_t p1,
                  void(*thread2)(void), uint32_t p2,
                  void(*thread3)(void), uint32_t p3,
                  void(*thread4)(void), uint32_t p4,
                  void(*thread5)(void), uint32_t p5,
                  void(*thread6)(void), uint32_t p6,
                  void(*thread7)(void), uint32_t p7){
// **similar to Lab 3. initialize priority field****
	int32_t status = StartCritical();
  tcbs[0].next = &tcbs[1]; // 0 points to 1
  tcbs[1].next = &tcbs[2]; // 1 points to 2
  tcbs[2].next = &tcbs[3]; // 2 points to 3
	tcbs[3].next = &tcbs[4]; // 3 points to 0
	tcbs[4].next = &tcbs[5]; // 3 points to 0
	tcbs[5].next = &tcbs[6]; // 3 points to 0
	tcbs[6].next = &tcbs[7]; // 3 points to 0
	tcbs[7].next = &tcbs[0]; // 3 points to 0
	tcbs[0].Priority = p0;
	tcbs[1].Priority = p1;
	tcbs[2].Priority = p2;
	tcbs[3].Priority = p3;
	tcbs[4].Priority = p4;
	tcbs[5].Priority = p5;
	tcbs[6].Priority = p6;
	tcbs[7].Priority = p7;
										
										
	for(int i = 0; i< NUMTHREADS; i++){
		tcbs[i].blocked = 0;
		tcbs[i].sleep = 0;
	}
	SetInitialStack(0); Stacks[0][STACKSIZE-2] = (int32_t)(thread0); // PC
  SetInitialStack(1); Stacks[1][STACKSIZE-2] = (int32_t)(thread1); // PC
  SetInitialStack(2); Stacks[2][STACKSIZE-2] = (int32_t)(thread2); // PC
  SetInitialStack(3); Stacks[3][STACKSIZE-2] = (int32_t)(thread3); // PC
  SetInitialStack(4); Stacks[4][STACKSIZE-2] = (int32_t)(thread4); // PC
  SetInitialStack(5); Stacks[5][STACKSIZE-2] = (int32_t)(thread5); // PC
  SetInitialStack(6); Stacks[6][STACKSIZE-2] = (int32_t)(thread6); // PC
  SetInitialStack(7); Stacks[7][STACKSIZE-2] = (int32_t)(thread7); // PC
  RunPt = &tcbs[0];       // thread 0 will run first
	EndCritical(status);
  return 1;               // successful
}


void static runperiodicevents(void){
// ****IMPLEMENT THIS****
// **DECREMENT SLEEP COUNTERS
// In Lab 4, handle periodic events in RealTimeEvents
	for(int i = 0; i < NUMTHREADS; i++){
		if(tcbs[i].sleep) {
			tcbs[i].sleep--;
		}
	}
}

//******** OS_Launch ***************
// Start the scheduler, enable interrupts
// Inputs: number of clock cycles for each time slice
// Outputs: none (does not return)
// Errors: theTimeSlice must be less than 16,777,216
void OS_Launch(uint32_t theTimeSlice){
  STCTRL = 0;                  // disable SysTick during setup
  STCURRENT = 0;               // any write to current clears it
  SYSPRI3 =(SYSPRI3&0x00FFFFFF)|0xE0000000; // priority 7
  STRELOAD = theTimeSlice - 1; // reload value
  STCTRL = 0x00000007;         // enable, core clock and interrupt arm
  StartOS();                   // start on the first task
}
// runs every ms
void Scheduler(void){      // every time slice
// ****IMPLEMENT THIS****
// look at all threads in TCB list choose
// highest priority thread not blocked and not sleeping 
// If there are multiple highest priority (not blocked, not sleeping) run these round robin
  uint32_t max = 255; // max
  tcbType *pt;
  tcbType *bestPt;
  pt = RunPt;         // search for highest thread not blocked or sleeping
  do{
    pt = pt->next;    // skips at least one
    if((pt->Priority < max)&&((pt->blocked)==0)&&((pt->sleep)==0)){
      max = pt->Priority;
      bestPt = pt;
    }
  } while(RunPt != pt); // look at all possible threads
  RunPt = bestPt; 
}

//******** OS_Suspend ***************
// Called by main thread to cooperatively suspend operation
// Inputs: none
// Outputs: none
// Will be run again depending on sleep/block status
void OS_Suspend(void){
  STCURRENT = 0;        // any write to current clears it
  INTCTRL = 0x04000000; // trigger SysTick
// next thread gets a full time slice
}

// ******** OS_Sleep ************
// place this thread into a dormant state
// input:  number of msec to sleep
// output: none
// OS_Sleep(0) implements cooperative multitasking
void OS_Sleep(uint32_t sleepTime){
// ****IMPLEMENT THIS****
// set sleep parameter in TCB, same as Lab 3
// suspend, stops running
	RunPt->sleep = sleepTime;
	OS_Suspend();
}

// ******** OS_InitSemaphore ************
// Initialize counting semaphore
// Inputs:  pointer to a semaphore
//          initial value of semaphore
// Outputs: none
void OS_InitSemaphore(int32_t *semaPt, int32_t value){
// ****IMPLEMENT THIS****
// Same as Lab 3
		(*semaPt) = value;

}

// ******** OS_Wait ************
// Decrement semaphore and block if less than zero
// Lab2 spinlock (does not suspend while spinning)
// Lab3 block if less than zero
// Inputs:  pointer to a counting semaphore
// Outputs: none
void OS_Wait(int32_t *semaPt){
// ****IMPLEMENT THIS****
// Same as Lab 3
	DisableInterrupts();
  (*semaPt) = (*semaPt) - 1;
  if((*semaPt) < 0){
    RunPt->blocked = semaPt; // reason it is blocked
    EnableInterrupts();
    OS_Suspend();       // run thread switcher
  }
  EnableInterrupts();
}

// ******** OS_Signal ************
// Increment semaphore
// Lab2 spinlock
// Lab3 wakeup blocked thread if appropriate
// Inputs:  pointer to a counting semaphore
// Outputs: none
void OS_Signal(int32_t *semaPt){
// ****IMPLEMENT THIS****
// Same as Lab 3
	tcbType *pt;
  DisableInterrupts();
  (*semaPt) = (*semaPt) + 1;
  if((*semaPt) <= 0){
    pt = RunPt->next;   // search for a thread blocked on this semaphore
    while(pt->blocked != semaPt){
      pt = pt->next;
    }
    pt->blocked = 0;    // wakeup this one
  }
  EnableInterrupts();
}

#define FSIZE 10    // can be any size
uint32_t PutI;      // index of where to put next
uint32_t GetI;      // index of where to get next
uint32_t Fifo[FSIZE];
int32_t CurrentSize;// 0 means FIFO empty, FSIZE means full
uint32_t LostData;  // number of lost pieces of data

// ******** OS_FIFO_Init ************
// Initialize FIFO.  The "put" and "get" indices initially
// are equal, which means that the FIFO is empty.  Also
// initialize semaphores to track properties of the FIFO
// such as size and busy status for Put and Get operations,
// which is important if there are multiple data producers
// or multiple data consumers.
// Inputs:  none
// Outputs: none
void OS_FIFO_Init(void){
// ****IMPLEMENT THIS****
// Same as Lab 3
	PutI = GetI = 0;
	OS_InitSemaphore(&CurrentSize, 0);
	LostData = 0;
}

// ******** OS_FIFO_Put ************
// Put an entry in the FIFO.  Consider using a unique
// semaphore to wait on busy status if more than one thread
// is putting data into the FIFO and there is a chance that
// this function may interrupt itself.
// Inputs:  data to be stored
// Outputs: 0 if successful, -1 if the FIFO is full
int OS_FIFO_Put(uint32_t data){
// ****IMPLEMENT THIS****
// Same as Lab 3
	if(CurrentSize == FSIZE) {
		LostData++;
		return -1;
	}
	Fifo[PutI] = data;
	PutI = (PutI+1)%FSIZE;
	OS_Signal(&CurrentSize);
  return 0;   // success
}

// ******** OS_FIFO_Get ************
// Get an entry from the FIFO.  Consider using a unique
// semaphore to wait on busy status if more than one thread
// is getting data from the FIFO and there is a chance that
// this function may interrupt itself.
// Inputs:  none
// Outputs: data retrieved
uint32_t OS_FIFO_Get(void){uint32_t data;
// ****IMPLEMENT THIS****
// Same as Lab 3
  OS_Wait(&CurrentSize);
	data = Fifo[GetI];
	GetI = (GetI+1)%FSIZE;
  return data;   // success
}
// *****periodic events****************
int32_t *PeriodicSemaphore0;
uint32_t Period0; // time between signals
int32_t *PeriodicSemaphore1;
uint32_t Period1; // time between signals
void RealTimeEvents(void){
  int flag=0;
  static int32_t realCount = -10; // let all the threads execute once
  // Note to students: we had to let the system run for a time so all user threads ran at least one
  // before signalling the periodic tasks
  realCount++;
  if(realCount >= 0){
		if((realCount%Period0)==0){
      OS_Signal(PeriodicSemaphore0);
      flag = 1;
		}
    if((realCount%Period1)==0){
      OS_Signal(PeriodicSemaphore1);
      flag=1;
		}
    if(flag){
      OS_Suspend(); // run the scheduler
    }
  }
}
// ******** OS_PeriodTrigger0_Init ************
// Initialize periodic timer interrupt to signal 
// Inputs:  semaphore to signal
//          period in ms
// priority level at 0 (highest)
// Outputs: none
void OS_PeriodTrigger0_Init(int32_t *semaPt, uint32_t period){
	PeriodicSemaphore0 = semaPt;
	Period0 = period;
	BSP_PeriodicTask_InitC(&RealTimeEvents,1000,0);
}
// ******** OS_PeriodTrigger1_Init ************
// Initialize periodic timer interrupt to signal 
// Inputs:  semaphore to signal
//          period in ms
// priority level at 0 (highest)
// Outputs: none
void OS_PeriodTrigger1_Init(int32_t *semaPt, uint32_t period){
	PeriodicSemaphore1 = semaPt;
	Period1 = period;
	BSP_PeriodicTask_InitC(&RealTimeEvents,1000,0);
}

//****edge-triggered event************
int32_t *edgeSemaphore;
// ******** OS_EdgeTrigger_Init ************
// Initialize button1, P5.1, to signal on a falling edge interrupt
// Inputs:  semaphore to signal
//          priority
// Outputs: none
void OS_EdgeTrigger_Init(int32_t *semaPt, uint8_t priority){
	edgeSemaphore = semaPt;
//***IMPLEMENT THIS***
  // P5.1 input with pullup
	
	uint64_t pri64 = priority;
	pri64 = pri64 << 29;
	
	OS_InitSemaphore(edgeSemaphore, 0);
	
	/*
	P5SEL1 &= ~0x2;
	P5SEL0 &= ~0x2;
	P5DIR &= ~0x2;
	P5REN |= 0x2;
	P5OUT |= 0x2;
	P5IES |= 0x2; // (c) P5.1 is falling edge event
	
	P5IFG &= ~0x2; // (d) clear flag1 
	P1IE |= 0x2;   // (e) arm interrupt on P5.1
	NVIC_IPR9 = (NVIC_IPR9)|(pri64 << 29);
	NVIC_ISER1 &= 0x40;
  */
	
	//*
  P1SEL1 &= ~0x2;              // (b) configure P1.1, P1.4 as GPIO
  P1SEL0 &= ~0x2;              //     built-in Buttons 1 and 2
  P1DIR &= ~0x2;               //     make P1.1, P1.4 in
  P1REN |= 0x2;                //     enable pull resistors
  P1OUT |= 0x2;                //     P1.1, P1.4 is pull-up
  P1IES |= 0x2;                // (c) P1.1, P1.4 is falling edge event
  P1IFG &= ~0x2;               // (d) clear flag1 and flag4
  P1IE |= 0x2;                 // (e) arm interrupt on P1.1, P1.4
  NVIC_IPR8 = (NVIC_IPR8&0x00FFFFFF)|0x40000000; // (f) priority 2
  NVIC_ISER1 = 0x00000008;      // (g) enable interrupt 35 in NVIC
	//*/
	
  // (f) priority 
  // (g) enable interrupt 39 in NVIC
}

// ******** OS_EdgeTrigger_Restart ************
// restart button1 to signal on a falling edge interrupt
// rearm interrupt
// Inputs:  none
// Outputs: none
void OS_EdgeTrigger_Restart(void){
//***IMPLEMENT THIS***
	
	
	P1IE |= 0x2;
	NVIC_ISER1 = 0x00000008;
	P1IFG &= ~0x2;
	
	
	/*
	P5IE |= 0x2;
	NVIC_ISER1 &= 0x40;
	P5IFG &= ~0x2;
	*/
	
	/*
		P5IE |= 0x2;   // (e) arm interrupt on P5.1
  // (g) enable interrupt 39 in NVIC
	NVIC_ISER1 &= (1<<7);

  // (d) clear flag1 
	P5IFG &= ~0x2;
	*/

}
void PORT1_IRQHandler(void){
//***IMPLEMENT THIS***
  // step 1 acknowledge by clearing flag
	/*
	uint16_t status = P5IV;
		P5IFG &= ~0x2;
*/
	uint16_t status = P1IV;
	//uint16_t status = P5IV;
  // step 2 signal semaphore (no need to run scheduler)
	OS_Signal(edgeSemaphore);
  // step 3 disarm interrupt to prevent bouncing to create multiple signals
	//P5IE &= ~0x2;
	P1IE &= ~0x2;
}

