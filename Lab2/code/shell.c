#include <stdio.h>
#include "myutils.c"  
#include <string.h>
#include <stdlib.h>

typedef struct cmd_struct{
    char cmd[80];
    char args[10][80];
    int nargs;
}cmd_type;

int main(int argc, char *argv[]){
    //char* cmds;
    int j = 0;
    char ch;
    //printf("- Ejercicio 4.1\n");
    /*
    int fd = open("cmds.txt", O_CREAT | O_RDWR, 0644);
    while(read_split(fd, cmds, 10, &ch) > 0){
        printf("%d) %s\n", ++j, cmds);
    }  
    close(fd);
    */

    printf("- Ejercicio 4.2 & 4.3\n");
    int ncmds = 0;
    cmd_type cmds[100];
    for(int i = 0; i < 100; i++){
        cmds[i].nargs = 0;
    }
    char ch2;
    int fd2 = open("cmds_args.txt", O_CREAT | O_RDWR, 0644);
    int res = read_split(fd2, cmds[ncmds].cmd, 80, &ch);
    while(res > 0){
        if(ch == ' '){
            res = read_split(fd2, cmds[ncmds].args[++cmds[ncmds].nargs], 80, &ch);
        }else if(ch == '\n'){
            if(cmds[ncmds].nargs > 0){
                printf("%d) cmd: %s ",++j,cmds[ncmds].cmd);
                for(int i = 0; i < cmds[ncmds].nargs; i++){
                    printf("argv[%d]: %s ",i+1, cmds[ncmds].args[i+1]);
                }
                printf("\n");
            }else{
                printf("%d) cmd: %s  %s\n",++j,cmds[ncmds].cmd, cmds[ncmds].args[cmds[ncmds].nargs]);
            }
            ncmds++;
            res = read_split(fd2, cmds[ncmds].cmd, 80, &ch);   
        }
    }
    close(fd2);

    
}