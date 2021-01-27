#include <stdio.h>
#include "myutils.c"  
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // Ejercicio 1

    int escaneo; 

    sscanf(argv[1], "%d", &escaneo);
    printf("%d\n", escaneo);

    // Ejercicio 2
    int* array = malloc(escaneo*sizeof(int));

    for(int i=0; i<escaneo; i++){
        array[i] = rand() % 100;
        //printf("Numero %d: %d\n",i,array[i]);
    }
    
    // Ejercicio 3/4/5/6
    int file_data = open("nums.dat", O_CREAT | O_RDWR, 0644);
    int file_txt = open("nums.txt", O_CREAT | O_RDWR, 0644);
    int count = 0;

    write(file_data, array, escaneo*3);

    char* string = malloc(escaneo*sizeof(char)*sizeof(int));
    for(int i=0; i<escaneo; i++){
        char str[3];
        sprintf(str, "%d", array[i]);
        strcat(string, str);
        strcat(string, "\n");
    }
    
    write(file_txt, string, escaneo*3);
    free(string);
    free(array);
    
    // Ejercicio 7/8
    
    int* dat_array = malloc(escaneo*sizeof(int));
    read(file_txt, dat_array, escaneo*sizeof(int));
    printf("%ls", dat_array);

    close(file_data);
    close(file_txt);
    
    return 0;
}