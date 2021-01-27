#include <stdio.h>
#include <unistd.h>  
#include <string.h>
#include <stdlib.h>
  
int main(int argc , char *argv [] ){
    int nums[10];
char buff[10];

int fd = open(“nums.dat”, O_CREAT | O_RDWR, 0644);
}