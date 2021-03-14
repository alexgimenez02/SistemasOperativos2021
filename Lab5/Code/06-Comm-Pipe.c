#include <stdio.h>       /* printf, sprintf */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>      /* fork, _exit */
#include <sys/wait.h>    /* wait */
#include <sys/types.h>


int main(int argc, char *argv[])
{
  int fd[2];
  int i,j, pid;
    for (i=0; i<10; i++) {
      pipe(fd);
      pid = fork();
    	if(pid == 0) {
        close(fd[0]);
    		usleep(10000+rand()%10000);
    		printf("I'm process %d\n",i);
    		_exit(0);
    	}else{
        wait(NULL);
        close(fd[1]);
      }
    }
    return 0;
}