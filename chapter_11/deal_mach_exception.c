#include <mach/mach.h>
#include <mach/port.h>
#include <mach/exception.h>
#include <mach/exception_types.h>
#include <stdio.h>
#include <mach/task.h>

mach_port_t myExceptionPort;

void signalHander(int sigNum){
  printf("Got signal %d \n", sigNum);
  exit(1);
}

void causeSomeException(int WantUnixSignals){
  char *nullPtr = NULL;
  if(WantUnixSignals) signal(11, signalHander);
  nullPtr[0] = 1;
}

void catchMACHException(mach_port_t TargetTask){
  kern_return_t rc;
  exception_mask_t myExceptionMask;
  rc = mach_port_allocate(mach_task_self(),MACH_PORT_RIGHT_RECEIVE, &myExceptionPort);
  if(rc != KERN_SUCCESS){
    fprintf(stderr, "Unable to allocate exception port\n");
    exit(1);
  }
  
  rc = mach_port_insert_right(mach_task_self(), myExceptionPort, myExceptionPort, MACH_MSG_TYPE_MAKE_SEND);
  if(rc != KERN_SUCCESS){
    fprintf(stderr, "Unable to insert right\n");
    exit(2);
  }

  myExceptionMask = EXC_MASK_ALL;
  rc = task_set_exception_ports(TargetTask, myExceptionMask, myExceptionPort, 2403, MACHINE_THREAD_STATE);
  if(rc != KERN_SUCCESS){
    fprintf(stderr, "Unable to set the exception port\n");
    exit(3);
  }
}

int main(int argc, char **argv){
  int arg, wantUnixSignals=0, wantMACHExceptions = 0;
  for(arg = 1 ; arg < argc ; arg++){
    if(strcmp(argv[arg],"-m")==0) wantMACHExceptions++;
    if(strcmp(argv[arg],"-u")==0) wantUnixSignals++;
  }
  if(wantMACHExceptions) catchMACHException(mach_task_self());
  causeSomeException(wantUnixSignals);
  fprintf(stderr, "Done\n");
  return 0;
}
