
#include "ucode.c"

char buf[1024];

main(int argc, char *argv[])
{
  int in, out, outtty, intty, n, line = 0;
  char tty[32], c;

  char *warning = "Unable to open the file\n";
  // curr tty
  gettty(tty);

  // open for r/w
  intty = open(tty, O_RDONLY);
  outtty = open(tty, O_WRONLY);

  if (argc == 1)
  {
    // STDIN
    in = 0;
    out = 1;
  }
  else
  {
    // file
    in = open(argv[1], O_RDONLY);
    if (in < 0)
    {
      write(outtty, warning, strlen(warning));
      return -1;
    }
    out = 1;
  }


  // read byte to byte
  while(1)
  {
    // read 1
    n = read(in, buf, 1);

    // keep checking until empty
    if (n < 1)
    {
      return 0;
    }

    // write 1 byte at a time
    write(outtty, buf, 1);

    // get enter
    if (buf[0] == '\n')
    {
      //go to next line
      write(outtty, "\r", 1);

      if (line < 25)
      {
        line++;
      }
      else
      {
        //check user input
        read(intty, &c, 1);

        //if get enter
        if(c == '\r')
        {
          line--;
        }
        //if get space
        else if(c == ' ')
        {
          line = 0;
        }
      }
    }
  }
  close(in); close(outtty); close(intty);
}
