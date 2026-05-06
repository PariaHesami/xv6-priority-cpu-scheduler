#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
workload(char *label, int prio)
{
  volatile int x = 0;
  int i, j;

  setpriority(getpid(), prio);

  for(i = 0; i < 5; i++){
    for(j = 0; j < 50000000; j++){
      x += j % 3;
    }
    printf("%s pid=%d priority=%d round=%d\n", label, getpid(), getpriority(getpid()), i + 1);
  }

  exit(0);
}

int
main(void)
{
  int pid;

  pid = fork();
  if(pid == 0){
    workload("HIGH", 9);
  }

  pid = fork();
  if(pid == 0){
    workload("MED", 5);
  }

  pid = fork();
  if(pid == 0){
    workload("LOW", 1);
  }

  wait(0);
  wait(0);
  wait(0);

  exit(0);
}
