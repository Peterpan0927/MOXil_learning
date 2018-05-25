#include <stdlib.h>
#include <stdio.h>

int global_j;
const int ci = 24;

int main(int argc , char **argv){
  int local_stack = 0;
  char *const_data = "This data is constant";
  char *tiny = malloc(32);
  char *small = malloc(2*1024);
  char *large =  malloc(1*1024*1024);
  
  printf("Text is %p\n", main);
  printf("Glocal Data is %p\n", &global_j);
  printf("Local ( Stack  ) is %p\n", &local_stack);
  printf("Constant Data is %p\n",&ci);
  printf("Hardcoded string (alse constant) are at %p\n", const_data);
  printf("Tiny allocations from %p\n",tiny);
  printf("Small allocations from %p\n",small);
  printf("Large allocations from %p\n", large);
  printf("Malloc (i.e.libSystem) is at %p\n", malloc);
  sleep(100);
  return 0;
}
