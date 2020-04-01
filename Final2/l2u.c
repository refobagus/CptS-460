#include "ucode.c"

#define PROG_NAME "L2U"
int fd1, fd2;

void setup(int argc, char *argv[]) {
  if (argc == 1) {
    fd1 = STDIN;
    fd2 = STDOUT;
  } else if (argc == 2) {
    fd1 = open(argv[1], O_RDONLY);
    fd2 = STDOUT;
  } else {
    fd1 = open(argv[1], O_RDONLY);
    fd2 = open(argv[2], O_WRONLY | O_CREAT);
  }
  if (fd1 < 0 || fd2 < 0) {
    printf("%s: fail to open file.\n", PROG_NAME);
    exit(1);
  }
}

void teardown() {
  close(fd1);
  close(fd2);
}

int main(int argc, char *argv[]) {
  char buf[256];
  int n;
  setup(argc, argv);
  while (n = read(fd1, buf, 256)) {
    buf[n] = 0;
    for (int i = 0; i < 256; i++) {
      if (buf[i] > 96 && buf[i] < 123)
        buf[i] -= 32;
    }
    write(fd2, buf, n);
  }
  teardown();
  return 0;
}