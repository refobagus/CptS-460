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

void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}
int kprintf(char *fmt, ...);

void IRQ_handler()
{
    int vicstatus, sicstatus;
    int ustatus, kstatus;

    // read VIC status register to find out which interrupt
    vicstatus = VIC_STATUS; // VIC_STATUS=0x10140000=status reg
    sicstatus = SIC_STATUS;  
    if (vicstatus & 0x80000000){
       if (sicstatus & 0x08){
          kbd_handler();
       }
    }
}

int body();

int pipe_writer(){ // pipe writer task code
  struct uart *up = &uart[0];
  char line[128];
  while(1){
    tswitch();
    uprintf(uart, "Enter a line for task1 to get : ");
    printf("task%d waits for line from UART0\n", running->pid);
    // gets the line from terminal
    ugets(up, line);
    uprints(up, "\r\n");
    printf(uart, "task%d writes line=[%s] to pipe\n", running->pid, line);
    tswitch();
    write_pipe(kpipe, line, strlen(line));
  }
}

int pipe_reader(){ // pipe reader task code
  char line[128];
  int i, n;
  while(1){
    printf("task%d reading from pipe\n", running->pid);
    n = read_pipe(kpipe, line, 20);
    printf("task%d read n=%d bytes from pipe : [", running->pid, n);
    for (i=0; i<n; i++)
      kputc(line[i]);
    printf("]\n");
  }
}

int main()
{ 
  color = WHITE;
  row = col = 0; 
  fbuf_init();
  kprintf("Welcome to Wanix in ARM\n");
  uart_init();
  kbd_init();

  // initialize PIPEs
  pipe.head = 0;
  pipe.tail = 0;
  pipe.data = 0;
  pipe.room = PSIZE; 
  pipe.status = FREE;
  kpipe = &pipe;

  init();
  kprintf("P0 kfork tasks\n");
  kfork((int)pipe_writer, 1); // pipe writer process
  kfork((int)pipe_reader, 1); // pipe reader process
  while(1){
    if (readyQueue)
      tswitch();
  }
}
