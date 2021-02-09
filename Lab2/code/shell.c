//LIBRERIAS
//******************************************************************
#include <stdio.h>
#include "myutils.c"  
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
//******************************************************************

//ESTRUCTURA cmd
typedef struct cmd_struct{
    char cmd[80];
    char args[10][80];
    int nargs;
}cmd_type;
//******************************************************************

//Ejercicios 4.2 y 4.3 x
//******************************************************************
void cmds_list(cmd_type cmds[], int* num, char* argv){
    int j = 0; //Contador de comandos
    char ch; //Almacena el character despues de cada palabra
    printf("· Ejercicio 4.2 & 4.3\n");
    printf("Archivo detectado: %s\n", argv);
    int ncmds = 0; //Numero de comando 
    
    for(int i = 0; i < 100; i++){
        cmds[i].nargs = 0; //Inicializamos la lista cmds de estructura cmd con nargs a 0
    }
    int fd2 = open(argv, O_CREAT | O_RDWR, 0644);
    int res = read_split(fd2, cmds[ncmds].cmd, 80, &ch);
    char execp[80]; //Creamos una lista para almacenar vacios
    while(res > 0){
        if(ch == ' '){ //Si el caracter al final de palabra es un espacio lee la siguiente
            res = read_split(fd2, cmds[ncmds].args[cmds[ncmds].nargs], 80, &ch);
            cmds[ncmds].nargs++;

        }else if(ch == '\n'){ //Si el caracter al final de palabra es un salto de linea
            if(cmds[ncmds].nargs > 0){ //Si el numero del argumento no es el primero no hara un salto de linea
                printf("%d) cmd: %s ", ++j, cmds[ncmds].cmd);
                for(int i = 0; i < cmds[ncmds].nargs; i++){
                    printf("argv[%d]: %s ", i+1, cmds[ncmds].args[i]);
                }
                printf("\n");

            }else{ //En caso de no ser el primer argumento hara el salto de linea
                printf("%d) cmd: %s  \n", ++j, cmds[ncmds].cmd);
            }
            ncmds++; 
            res = read_split(fd2, cmds[ncmds].cmd, 80, &ch);   

        }else if(ch == '&'){ //Si el caracter al final de palabra es un &
            strcpy(cmds[ncmds].args[cmds[ncmds].nargs - 1], "&");
            res = read_split(fd2, execp, 80, &ch);  

        }else{ //En caso de haber terminado de leer el archivo hara la ultima ejecucion y finalizara el while
            if(cmds[ncmds].nargs > 0){
                printf("%d) cmd: %s ", ++j, cmds[ncmds].cmd);
                for(int i = 0; i < cmds[ncmds].nargs; i++){
                    printf("argv[%d]: %s ", i+1, cmds[ncmds].args[i]);
                }
                printf("\n");
            }else{
                printf("%d) cmd: %s  \n", ++j, cmds[ncmds].cmd);
            }
            res = 0;
        }
    }
    num = &ncmds; //Almacenamos la direccion de memoria de la variable 
    close(fd2);
}

void print_cmds_list(cmd_type cmds[]){
    int j = 0; //Contador de comandos
    int ncmds = 0; //Puntero de cmds
    while(strcmp(cmds[ncmds].cmd,"") != 0){ //Mientras el cmd actual tenga contenido
        if(cmds[ncmds].nargs > 0){ //Si tiene argumentos
            printf("%d) cmd: %s ", ++j, cmds[ncmds].cmd); //Printamos el comando con su numero
            for(int i = 0; i < cmds[ncmds].nargs; i++){
                printf("argv[%d]: %s ", i+1, cmds[ncmds].args[i]); //Printamos los argumentos
            }
            printf("\n"); //Salto de linea
        }else{ //Si no tiene argumentos
            printf("%d) cmd: %s  %s\n", ++j, cmds[ncmds].cmd, cmds[ncmds].args[cmds[ncmds].nargs]);  //Printamos el comando
        }
        ncmds++; //Aumentamos el puntero de cmds
    }
}
//******************************************************************

//Inicializamos el programa
//******************************************************************
int main(int argc, char *argv[]){

    //Declaramos variables
    char ccmds[10];
    int j = 0;
    char ch;
    char* argvs = argv[1];
    
    //Ejercicio 4.1
    //******************************************************************
    write(0,"-------------------------------------\n", sizeof("-------------------------------------\n"));
    write(0,"· Ejercicio 4.1\n", sizeof("· Ejercicio 4.1\n"));
    
    write(0,"Apertura del archivo cmds.txt",sizeof("Apertura del archivo cmds.txt"));
    int fd = open("cmds.txt", O_CREAT | O_RDWR, 0644);
    while(read_split(fd, ccmds, 10, &ch) > 0){
        printf("%d) %s\n", ++j, ccmds);
    } 
    
    printf("-------------------------------------\n");

    //Ejercicio 4.2 & 4.3
    //Creamos una funcion para desarrollarlos al principio del codigo
    //******************************************************************

    //Declaramos variables y aplicamos la funcion
    cmd_type cmds[100];
    int num;
    
    cmds_list(cmds, &num, argvs);
    char buff[10];
    char ch_end;
    int status;

    //Ejercicio 4.4 & 4.5
    //******************************************************************
    write(0,"-------------------------------------\n", sizeof("-------------------------------------\n"));
    write(0,"· Ejercicio 4.4 & 4.5 & 4.6\n", sizeof("· Ejercicio 4.4 & 4.5 & 4.6\n"));
    while(strcmp(buff,"exit") != 0){ //Se ejecuta mientras en el buff no coincida con exit o termina de leerse
        write(0,"MyShell> ", strlen("MyShell> "));
        read_split(0, buff, 5, &ch_end); //Lee por terminal y guarda en buff
        int char_num; //Inicializamos variable de indice de comando
        //Compara si el valor aceptado por pantalla (buff) es un comando valido
        if(buff[0] > '0' || buff[0] < '9'){
            sscanf(buff, "%d", &char_num); //Almacena el numero de buff en char_num
            char_num--;
            int pid; //Iniciamos el pid
            if(strcmp(cmds[char_num].cmd,"") != 0){ //Si el cmd no es vacio
                
                if(cmds[char_num].nargs > 0){ //Si tiene argumentos
                    int booleano = 0; //Creamos un booleano
                    char* cm = cmds[char_num].cmd; //Guardamos el cmd en un sitio
                    char* argv[cmds[char_num].nargs + 2]; //Guardamos sus argumentos en un array
                    argv[0] = cm; //El inicio del array tiene el cmd
                        
                    for(int i = 0; i < cmds[char_num].nargs; i++){ 
                        if(strcmp(cmds[char_num].args[i],"&") != 0){
                            argv[i+1] = cmds[char_num].args[i]; //Guardamos el argumento
                        }else{
                            booleano = 1; //Cambiamos el booleano de valor si tiene &
                        }
                    }
                    //Guardamos un NULL en la ultima posicion
                    if (booleano == 1) argv[cmds[char_num].nargs] = NULL; //Si tiene & lo ponemos en la posicion del &
                    else argv[cmds[char_num].nargs + 1] = NULL; //Sino lo ponemos al final
                    pid = fork(); //Creamos el padre
                    if(pid != 0){ //Codigo padre
                        printf("Ejecutando comando: %s ", cmds[char_num].cmd); 
                        for(int i = 0; i < cmds[char_num].nargs; i++){
                            printf("%s ", cmds[char_num].args[i]);
                        } 
                        
                        if(booleano != 1)  wait(&status); //Si no hay & esperamos la respuesta de hijo
                    } 
                    else if(pid == 0){ //Codigo hijo
                        execvp(cm,argv); //Ejecutamos el cmd indicado
                        _exit(1); //Salimos del child                          
                    }
                    
                }else{ 
                    pid = fork(); //Creamos el padre
                    if(pid != 0){ //Codigo padre
                        fflush(stdout); //Salto de linea
                        printf("Ejecutando comando: %s ", cmds[char_num].cmd);
                        wait(&status); //Esperamos al hijo
                    }
                    if(pid == 0){ //Codigo hijo
                        char* cm = cmds[char_num].cmd;
                        char* argv[2];
                        argv[0] = cm;
                        argv[1] = NULL;
                        execvp(cm,argv); //Ejecutamos comandos
                        _exit(1);
                    }
                }
                printf("\n");
            }
            else { //Si no existe la opcion
                write(0,"No command on that option.\n", strlen("No command on that option.\n"));
                if(pid == 0) _exit(2); //Y el hijo llega aqui (por casualidad)
            }
        }
        else if(strcmp(buff,"list") == 0){
            print_cmds_list(cmds); //Reimprimimos la lista
        }else if(strcmp(buff,"exit") != 0){
            write(0,"Not a valid command.\n", strlen("Not a valid command.\n")); //Si se equivocan de accion imprimimos error
        }
    
    } 

    while(wait(&status) > 0); //Terminamos todos los procesos concurrentes
    close(fd); //Cerramos el fichero
}
//******************************************************************