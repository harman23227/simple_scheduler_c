#include<stdio.h>
#include<sys/wait.h>
#include <sys/time.h>
#include <time.h>
int main() {
  
  for (int i = 0; i < 1000; i++) {
    //do something
    usleep(850);
  }
  printf("Hello, world!\n");
  return 0;
}
