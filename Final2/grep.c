#include "ucode.c"

int matchstar();
int matchhere();
int match();

#define PROG_NAME "GREP"
int fd;
char *pattern;

void setup(int argc, char *argv[]) {
  if (argc == 1) {
    printf("%s: not enough arguments\n", PROG_NAME);
    exit(1);
  } else if (argc == 2) {
    pattern = argv[1];
    fd = STDIN;
  } else if (argc = 3) {
    pattern = argv[1];
    fd = open(argv[2], O_RDONLY);
  } else {
    printf("%s: too many arguments\n", PROG_NAME);
    exit(1);
  }
  if (fd < 0) {
    printf("%s: fail to open %s\n", PROG_NAME, argv[2]);
    exit(1);
  }
}

void teardown() { close(fd); }

int main(int argc, char *argv[]) {
  char line[256];
  int matches = 0, n, lineno = 0;
  setup(argc, argv);
  do {
    lineno++;
    n = readline(fd, line);
    if (match(pattern, line)) {
      printf("line %d: %s", lineno, line);
      matches++;
    }
  } while (*line && n);
  printf("%d matches found\n", matches);
  teardown();
  return 0;
}

// MATCH CODE
int matchstar(int c, char *regexp, char *text) {
  do {
    if (matchhere(regexp, text))
      return 1;
  } while (*text != '\0' && (*text++ == c || c == '.'));
  return 0;
}

int matchhere(char *regexp, char *text) {
  if (regexp[0] == '\0')
    return 1;
  if (regexp[1] == '*')
    return matchstar(regexp[0], regexp + 2, text);
  if (regexp[0] == '$' && regexp[1] == '\0')
    return *text == '\0';
  if (*text != '\0' && (regexp[0] == '.' || regexp[0] == *text))
    return matchhere(regexp + 1, text + 1);
  return 0;
}

int match(char *regexp, char *text) {
  if (regexp[0] == '^')
    return matchhere(regexp + 1, text);
  do {
    if (matchhere(regexp, text))
      return 1;
  } while (*text++ != '\0');
  return 0;
}