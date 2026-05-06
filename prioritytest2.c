#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
busy(char *label, int prio)
{
  volatile int x = 0;
  int i, j;

  setpriority(getpid(), prio);

  for(i = 0; i < 15; i++){
    for(j = 0; j < 40000000; j++){
      x += (j % 7);
    }
    printf("%s pid=%d current_priority=%d round=%d\n",
           label, getpid(), getpriority(getpid()), i + 1);
  }

  exit(0);
}

int
main(void)
{
  int pid;

  pid = fork();
  if(pid == 0){
    busy("HIGH", 7);
  }

  pid = fork();
  if(pid == 0){
    busy("LOW1", 1);
  }

  pid = fork();
  if(pid == 0){
    busy("LOW2", 1);
  }

  wait(0);
  wait(0);
  wait(0);

  exit(0);
}
