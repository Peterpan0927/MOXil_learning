#include <stdio.h>
#include <mach/mach_port.h>

kern_return_t lsPorts(task_t TargetTask)
{
  kern_return_t kr;
  mach_port_name_array_t portNames = NULL;
  mach_msg_type_number_t portNamesCount;
  mach_port_type_array_t portRightTypes = NULL;
  mach_msg_type_number_t portRightTypesCount;
  mach_port_right_t portRight;
  unsigned int p;

  kr = mach_port_names(TargetTask, &portNames, &portNamesCount,&portRightTypes,&portRightTypesCount);
  if(kr != KERN_SUCCESS){
    fprintf(stderr, "Error getting mach port names...%d", kr); return(kr);
  }
  for(p=0;p<portNamesCount;p++){
    printf("0x%x 0x%x\n", portNames[p], portRightTypes[p]);
  }
}

int main(int argc, char **argv){
  task_t targetTask;
  kern_return_t kr;
  int pid=atoi(argv[1]);
  kr=task_for_pid(mach_task_self(),pid,&targetTask);
  lsPorts(targetTask);
  return 0;
}
