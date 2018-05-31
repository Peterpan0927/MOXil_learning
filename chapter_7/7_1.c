#include <stdio.h>
#include <sys/socket.h>
#include <launch.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>

#define JOBKEY_LISTENERS "Listeners"
#define MAXSIZE 1024
#define CMD_MAX 80

int main(int argc, char **argv){
  launch_data_t checkinReq, checkinResp;
  launch_data_t mySocketDict;
  launch_data_t myListeners;
  
  int fdNum,fd;
  struct sockaddr sa;
  unsigned int len = sizeof(struct sockaddr);
  int fdSession;
  
  checkinReq = launch_data_new_string(LAUNCH_KEY_CHECKIN);
  checkinResp = launch_msg(checkinReq);
  
  if(!checkinResp){
    fprintf(stderr, "This Command can only be run under launchd");
    exit(2);
  }
  
  mySocketDict = launch_data_dict_lookup(checkinReq, LAUNCH_JOBKEY_SOCKETS);
  
  if(!mySocketDict){
    fprintf(stderr, "Can't find <Sockets> Key in plist\n");
    exit(1);
  }
  
  myListeners = launch_data_dict_lookup(mySocketDict, JOBKEY_LISTENERS);

  if(!myListeners){
    fprintf(stderr, "Can't find <Listeners> in Key <Sockets>\n");
    exit(1);
  }

  fdNum = launch_data_array_get_count(myListeners);
  if(fdNum!=1){
    fprintf(stderr, "Number of File Descripter is %d but should be 1\n", fdNum);
    exit(1);
  }

  fd = launch_data_get_fd(launch_data_array_get_index(myListeners,0));
  fdSession = accept(fd, &sa, &len);

  launch_data_free(checkinResp);

  fprintf(stderr, "Execing %s\n", argv[1]);

  //重定向
  dup2(fdSession, 0);
  dup2(fdSession, 1);
  dup2(fdSession, 2);
  dup2(fdSession, 255);

  execl(argv[1], argv[1], argv[2], NULL);
  close(fdSession);

  return(42);
}
