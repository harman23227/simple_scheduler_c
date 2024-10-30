#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <stdbool.h>
#include <signal.h> 
#include <sys/time.h>
#include <time.h>
#include <errno.h>

int main() {
  
  for (int i = 0; i < 1000; i++) {
    //do something
    usleep(500);
  }
  printf("Hello, world!\n");
  return 0;
}
