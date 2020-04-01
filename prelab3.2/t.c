/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

#include "defines.h"
#include "string.c"

char *tab = "0123456789ABCDEF";
int color;

#include "vid.c"
#include "interrupts.c"
#include "kbd.c"

void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}
int kprintf(char *fmt, ...);
void timer_handler();

void IRQ_handler()
{
    int vicstatus, sicstatus;
    int ustatus, kstatus;

    // read VIC SIV status registers to find out which interrupt
    vicstatus = VIC_STATUS;
    sicstatus = SIC_STATUS;  

    if (vicstatus & (1<<31)){
      if (sicstatus & (1<<3)){
          kbd_handler();
       }
    }
}

int main()
{
  int i;
  char line[128];
  color = RED; // int color in vid.c file
  fbuf_init(); // initialize LCD display
  /* enable VIC interrupts: timer0 at IRQ3, SIC at IRQ31 */
  VIC_INTENABLE
    = 0;
  VIC_INTENABLE |= (1<<4); // timer0,1 at PIC.bit4
  VIC_INTENABLE |= (1<<5);
  VIC_INTENABLE |= (1<<31); // timer2,3 at PIC.bit5
  // SIC to PIC.bit31
  /* enable KBD IRQ on SIC */
  SIC_INTENABLE = 0;
  SIC_INTENABLE |= (1<<3);
  kbd_init(); // initialize keyboard driver
  printf("C3.2 start: test KBD and TIMER drivers\n");
  while(1){
    color = CYAN;
    printf("Enter a line from KBD\n");
    kgets(line);
    printf("line = %s\n", line);
  }
}
