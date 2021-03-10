//LIBRERIAS
//******************************************************************
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "myutils.c"
#include "myutils.h"
//******************************************************************

//SELECCION DE TIPO DE SOLUCIÓN
int SOLUTION = 2; // Default implementation monitors, but reading from argv
char* str_id[5] = {"NO_SYNCH", "GLOBAL_LOCK", "ACC_LOCK", "ACC_MONITORS", "NxN LOCKS"}; // Todas las soluciones posibles

pthread_mutex_t global_mutex;

//ESTRUCTURA struct_account
//Guardamos la información de cada cuenta
typedef struct struct_account {
	int id; //Identificador
	int account_type; //Tipo de cuenta
	double balance; //Dinero en la cuenta
} account;

//ESTRUCTURA struct_sync_account
//Aplicamos un monitor_lock para cada cuenta
typedef struct struct_sync_account {
	account a; //Cuenta
	monitor_lock m; //Lock
} account_sync;

//DEFINIMOS VARIABLES GLOBALES
#define N 10               // Number of Accounts
#define NThreads 200      // Number of Threads
account_sync bank[N];

pthread_mutex_t  bank_locks[N][N];
//******************************************************************

//Función simple para aplicar varios fors simultaneos que finaliza en un contador sum
void do_something() {
	int n = 20;
	int sum = 0;
	for(int i=0; i<n; i++) 
	for(int j=0; j<n; j++) 
	for(int k=0; k<n; k++) sum++;
}

//Función para comprobar si se le puede substraer una cantidad al banco de la cuenta
bool check(account* acc, double amount) {
	return acc->balance - amount >= 0;
}

//Función para substraer un valor a la cuenta seleccionada
bool withdraw(account* acc, double amount) {
	if(check(acc,amount))  { 
		acc->balance -= amount; 
		do_something();
		return true;
	}
	return false;
}

//Función para ingresar una cantidad a la cuenta seleccionada
void deposit(account* acc, double amount) {
	acc->balance += amount;
	do_something() ;
}

//Función principal de tranferencia de dinero de una cuenta a otra
bool transfer(account_sync* from, account_sync* to, double amount) {
	monitor_lock* m1 = &from->m;
	monitor_lock* m2 = &to->m;
	account* acc_from = &from->a;
	account* acc_to = &to->a;

	if(SOLUTION==1) {
		pthread_mutex_lock(&global_mutex);
	} else if(SOLUTION==2) {
		if(acc_from->id > acc_to->id) { m1 = &to->m; m2 = &from->m; }
		pthread_mutex_lock(&m1->lock);
		pthread_mutex_lock(&m2->lock);
	} else if(SOLUTION==3) {
		if(acc_from->id > acc_to->id) { m1 = &to->m; m2 = &from->m; }
		mon_lock(m1);
		mon_lock(m2);
	} else if(SOLUTION==4) {
		if(acc_from->id < acc_to->id) pthread_mutex_lock(&bank_locks[acc_from->id][acc_to->id]);
		else pthread_mutex_lock(&bank_locks[acc_to->id][acc_from->id]);
	}

	// ******************************************
	// do transfer operation 
	bool bDone = withdraw(&from->a, amount);
	if(bDone) deposit(&to->a, amount);
	// end of transfer operation 
	// ******************************************

	if(SOLUTION==1) {
		pthread_mutex_unlock(&global_mutex);
	} else if(SOLUTION==2) {
		pthread_mutex_unlock(&m1->lock);
		pthread_mutex_unlock(&m2->lock);
	} else if(SOLUTION==3) {
		mon_unlock(m1);
		mon_unlock(m2);
	} else if(SOLUTION==4) {
		if(acc_from->id < acc_to->id) pthread_mutex_unlock(&bank_locks[acc_from->id][acc_to->id]);
		else pthread_mutex_unlock(&bank_locks[acc_to->id][acc_from->id]);
	}

	return bDone;
}

//Función inicializadora de el banco
double bankInit() {
	if(SOLUTION==1) pthread_mutex_init(&global_mutex, NULL);

	double sum = 0;
	for(int i=0; i<N; i++) {
		account_sync* acc_sync = &bank[i];
		account* acc = &acc_sync->a; 
		acc->id = i;
		acc->balance = 100;
		sum += acc->balance;
		if(SOLUTION==2) pthread_mutex_init(&acc_sync->m.lock, NULL);
		if(SOLUTION==3) mon_lock_init(&acc_sync->m);		
	}

	if(SOLUTION==4) {
  	  for(int i=0; i<N; i++) {
	    for(int j=0; j<N; j++) {
		  pthread_mutex_init(&bank_locks[i][j], NULL);
	    }
	  }
	}

	return sum;
}

//Función de transferencias aleatorias 
void* transferThread( void* param ) {
	int from = -1;
	int to = -1;
	int iter = 1000;
	while(iter--) {
		from = -1;
		to = -1;
		while(from == to) {
			from = rand()%N;
			to = rand()%N;
		}
		transfer(&bank[from], &bank[to], rand()%50+1);
	}
	return NULL;
}
//******************************************************************

//Inicializamos el programa
//******************************************************************
int main(int argc, char *argv[])
{
	if(argc > 1) SOLUTION = atoi(argv[1]);

	double sum = bankInit();
    printf("Initial bank capital: %f\n", sum);

	pthread_t* tids;          					/* the thread identifiers */
	tids = malloc(NThreads * sizeof(pthread_t));

	
    printf("SOLUTION: %s\n", str_id[SOLUTION]);
    printf("Creating %d Threads\n", NThreads);
	
	startTimer(0);
    for(int i=0; i<NThreads; i++) {
        pthread_create(&tids[i], NULL, transferThread, NULL);
    }

    printf("Main : waiting for threads...\n");
    for(int i=0; i<NThreads; i++) pthread_join(tids[i], NULL);

    free(tids);

    double sumEnd = 0;
	for(int i=0; i<N; i++) {
		printf("Account %d balance : %f\n", i, bank[i].a.balance);
		sumEnd += bank[i].a.balance;
	}
    
    if(sumEnd != sum) printf("ERROR : ******** CORRUPT BANK!!!!!!  sum: %lf *******\n",sumEnd);
    else printf("Final bank sum: %f\n",sum);
	
    printf("Done: %ld millis\n", endTimer(0));


    // Create the bank binary file 
	int fbank = open("bank.dat",  O_CREAT | O_RDWR, 0640);
	for(int i=0; i<N; i++) {
		write(fbank, &bank[i].a, sizeof(account));
	}
	close(fbank);

	return 0;
}
//******************************************************************