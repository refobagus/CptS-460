#include "type.h"

int read_pipe(PIPE *p, char *buf, int n)
{
  int r = 0;
  if (n<=0)
    return 0;
  while(n){
    while(p->data){
      *buf++ = p->buf[p->tail++]; // read a byte to buf
      p->tail %= PSIZE;
      p->data--; p->room++; r++; n--;
      if (n==0)
        break;
    }
    kwakeup(&p->room); // wakeup writers
    if (r) // if has read some data
      return r;
    // pipe has no data
    ksleep(&p->data); // sleep for data
  }
}

int write_pipe(PIPE *p, char *buf, int n)
{
  int r = 0;
  if (n<=0)
    return 0;
  while(n){
    while(p->room){
      p->buf[p->head++] = *buf++; // write a byte to pipe;
      p->head %= PSIZE;
      p->data++; p->room--; r++; n--;
      if (n==0)
        break;
    }
    kwakeup(&p->data);// wakeup readers, if any.
    if (n==0)
      return r;// finished writing n bytes
    // still has data to write but pipe has no room
    ksleep(&p->room);
    // sleep for room
  }
}
int pipe_writer(){ // pipe writer task code
  struct uart *up = &uart[0];
  char line[128];
  while(1){
    uprintf(uart, "Enter a line for task1 to get : ");
    printf("task%d waits for line from UART0\n", running->pid);
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
