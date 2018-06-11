#include <sys/types.h>
#include <sys/sysctl.h>

int main(){
  size_t len=0;
  int name[3]={CTL_KERN, KERN_PANICINFO,KERN_PANICINFO_TEST};
  sysctl(name,3,NULL,(void *)&len, NULL,0);
}
