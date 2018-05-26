#!/usr/sbin/dtrace -s


syscall::open:entry
{
  self->tracing = 1;
  printf("file at: %x opened with mode %x", arg0, arg1);
}

fbt:::entry
/ self->tracing /
{
  printf("%x %x %x",arg0, arg1, arg2);
}

fbt::open:entry
/ self->tracing /
{ 
  printf("PID %d (%s) is opening , the father process's pid is %d\n", ((proc_t)arg0)->p_pid, ((proc_t)arg0)->p_comm, ((proc_t)arg0)->p_ppid);
}

fbt:::return 
/ self->tracing /
{
  printf("Returned %x\n",arg1);
}

syscall::open:return
/ self->tracing / 
{
  self->tracing=0;
  exit(0)
}
