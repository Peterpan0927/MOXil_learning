#include <mach/mach.h>
#include <stdio.h>

int main(int argc, char **argv){
  mach_port_t self = host_self();
  kern_return_t rc;
  char buf[1024];
  host_basic_info_t hi;
  int len = 1024;
  
  rc = host_info(self,HOST_BASIC_INFO, (host_info_t)buf, &len);
  if(rc!=0){
    fprintf(stderr, "Nope!\n");
    return (1);
  }
  hi = (host_basic_info_t)buf;
  
  printf("CPUS:\t\t %d/%d\n", hi->avail_cpus, hi->max_cpus);
  printf("Physical CPUs:\t %d/%d\n", hi->physical_cpu, hi->physical_cpu_max);
  printf("Logic CPUS:\t %d/%d\n",hi->logical_cpu,hi->logical_cpu_max);
  printf("CPU type:\t %d/%d, ThreadType:%d\n", hi->cpu_type, hi->cpu_subtype, hi->cpu_threadtype);

  printf("memory size:\t %ld/%ld\n", hi->memory_size, hi->max_mem);
  return 0;
}
