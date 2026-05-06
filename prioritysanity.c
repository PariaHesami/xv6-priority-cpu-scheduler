#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  int pid = getpid();

  printf("pid = %d\n", pid);
  printf("default priority = %d\n", getpriority(pid));

  printf("setpriority(%d, 8) -> %d\n", pid, setpriority(pid, 8));
  printf("new priority = %d\n", getpriority(pid));

  printf("setpriority(%d, 11) -> %d\n", pid, setpriority(pid, 11));
  printf("priority after invalid set = %d\n", getpriority(pid));

  printf("getpriority(9999) -> %d\n", getpriority(9999));
  printf("setpriority(9999, 5) -> %d\n", setpriority(9999, 5));
  
  exit(0);
}
