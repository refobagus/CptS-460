#include "defines.h" // LCD, TIMER and UART addresses
#include "string.c" // strcmp, strlen, etc
#include "vid.c" // LCD driver file
#include "timer.c" // timer handler file
#include "exceptions.c" // other exception handlers

void copy_vectors(void){// copy vector table in ts.s to 0x0
  extern u32 vectors_start, vectors_end;
  u32 *vectors_src = &vectors_start;
  u32 *vectors_dst = (u32 *)0;
  while (vectors_src < &vectors_end)
    *vectors_dst++ = *vectors_src++;
}

void timer_handler();
TIMER *tp[4]; // 4 TIMER structure pointers

void IRQ_handler() // IRQ interrupt handler in C
{
  // read VIC status registers to determine interrupt source
  int vicstatus = VIC_STATUS;
  // VIC status BITs: timer0,1=4, uart0=13, uart1=14
  if (vicstatus & (1<<4)){
    // bit4=1:timer0,1
    if (*(tp[0]->base+TVALUE)==0) // timer 0
      timer_handler(0);
    if (*(tp[1]->base+TVALUE)==0) // timer 1
      timer_handler(1);
  }
  if (vicstatus & (1<<5)){
    // bit5=1:timer2,3
    if (*(tp[2]->base+TVALUE)==0)
      // timer 2
      timer_handler(2);
    if (*(tp[3]->base+TVALUE)==0)
      // timer 3
      timer_handler(3);
  }
}

int main()
{
  int i;
  color = RED; // int color in vid.c file
  fbuf_init(); // initialize LCD driver
  printf("main starts\n");
  /* enable VIC for timer interrupts */
  VIC_INTENABLE = 0;
  VIC_INTENABLE |= (1<<4); // timer0,1 at VIC.bit4
  VIC_INTENABLE |= (1<<5);
  timer_init(); // timer2,3 at VIC.bit5
  for (i=0; i<4; i++){ // start all 4 timers
    tp[i] = &timer[i];
    timer_start(i);
  }
  printf("Enter while(1) loop, handle timer interrupts\n");
}
