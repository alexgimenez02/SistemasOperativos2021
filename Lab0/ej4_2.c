#include <stdio.h>
#include <unistd.h>  
#include <string.h>
#include <stdlib.h>


void print_mem(void* pointer, char* type, int number){
    for(int i = 0; i<number; i++){
        if(strcmp(type,"int") == 0){
            printf("%d\n",*((int*) pointer + i));
        }else if(strcmp(type,"char") == 0){
            printf("%c\n",*((char*) (pointer + i)));
        }else{
            printf("The type of this pointer is %s\n", type);
        }
    }
} 

int main(){
    int* val = malloc(8*sizeof(int));

    for(int i = 0; i<8; i++){
        val[i] = 2*i;
    }

    char* chr = malloc(8*sizeof(char));;

    for(int i = 0; i<8; i++){
        *(chr + i) = 65 + i;
    }

    print_mem(val, "int", 8);
    print_mem(chr, "char", 8);

    free(val);
    free(chr);
}
