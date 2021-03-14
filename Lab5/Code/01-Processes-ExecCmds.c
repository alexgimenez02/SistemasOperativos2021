#include <stdio.h>       /* printf, sprintf */
#include <string.h>
#include <unistd.h>      /* fork, _exit */
#include <sys/wait.h>    /* wait */



int main(int argc, char *argv[])
{
    int num_cmds = 2;
    char* cmds[][10] = {{"ps", NULL}, {"ls", "-la", NULL}};  
    int pid;
    
    for (int i=0; i<num_cmds; i++) {
      pid = fork();
      if(pid == 0){
        printf("Running command at position %d\n", i);
        execvp(cmds[i][0], cmds[i]);
      }
      else while (wait(NULL) > 0);
    }
    return 0;
}