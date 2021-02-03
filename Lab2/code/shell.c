#include <stdio.h>
#include "myutils.c"  
#include <string.h>
#include <stdlib.h>

typedef struct cmd_struct{
    char cmd[80];
    char args[10][80];
    int nargs;
}cmd_type;


void cmds_list(cmd_type cmds[], int* num){
    int j = 0;
    char ch;
    printf("- Ejercicio 4.2 & 4.3\n");
    int ncmds = 0;
    
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
    num = &ncmds;
    close(fd2);
}

void print_cmds_list(cmd_type cmds[]){
    int j = 0;
    int ncmds = 0;
    while(strcmp(cmds[ncmds].cmd,"") != 0){
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
    }
}
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
    cmd_type cmds[100];
    int num;
    cmds_list(cmds,&num);
    char buff[10];
    char ch_end;
    write(0,"- Ejercicio 4.4\n",sizeof("- Ejercicio 4.4\n"));
    while(strcmp(buff,"exit") != 0){
        write(0,"MyShell> ",strlen("MyShell> "));
        read_split(0,buff,5,&ch_end);
        int char_num;
        if(strcmp(buff,"1") == 0 || strcmp(buff,"2") == 0 || strcmp(buff,"3") == 0 || strcmp(buff,"4") == 0 || strcmp(buff,"5") == 0 || strcmp(buff,"6") == 0 || strcmp(buff,"7") == 0 || strcmp(buff,"8") == 0 || strcmp(buff,"9") == 0){
            sscanf(buff,"%d",&char_num);
            char_num--;
            char* command = cmds[char_num].args[0];
            int narg = cmds[char_num].nargs;
            for(int i = 1; i < narg; i++){
                strcat(command," ");
                strcat(command,cmds[char_num].args[i]);
                
            }
            if(strcmp(cmds[char_num].cmd,"") != 0){
                if(cmds[char_num].nargs > 0){
                    fflush(stdout);
                    printf("Ejecutando comando: %s ",cmds[char_num].cmd);
                    for(int i = 0; i < cmds[char_num].nargs; i++){
                        printf("%s ", cmds[char_num].args[i+1]);
                    }
                
                }else{
                    fflush(stdout);
                    printf("Ejecutando comando: %s ",cmds[char_num].cmd);
                }
                printf("\n");
            }
            else write(0,"No command on that option.\n",strlen("No command on that option.\n"));
            
        }
        else if(strcmp(buff,"list") == 0){
            print_cmds_list(cmds);
        }else if(strcmp(buff,"exit") != 0){
            write(0,"Not a valid command.\n",strlen("Not a valid command.\n"));
        }
    }


    
}