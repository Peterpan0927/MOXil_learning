Entry(hndl_alltraps)
  mov %esi,%eax
  testb $3, %al
  jz trap_from_kernel

  TIME_TRAP_UENTRY

  movq %gs:CPU_ACTIVE_THREAD, %rdi
  movq %rsp, ACT_PCB_ISS(%rdi) /* 隐藏PCB栈 */
  movq %rsp, %rdi              /* 将栈指针传给arg0 */
  movq %gs:CPU_KSENEL_STACK    /* 切换到内核栈 */
  sti

  CCALL(user_trap)

  //user_trap()解除中断
  cli                         /* 关中断 临界区 */
  xorl %ecx, %ecx             /* 不检查是否在PFZ中 */

  //不返回 贯穿到return_from_trap继续执行

