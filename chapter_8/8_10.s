Entry(hndl_syscall)
  TIME_TRAP_UENTRY

  movq %gs:CPU_KERNEL_STACK, %rdi
  xchgq %rdi, %rsp          /* 切换到内核栈 */
  movq %gs:CPU_ACTIVE_THREAD, %rcx /* 获得当前线程 */
  movq %rdi, ACT_TASK(%ecx), %rbx  /* 指向当前任务 */
  
  /* 检查当前任务的vtimer */
  TASK_VTIMER_CHECK(%rbx, %rcx)

  /* 通过系统调用类别判断 */

  movl R64_RAX(%rdi), %eax
  movl %eax, %edx
  andl $(SYSCALL_CLASS_MASK), %edx
  cmpl $(SYSCALL_CLASS_MACH << SYSCALL_CLASS_SHIFT), %edx
  je   EXT(hndl_mach_scall64)
  cmpl $(SYSCALL_CLASS_UNIX << SYSCALL_CLASS_SHIFT), %edx
  je EXT(hndl_unix_scall64)
  cmpl $(SYSCALL_CLASS_MDEP << SYSCALL_CLASS_SHIFT), %edx
  je EXT(hndl_mdep_scall64)
  cmpl $(SYSCALL_CLASS_DIAG << SYSCALL_CLASS_SHIFT), %edx
  je EXT(hndl_diag_scall64)

  CCALL(i386_exception, $(EXC_SYSCALL), %rax, $1)
