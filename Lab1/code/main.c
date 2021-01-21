#include <stdio.h>
#include "myutils.c"  
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  // Ejercicio 1
  printf("Ejercicio 1");
  char* escaneo;
  sscanf(escaneo, "%s", argv[1]);
  printf("%d", escaneo);


  // Ejercicio 2
  int* array = malloc(50*sizeof(int));

  for(int i=0; i<50; i++){
    array[i] = rand() 100%;

  }
}