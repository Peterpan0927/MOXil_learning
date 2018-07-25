#include <mach/mach.h>
#include <stdio.h>
int main(){
  //特权用户和普通用户的返回值类型不同，但是值相同，可以参考osfmk/kern/host.c的函数
  mach_port_t h = mach_host_self();
  mach_port_t hp;
  kern_return_t rc;
  //hp只是用来接受那个拿到的特权端口,只要当前是特权用户，用的h也是一样的效果
  rc = host_get_host_priv_port(h, &hp);
  if(rc == KERN_SUCCESS) host_reboot(h,0);
  printf("%d", rc);
  printf("sorry\n");
  return 0;
}
