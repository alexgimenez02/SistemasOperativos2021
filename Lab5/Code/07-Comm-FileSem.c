#include <stdio.h>       /* printf, sprintf */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>      /* fork, _exit */
#include <sys/wait.h>    /* wait */
#include <semaphore.h>
#include <time.h>    // for usleep : waits for microseconds
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>

struct timeval start;
void startTimer();
long endTimer();

#define NPROCESS 10
#define NSUMS 4


int sums[NSUMS];

int main(int argc, char *argv[])
{
	// create a file sums.dat with NSUMS zero ints
	int fd = open("sums.dat", O_CREAT | O_RDWR, 0640);
	memset(sums,0,NSUMS*sizeof(int));
	write(fd, sums, NSUMS*sizeof(int));
	close(fd);

	sem_t* mutex = sem_open("mutex", O_CREAT, 0600, 1);
	startTimer();

    for (int i=0; i<NPROCESS; i++) {
    	int pid = fork();
    	if(pid == 0) {
    		// Child code ONLY
    		usleep(10000+rand()%10000);
    		printf("I'm process %d\n",i);

    		// We need to open the file in each process
    		int fd = open("sums.dat", O_CREAT | O_RDWR, 0640);
        sem_t* mutex[NSUMS];
			for(int j = 0; j<NSUMS; j++) mutex[j] = sem_open("mutex", O_CREAT, 0600, 1);

			for(int i=0; i<500; i++) {
			    sem_wait(mutex[i%NSUMS]);
			    int sum;
			    lseek(fd,(i%NSUMS)*sizeof(int), SEEK_SET);
			    read(fd,&sum,sizeof(int));
			    sum += 1;
			    lseek(fd,(i%NSUMS)*sizeof(int), SEEK_SET);
			    write(fd,&sum,sizeof(int));
			    usleep(100+rand()%100);
			    sem_post(mutex[i%NSUMS]);
			}
			close(fd);
    		_exit(0);
    	}
    }
    // Father code ONLY
    while (wait(NULL) > 0);

    sem_close(mutex);

	fd = open("sums.dat", O_CREAT | O_RDWR, 0640);
	read(fd, sums, NSUMS*sizeof(int));
    close(fd);

	printf("Sums are: ");
	for(int i=0; i<NSUMS; i++)  printf("%d ", sums[i]);
	printf(" computed in %ld millis\n", endTimer());

    return 0;
}

void startTimer() {
  gettimeofday(&start, NULL);
}

long endTimer() {
  long mtime, seconds, useconds;    
  struct timeval end;
  gettimeofday(&end, NULL);
  seconds  = end.tv_sec  - start.tv_sec;
  useconds = end.tv_usec - start.tv_usec;
  mtime = ((seconds) * 1000 + useconds / 1000.0) + 0.5;
  return mtime;
}