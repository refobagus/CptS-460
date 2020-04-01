#include "ucode.c"

#define PROG_NAME "CP"
int fd1, fd2;

void setup(int argc, char *argv[]) {
  if (argc == 1) {
    printf("%s: not enough arguments\n", PROG_NAME);
    exit(1);
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
  do {
    n = read(fd1, buf, 256);
    write(fd2, buf, n);
  } while (n == 256);
  teardown();
  return 0;
}