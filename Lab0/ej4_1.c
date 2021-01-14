#include <stdio.h>
#include <unistd.h>  
#include <string.h>
#include <stdlib.h>

int main(){
    int* pa = malloc(10*sizeof(int));
    char* pch = malloc(10);
    
    //Apartado 1
    for(int i = 0; i<10; i++){
        pa[i] = 2*i+1;
    }
    
    //Apartado 2
    for(int i = 0; i<10; i++){
        printf("\nPosition of array: %d"
        "\nMemory address: %p"
        "\nMemory content: %d" 
        "\n------------------", i, &pa[i], pa[i]);
    }

    //Apartado 3
    for(int i = 0; i<10; i++){
        *(pa+i) = 2*i+1;
    }
    for(int i = 0; i<10; i++){
        printf("\nPosition of array: %d"
        "\nMemory address: %p"
        "\nMemory content: %d" 
        "\n------------------", i, (pa+i), *(pa+i));
    }
    
    //Apartado 4
    for(int i = 0; i<10; i++){
        *(pch+i) = 97 + i;
    }
    
    //Apartado 5
    for(int i = 0; i<10; i++){
        printf("\nPosition of array: %d"
        "\nMemory address: %p"
        "\nMemory content: %c" 
        "\n------------------", i, (pch+i), *(pch+i));
    }
}

