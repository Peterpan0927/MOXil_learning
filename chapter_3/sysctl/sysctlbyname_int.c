#include <sys/sysctl.h>
#include <stdio.h>

int main(){
  int maxproc;
  size_t len = sizeof(maxproc);
  sysctlbyname("kern.maxproc", &maxproc, &len, NULL, 0);
  printf("the value of kern.maxproc is %d\n", maxproc);
  return 0;
}
