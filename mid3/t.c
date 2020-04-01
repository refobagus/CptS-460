extern void kbd_handler();
extern void kbd_init();
int color;

#include "type.h"
#include "string.c"
// #include "queue.c"  // use provided queue.obj  during linking
// #include "kbd.c"    // use provided kbd.obj    during linking
#include "vid.c"
#include "exceptions.c"
#include "kernel.c"
#include "uart.c"
#include "pipe.c"
#include "timer.c"

void copy_vectors(void) { // copy vector table in ts.s to 0x0
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
  int sicstatus = SIC_STATUS;
  // read VIC status registers to determine interrupt source
  int vicstatus = VIC_STATUS;
  // VIC status BITs: timer0,1=4, uart0=13, uart1=14
  if (vicstatus & (1 << 4)) {
    // bit4=1:timer0,1
    if (*(tp[0]->base + TVALUE) == 0) // timer 0
      timer_handler(0);
    if (*(tp[1]->base + TVALUE) == 0) // timer 1
      timer_handler(1);
  }
  if (vicstatus & (1 << 5)) {
    // bit5=1:timer2,3
    if (*(tp[2]->base + TVALUE) == 0)
      // timer 2
      timer_handler(2);
    if (*(tp[3]->base + TVALUE) == 0)
      // timer 3
      timer_handler(3);
  }

  // Keyboard bit
  if (vicstatus & 0x80000000) {
    if (sicstatus & 0x08) {
      kbd_handler();
    }
  }
}
int body();

int main()
{ 
  int i;
  color = WHITE; // int color in vid.c file
  fbuf_init(); // initialize LCD driver
  kbd_init();
  printf("\n"); // skip past top of screen display
  printf("main starts\n");
  /* enable VIC for timer interrupts */
  VIC_INTENABLE = 0;
  VIC_INTENABLE |= (1 << 4);  // timer0,1 at VIC.bit4
  VIC_INTENABLE |= (1 << 5);  // timer2,3 at VIC.bit5
  VIC_INTENABLE |= (1 << 31); // SIC to VIC's IRQ31

  /* enable KBD IRQ */
  SIC_ENSET = 1 << 3;    // KBD int=3 on SIC
  SIC_PICENSET = 1 << 3; // KBD int=3 on SIC

  timer_init();
  for (i = 0; i < 2; i++) { // start all 4 timers
    tp[i] = &timer[i];
    timer_start(i);
  }

  init();

  kfork((int)body, 1);
  kfork((int)body, 1);
  kfork((int)body, 1);
  kfork((int)body, 1);

  while (1) {
    if (readyQueue)
      tswitch();
  }
}
