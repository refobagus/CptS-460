/********** init.c file **********/
#include "ucode.c"

int console, s0, s1;

int parent()
{
  int pid, status;
  while(1)
  {
    printf("INIT: wait for ZOMBIE child\n");
    pid = wait(&status);
    // if console login process died
    if(pid == console)
    {
      printf("INIT: forks a new console login\n");
      // fork another one
      console = fork();
      if(console)
        continue;
      else
        // new console login process
        exec("login /dev/tty0");;
    }

    if(pid == s0)
    {
      printf("INIT: forks a new login on serial0");
      // fork another one
      s0 = fork();
      if(s0)
        continue;
      else
        // new serial0 login process
        exec("login /dev/ttyS0");
    }

    if (pid == s1)
    {
      printf("INIT: forks a new login on serial1");
      // fork another one
      s1 = fork();
      if(s1)
        continue;
      else
        // new serial1 loging process
        exec("login /dev/ttyS1");
    }

    printf("INIT: I just buried an orphan child proc %d\n", pid);
  }
}

main()
{
  // file descriptors for terminal I/O
  int in, out;

  // file descriptor 0
  in = open("/dev/tty0", O_RDONLY);
  // for display to console
  out = open("/dev/tty0", O_WRONLY);

  console = fork();

  // parent fork
  if (console)
  {
    s0 = fork();
    // parent
    if (s0)
    {
      s1 = fork();
      // parent
      if (s1)
      {
        parent();
      }
      else
      {
        // child: exec to login on ttyS0
        exec("login /dev/ttyS1");
      }
    }
    else
    {
      // child: exec to login on ttyS1
      exec("login /dev/ttyS0");
    }
  }
  else
  {
    // child: exec to login on tty0
    exec("login /dev/tty0");;
  }
}
