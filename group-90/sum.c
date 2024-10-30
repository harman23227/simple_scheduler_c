#include <stdio.h>

int main() {
  int sum = 0;
  for (int i = 1; i <= 1000; i++) {
    sum += i;
  }

  printf("The sum of the first 1000 numbers is: %d\n", sum);

  return 0;
}
