#include <stdio.h>

int main(){

  int rc = fork();
  int child = r0;
  switch(rc){
    case -1:
      fprintf(stderr, "Unable to fork\n");
      return (1);
    case 0:
      printf("I am a child! I am born\n");
      child++;
      break;
    default:
      printf("I am a parent!Going to sleep and now waiting\n");
      break;
  }
  printf("%s exiting\n" child ? "child" : "parent");
  return (0);
}

