//LIBRERIAS
//******************************************************************
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>

#include "myutils.h"
//******************************************************************

//DEFINIMOS VARIABLES GLOBALES
#define N 10  
#define LOOPS 100

//ESTRUCTURA struct_account
//Guardamos la información de cada cuenta
typedef struct struct_account {
	int id;
	int account_type;
	double balance;
} account;

int sol;

//Función de suma
void sum(int fd, sem_t* s){

  sem_wait(s);
  double sumEnd = 0;
  for(int i=0; i<N; i++) {
    lseek(fd, i*sizeof(account), SEEK_SET);
    account bank; read(fd, &bank, sizeof(account));
		printf("Account %d balance : %0.1f\n", i, bank.balance);
		sumEnd += bank.balance;
	}

  printf("Total money in the bank accounts: %0.1f\n", sumEnd);

}

void sum_file(int fd){

  double sumEnd = 0;
  for(int i=0; i<N; i++) {
    
    lseek(fd, i*sizeof(account), SEEK_SET);
    account bank; read(fd, &bank, sizeof(account));
    file_lock(fd, i, sizeof(account));
		printf("Account %d balance : %0.1f\n", i, bank.balance);
		sumEnd += bank.balance;
    file_unlock(fd, i, sizeof(account));
	}

  printf("Total money in the bank accounts: %0.1f\n", sumEnd);

}
//******************************************************************

//Inicializamos el programa
//******************************************************************
int main(int argc, char* argv[]){
  
  int fd, i = 0;
  if(argc > 1) sol = atoi(argv[1]);
  sem_t sum_semaphore;
  fd = open("bank.dat", O_RDWR, 0644);
  if(sol != 0) sem_open("sum_semaphore", O_CREAT, 0600, 1);
  
  do{
    startTimer(0);
    if(sol == 0){ 
      sum_file(fd);
    }
    else{
      sum(fd, &sum_semaphore);
      sem_post(&sum_semaphore);
    }
    printf("Time spent during the sum %ld millis\n", endTimer(0));
    usleep(500000);
    printf("\nSleeped through 0.5 seconds\n\n");
    i++;
  }while(i < LOOPS);

  close(fd);
  
}
//******************************************************************