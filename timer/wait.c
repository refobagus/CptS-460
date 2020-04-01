int tswitch();

int append_orphan(PROC *p) {
	
	//assign q to init proc child list
	PROC *q = proc[1].child;


	
	if(q == 0) q = p;
	
	else {
		while(q->sibling) q = q->sibling;
	}

	q->sibling = p;
}

int ksleep(int event)
{
  int sr = int_off();
  
  //printf("proc %d going to sleep on event=%d\n", running->pid, event);

  running->event = event;
  running->status = SLEEP;
  enqueue(&sleepList, running);
  printList("sleepList", sleepList);
  tswitch();
  int_on(sr);
}

int kwakeup(int event)
{
  PROC *temp, *p;
  temp = 0;
  int sr = int_off();
  
  //printList("sleepList", sleepList);

  while (p = dequeue(&sleepList)){
     if (p->event == event){
	printf("wakeup %d\n", p->pid);
	p->status = READY;
	enqueue(&readyQueue, p);
     }
     else{
	enqueue(&temp, p);
     }
  }
  sleepList = temp;
  //printList("sleepList", sleepList);
  int_on(sr);
}



int kexit(int exit_value)
{
	running->status = ZOMBIE;


	append_orphan(running->child);
	running->exit_code = exit_value;

	kwakeup((int)running->parent);
	tswitch();
}

int kwait(int *status) {


  PROC *p, *prev = 0;
  int pid;

  if(running->child == 0) return -1;
  
  while(1) {

    p = running->child;
    //first child is a zombie
    if(p->status == ZOMBIE) {
      //reassign running child pointer
      running->child = running->child->sibling;
    }
    //first child is not a zombie
    else {
      while(p != 0 && p->status != ZOMBIE) {
        prev = p;
        p = p->sibling;
      }
    }
    
    

    if(p != 0) {
      
      //delete from child list
      if(prev) prev->sibling = p->sibling;

      pid = p->pid;
      *status = p->status;
        
      p->status   = FREE;
      p->priority = 0;      

      p->child    = 0;
      p->sibling  = 0;
      p->parent   = 0;
      
      enqueue(&freeList, p);
      
      return pid;
    }
    ksleep((int)running);   
  } 
}
