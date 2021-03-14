#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // for sleep function : waits for seconds
#include <time.h>    // for usleep : waits for microseconds

#define NTHREADS 10


void* fsleep(int id) {
  usleep(1);
  printf("fsleep no thread function ID: %d\n", id);
  return NULL;
}

int main(int argc, char *argv[])
{
  srand(time(NULL));

  for(int i=0; i<NTHREADS; i++) {
    printf("Not creating thread  %d\n", i);
    fsleep(i);
  }
  
  return 0;
}