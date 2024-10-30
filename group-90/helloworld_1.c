#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>  // Include this header for usleep
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>

int main() {
    for (int i = 0; i < 100000; i++) {
        // Simulate some work
        usleep(200); // Sleep for 200 microseconds
    }
    printf("Hello, world!\n");
    return 0;
}

