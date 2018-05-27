#include <stdio.h>
int f(){
  char *c = malloc(24);
}

int main(void)
{
  f();
  sleep(100);
  return 0;
}
