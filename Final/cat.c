#include "ucode.c"

main(int argc, char *argv[])
{
  int in, out, outtty, i = 0, n;
  char buf[1024], tty[32], c;
  char string[128];

  gettty(tty);

  // open tty for write
  outtty = open(tty, O_WRONLY);

  if (argc == 1)
  {
    // for stdin
    in = 0;
    out = 1;
  }

  else
  {
    // open file
    in = open(argv[1], O_RDONLY);
    if (in < 0)
    {
      prints("Cannot open up file for cat\n\r");
      return -1;
    }
    out = 1;
  }

  while(1)
  {
    // reads one byte at a time from the file
    n = read(in, buf, 1);
    if (n < 1)
    {
      return 0;
    }

    // STDIN
    if (in == 0)
    {
      line[i] = buf[0];
      if (buf[0] != 13)
      {
        // write out
        write(out, buf, 1);
        i++;
      }
      else
      {
        // check if enter pressed, then write out the output
        write(out, "\n\r", 2);
        write(outtty, line, i);
        write(outtty, "\n\r", 2);
        memset(line, 0, 128);
        i = 0;
      }
    }
    else
    {
      // writes byte by byte out to stdinout
      write(out, buf, 1);
      // handles the newline character
      if (buf[0] == '\n')
      {
        write(outtty, "\r", 1);
      }
    }
  }

  close(in); close(outtty);
}
