#include "ucode.c"

void print_line();
void print_page();

#define PROG_NAME "MORE"
int fd, fd_tty;
char buf[256];

void setup(int argc, char *argv[]) {
  if (argc == 1)
    fd = STDIN;
  else if (argc == 2) {
    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
      printf("%s: fail to open %s \n", PROG_NAME, argv[1]);
      exit(1);
    }
  } else if (argc > 3) {
    printf("%s: too many args\n", PROG_NAME);
    exit(1);
  }

  gettty(buf);
  fd_tty = open(buf, O_RDONLY);
}

int main(int argc, char *argv[]) {
  char c = 0;
  setup(argc, argv);

  print_page(fd);
  while (c != 'q') {
    c = fgetc(fd_tty);
    switch (c) {
    case '\r':
      print_line(fd);
      break;
    case ' ':
      print_page(fd);
      break;
    case 'q':
      printf(" \n");
      break;
    }
  }

  return 0;
}

// PRINTING CODE
void print_line(int fd) {
  char s[512];
  int n = readline(fd, s);
  if (n == 0 || !*s)
    exit(0);
  printf("%s", s);
}

void print_page(int fd) {
  for (int i; i < 20; i++)
    print_line(fd);
}