#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  int p[2];
  int q[2];
  char buf[16];
  int n;

  if (argc <= 1) {
    fprintf(2, "usage: pingpong [bytes for pingpong (less than 15 bytes)]\n");
    exit(1);
  }

  pipe(p);
  pipe(q);

  if (fork() == 0) {
    n = read(q[0], buf, sizeof buf);
    if (n >= 1) {
      printf("%d: received ping: %s\n", getpid(), buf);
      write(p[1], buf, n);
    }
    close(p[0]);
    close(q[1]);
    close(q[0]);
    close(p[1]);
  } else {
    close(p[1]);
    close(q[0]);
    // write(q[1], "a", 1);
    write(q[1], argv[1], strlen(argv[1]));
    close(q[1]);
    n = read(p[0], buf, sizeof buf);
    close(p[0]);
    printf("%d: received pong: %s\n", getpid(), buf);
  }
  
  exit(0);
}