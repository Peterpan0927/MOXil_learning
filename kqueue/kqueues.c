#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/sysctl.h>
#include <stdio.h>
int main(int argc, char **argv){
  pid_t pid;
  int kq;
  int rc;
  int done;
  struct kevent ke;

  pid = atoi(argv[1]);
  kq = kqueue();
  
  EV_SET(&ke, pid, EVFILT_PROC, EV_ADD, NOTE_EXIT | NOTE_FORK | NOTE_EXEC, 0, NULL);
  rc = kevent(kq, &ke, 1, NULL, 0, NULL);
  if (rc < 0 ) {perror("kevent"); exit(3);}

  done = 0;
  while(!done){
    memset(&ke, '\0', sizeof(struct kevent));
    rc = kevent(kq, NULL, 0, &ke, 1, NULL);
    if (rc < 0) {perror("kevent"); exit(3);}
    if(ke.fflags & NOTE_FORK)
      printf("pid %d fork()ed\n" ,ke.ident);
    if(ke.fflags & NOTE_EXEC)
      printf("pid %d has exec()ed\n", ke.ident);
    if(ke.fflags & NOTE_EXIT ){
      printf("pid %d has exit()ed\n", ke.ident);
      done++;
    }
  }
  return 0; 
}
