#include "ucode.c"

#define CNSL_PATH "/dev/tty0"
#define SRL0_PATH "/dev/ttyS0"
#define SRL1_PATH "/dev/ttyS1"

void parent_loop(), login(), set_io();
extern int open();
extern char *strcpy(), *strcat();

int cnsl_pid, srl0_pid, srl1_pid;
int in, out, err;

int main(int argc, char const *argv[]) {
  int pid = getpid();
  set_io(CNSL_PATH, &in, &out, &err);
  puts("\n### INIT ###\n");

  cnsl_pid = fork();
  if (cnsl_pid) { // parent
    srl0_pid = fork();
    if (srl0_pid) { // parent
      srl1_pid = fork();
      if (srl1_pid) { // parent
        parent_loop();
      } else { // child
        login(SRL1_PATH);
      }
    } else { // child
      login(SRL0_PATH);
    }
  } else { // child
    login(CNSL_PATH);
  }
  return 0;
}

void login(char *dev) {
  char buf[64];
  strcpy(buf, "login ");
  strcat(buf, dev);
  exec(buf);
  printf("LOGIN: error exec(%s)\n", buf);
  exit(0);
}

void parent_loop() {
  while (1) {
    int child_pid, status;
    printf("P%d waits for dead children...\n", getpid());
    child_pid = wait(&status);
    printf("P%d harvests dead child P%d with status %x\n", getpid(), child_pid,
           status);
    int new_child = fork();
    if (new_child) { // parent
      printf("P%d forks login process P%d\n", getpid(), new_child);
    } else { // child
      if (child_pid == cnsl_pid) {
        login(CNSL_PATH);
      } else if (child_pid == srl0_pid) {
        login(SRL0_PATH);
      } else if (child_pid == srl1_pid) {
        login(SRL1_PATH);
      }
    }
  }
}