void int_on(int);
int int_off(void);

int ksleep(int event)
{
  int sr = int_off();
  printf("proc %d going to sleep on event=%d\n", running->pid, event);
  running->event = event;
  running->status = SLEEP;
  enqueue(&sleepList, running);
  //printList("sleepList", sleepList);
  tswitch();
  int_on(sr);
}

int kwakeup(int event)
{
  PROC *temp = 0, *p;
  int sr = int_off();
  while (p = dequeue(&sleepList)) {
    if (p->event == event) {
      p->status = READY;
      enqueue(&readyQueue, p);
    } else {
      enqueue(&temp, p);
    }
  }
  //printList("sleepList", sleepList);
  sleepList = temp;
  int_on(sr);
}

int kexit(int exitValue) {
  PROC *p1 = proc + 1, *child = running->child;
  printf("proc %d in kexit(%d)\n", running->pid, exitValue);
  running->exitCode = exitValue;
  running->status = ZOMBIE;
  
  while (child) {
    child->parent = p1;
    child->ppid = 1;
    running->child = child->sibling;
    child->sibling = p1->child;
    p1->child = child;
    child = running->child;
  }
  kwakeup((int)running->parent);
  tswitch();
}

int freeZomb(PROC *ptr){
  PROC *prev;

  if (ptr->parent->child == ptr) {
    ptr->parent->child = ptr->sibling;
  } 
  else {
    prev = ptr->parent->child;
    while (prev->sibling != ptr){
      prev = prev->sibling;
    }
    prev->sibling = ptr->sibling;
  }
  ptr->sibling = 0;   
  ptr->parent = 0;//remove all connection
  ptr->child = 0;
  ptr->ppid = 0;         
  ptr->status = FREE;
}
int kwait(int *status)
{
  /*if (caller has no child)
return -1 for error;
while(1){
// caller has children
search for a (any) ZOMBIE child;
if (found a ZOMBIE child){
get ZOMBIE child pid;
copy ZOMBIE child exitCode to *status;
release child PROC to freeList as FREE;
return ZOMBIE child pid;
}
ksleep(running);
// sleep on its PROC address
}*/
  
  PROC *parent = running;

  if (!running->child){
    printf("wait error: no child\n");
    return -1;
  }

  while(1){
    PROC *p = parent->child;
    PROC *prev = parent;

    while(p) //search children for zombies
    {
      if (p->status ==ZOMBIE) 
      {
      *status = p->exitCode;
      enqueue(&freeList, p);
      freeZomb(p);
      return p->pid;
      }
      prev = p;
      p = p->sibling;
    }
    ksleep((int)running);
  }
}

