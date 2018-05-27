#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct frame{
  void *retaddr;
  void *fp;
};

#define STACKSHOT_THREAD_SNAPSHOT_MAGIC     0xfeedface
#define STACKSHOT_TASK_SNAPSHOT_MAGIC       0xdecafbad
#define STACKSHOT_MEM_AND_IO_SNAPSHOT_MAGIC 0xbfcabcde

struct thread_snapshot {
	uint32_t 		snapshot_magic;
	uint32_t 		nkern_frames;
	uint32_t 		nuser_frames;
	uint64_t 		wait_event;
	uint64_t 	 	continuation;
	uint64_t 		thread_id;
	uint64_t 		user_time;
	uint64_t 		system_time;
	int32_t  		state;
	int32_t			priority;    /*	static priority */
	int32_t			sched_pri;   /* scheduled (current) priority */
	int32_t			sched_flags; /* scheduler flags */
	char			ss_flags;
}__attribute__ ((packed));

struct task_snapshot {
	uint32_t snapshot_magic;
	int32_t pid;
	uint64_t		uniqueid;
	uint64_t		user_time_in_terminated_threads;
	uint64_t		system_time_in_terminated_threads;
	uint8_t			shared_cache_identifier[16];
	uint64_t		shared_cache_slide;
	uint32_t		nloadinfos;
	int			suspend_count; 
	int			task_size;	/* pages */
	int			faults;		/* number of page faults */
	int			pageins;	/* number of actual pageins */
	int			cow_faults;	/* number of copy-on-write faults */
	uint32_t		ss_flags;
	uint64_t		p_start_sec;	/* from the bsd proc struct */
	uint64_t		p_start_usec;	/* from the bsd proc struct */

	/* 
	 * We restrict ourselves to a statically defined
	 * (current as of 2009) length for the
	 * p_comm string, due to scoping issues (osfmk/bsd and user/kernel
	 * binary compatibility).
	 */
	char			p_comm[17];
} __attribute__((packed));

int stack_snapshot(int pid, char *tracebuf, int bufsize, int options){
  return syscall(365,pid,tracebuf,bufsize,options);
}

int dump_thread_snapshot(struct thread_snapshot *ths){
  if(ths->snapshot_magic != STACKSHOT_THREAD_SNAPSHOT_MAGIC){
    fprintf(stderr, "Error: Magic %p expected, Found %p\n",STACKSHOT_TASK_SNAPSHOT_MAGIC, ths->snapshot_magic);
    return 0;
  }
  printf("\tThread ID:0x%x ",ths->thread_id);
  printf("State: %x\n", ths->state);
  if(ths->wait_event) printf("waiting on: 0x%x", ths->wait_event);
  if(ths->continuation)printf("\t Contitution: %p\n", ths->continuation);
  if(ths->nkern_frames || ths->nuser_frames) printf("\tFrame: %d user\n", ths->nkern_frames, ths->nuser_frames);
  return (ths->nkern_frames+ths->nuser_frames);
}

void dump_task_snapshot(struct task_snapshot *ts){
  if(ts->snapshot_magic != STACKSHOT_TASK_SNAPSHOT_MAGIC){
    fprintf(stderr, "Error: Magic %p expected ,Found %p\n", STACKSHOT_TASK_SNAPSHOT_MAGIC, ts->snapshot_magic);
    return;
  }
  fprintf(stdout, "PID: %d (%s)\n", ts->pid, ts->p_comm);
}

#define BUFSIZE 50000

int main(int argc, char **argv){
  char buf[BUFSIZE];
  int rc = stack_snapshot(-1,buf,BUFSIZE,100);
  struct task_snapshot *ts;
  struct thread_snapshot *ths;
  int off=0;
  int warn = 0;
  int nframes=0;
  
  if(rc < 0){perror("stack_snapshot"); return (-1);}
  while(off < rc){
  //遍历缓冲区，缓冲区中保存的是连续导出的snapshot结构体
  ts = (struct task_snapshot *)(buf+off);
  ths = (struct thread_snapshot *)(buf+off);
  
  switch(ts->snapshot_magic){
    case STACKSHOT_TASK_SNAPSHOT_MAGIC:
      dump_task_snapshot(ts);
      off+=(sizeof(struct task_snapshot));
      warn=0;
      break;
    case STACKSHOT_THREAD_SNAPSHOT_MAGIC:
      dump_thread_snapshot(ths);
      off+=(sizeof(struct thread_snapshot));
      off+=8;
      if(nframes) printf("\t\tReturn Addr\t Frame Ptr\n");
      while(nframes){
        struct frame *f = (struct frame *)(buf+off);
        printf("\t\t%p\t%p\n", f->retaddr, f->fp);
        off += sizeof(struct frame);
        nframes--;
      }
      warn=0;
      break;
    case STACKSHOT_MEM_AND_IO_SNAPSHOT_MAGIC:
      break;
    default:
      if(!warn){
        warn++;
        fprintf(stdout, "Magic %p at offset:d?"
                        "Seeking next magic\n",ts->snapshot_magic,off);
      }
      off++;;
    }  
  }
  return 0;
}
