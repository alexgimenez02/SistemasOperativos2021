#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>

#include "myutils.h"

#define N_TRANSFERS 100
#define N_PROCESS 100
#define N 10  

typedef struct struct_account {
	int id;
	int account_type;
	double balance;
} account;

int sol;

void do_something() {
	int n = 20;
	int sum = 0;
	for(int i=0;i<n;i++) 
	for(int j=0;j<n;j++) 
	for(int k=0;k<n;k++) sum++;
}

bool check(account* acc, double amount) {
	return acc->balance - amount >= 0;
}

bool withdraw(account* acc, double amount) {
	if(check(acc,amount))  { 
		acc->balance = acc->balance - amount; 
		do_something();
		return true;
	}
	return false;
}

void deposit(account* acc, double amount) {
	acc->balance = acc->balance + amount;
	do_something();
}

void transfer(int id_from, int id_to, double amount) {
  int fd = open("bank.dat",O_RDWR,0644);
  int l1, l2;
  l1 = id_from * sizeof(account);
  l2 = id_to * sizeof(account);
	if(id_from > id_to) { int tmp = l1; l1 = l2; l2 = tmp; }
  //*******************************************
  //bloqueo de cuentas
  
  while(file_lock(fd,l1,sizeof(account)) < 0);
  while(file_lock(fd,l2,sizeof(account)) < 0);
  
  //fin de bloqueo de cuentas
  //*******************************************
  // ******************************************
	// busqueda de cuenta en archivo 
  lseek(fd,id_from*sizeof(account),SEEK_SET);
  account from; read(fd,&from,sizeof(account));

  lseek(fd,id_to*sizeof(account),SEEK_SET);
  account to; read(fd,&to,sizeof(account));
  
  // finalizacion busqueda de archivos
  //********************************************

  //********************************************
  //do transfer operation
  bool capable = withdraw(&from,amount);
	if(capable) deposit(&to,amount);

	// end of transfer operation 
	// ******************************************
  
  //*******************************************
  //escritura de cuentas en archivo
  lseek(fd,id_to*sizeof(account),SEEK_SET);
  write(fd,&to,sizeof(account));
  lseek(fd,id_from*sizeof(account),SEEK_SET);
  write(fd,&from,sizeof(account));
  //fin de escritura de cuentas en archivo
  //********************************************

  //********************************************
  //Desbloqueo de cuentas 
  file_unlock(fd,l1,sizeof(account));
  file_unlock(fd,l2,sizeof(account));
  
  close(fd);
  //Fin desbloqueo de cuentas
  //********************************************
}
void transfer_sem(int id_from, int id_to, double amount,sem_t* s) {
  int fd = open("bank.dat",O_RDWR,0644);
  int l1, l2;
  l1 = id_from * sizeof(account);
  l2 = id_to * sizeof(account);
	if(id_from > id_to) { int tmp = l1; l1 = l2; l2 = tmp; }
  //*******************************************
  //bloqueo de cuentas
  sem_wait(s);  
  //fin de bloqueo de cuentas
  //*******************************************
  // ******************************************
	// busqueda de cuenta en archivo 
  lseek(fd,id_from*sizeof(account),SEEK_SET);
  account from; read(fd,&from,sizeof(account));
  
  lseek(fd,id_to*sizeof(account),SEEK_SET);
  account to; read(fd,&to,sizeof(account));
  // finalizacion busqueda de archivos
  //********************************************

  //********************************************
  //do transfer operation
  bool capable = withdraw(&from,amount);
	if(capable)deposit(&to,amount);

	// end of transfer operation 
	// ******************************************

  //*******************************************
  //escritura de cuentas en archivo
  lseek(fd,id_to*sizeof(account),SEEK_SET);
  write(fd,&to,sizeof(account));
  lseek(fd,id_from*sizeof(account),SEEK_SET);
  write(fd,&from,sizeof(account));
  //fin de escritura de cuentas en archivo
  //********************************************

  //********************************************
  //Desbloqueo de cuentas 
  close(fd);
  //Fin desbloqueo de cuentas
  //********************************************
}

int main(int argc, char* argv[]){
  int from,to;
  double amount;
  sem_t sem;
  sem_open("transfer_semaphore" , O_CREAT, 0600,1);
  if(argc > 1) sol = atoi(argv[1]); //Convertimos la opcion en un int
  startTimer(0); 
  for(int i = 0; i < N_PROCESS; i++){ //Numero de procesos
    int pid = fork(); //Padre

    if(pid == 0){ //Codigo hijo

      for(int j = 0; j < N_TRANSFERS; j++){ //Numero de transferencias
        amount = rand()%20; //Creamos la cantidad
        from = -1; 
        to = -1; 
		    do{
			    from = rand()%N; //Indicamos el banco de origen
			    to = rand()%N;   //Indicamos el banco de destino
        }while(from == to); //Mientras sean iguales
        if(sol == 0) //Si es con file locks
          transfer(from,to,amount);
        else{ //Si es con named semaphores
          transfer_sem(from,to,amount,&sem); 
          sem_post(&sem); //Sem_signal
        }
      }
      exit(0); //Salida codigo hijo (evitar crear multiples hijos de un hijo)
    }else wait(NULL); //Esperamos al hijo actual (concurrente)

  }
  /*int fd = open("bank.dat",O_RDWR,0644);
  double sumEnd = 0;
  for(int i=0; i<N; i++) {
    lseek(fd, i*sizeof(account), SEEK_SET);
    account bank; read(fd, &bank, sizeof(account));
		printf("Account %d balance : %0.1f\n", i, bank.balance);
		sumEnd += bank.balance;
	}
  printf("Total sum: %0.1f\n",sumEnd);
  close(fd);*/
  printf("Done: %ld millis\n",endTimer(0)); 
}
