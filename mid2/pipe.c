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

