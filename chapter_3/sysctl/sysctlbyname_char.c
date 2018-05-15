#include <stdio.h>
#include <sys/sysctl.h>

int main(){
  char *p;
  size_t len;
  sysctlbyname("kern.hostname", NULL, &len, NULL, 0);
  p = malloc(len);
  sysctlbyname("kern.hostname", p, &len, NULL, 0);
  printf("the value of kern.hostname is %s\n", p);
  return 0;
}
