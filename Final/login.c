#include "ucode.c"

char buf[1024];

int getLineData(char *line, char *names[ ], char token)
{
  int i, n; char *cp;

  n = 0;

  //clean array
  for (i=0; i<16; i++)
      names[i]=0;

  cp = line;
  while (*cp != 0){
      
      //check for blanks and skips
      while (*cp == ' '){
        *cp++ = 0;
      }

      //get username
      if (*cp != 0){
        names[n++] = cp;
      }
      // get token
      while (*cp != token && *cp != 0){ 
	      cp++;
      }

      if (*cp != 0){
	      *cp = 0;
      }
      else{
        break;
      }
       cp++;
  }

  return n;
}

main(int argc, char *argv[])
{
  int i = 0;
  int in, out, err;
  int fd, n, size;
  char *data[100], *lines[100];
  char name[128], password[128];

  // close fd 0,1 from init
  close(0);
  close(1);
  close(2);

  // open dev/ttys0 as in out and err
  in =  open(argv[1], 0);
  out = open(argv[1], 1);
  err = open(argv[1], 2);

  // store tty string in proc tty
  settty(argv[1]);

  fd = open("/etc/passwd", O_RDONLY);

  while(1)
  {
    printf("login:");
    gets(name);

    printf("password:");
    gets(password);

    n = read(fd, buf, 1024);
    if (n <= 0)
    {
      printf("There are no passwords found\n");
    }

    //get each lines of data by tokenizing \n
    size = getLineData(buf, lines, '\n');

    for (i = 0; i < size; i++)
    {
      //get all info of 1 line
      //username:password:uid:gid:mode:directory:execmd
      getLineData(lines[i], data, ':');

      //check if valid user
      if (strcmp(name, data[0]) == 0 && strcmp(password, data[1]) == 0)
      {
        //change multiple stuff, uid gid, cd into user directory
        chuid(atoi(data[2]), atoi(data[3]));
        chdir(data[5]);

        close(fd);
        printf("Logged in as %s\n", data[0]);

        //exec sh
        exec(data[6]);
        return 1;
      }
    }
    prints("login failed, reset to login again.\n");
  }
}
