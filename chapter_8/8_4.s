#define HNDL_ALLINTRS EXT(hndl_allintrs)
Entry(hndl_allintrs)

  /*
   * 测试是否在中断栈上
   */
  movq %gs:CPU_INT_STACK_TOP, %rcx
  cmpq %rsp, %ecx
  jb 1f
  leaq -INTSTACK_SIZE(%rcx), %rdx
  cmpq %rsp,%rdx
  jb int_from_intstack

1:
  xchgq %rcx, %rsp /* 切换到中断栈 */
  
  mov %cr0, %rax   /* 加载cr0的值 */
  orl $(CR0_TS), %eax /* 设置TS位 */
  mov %rax, %cr0    /* 设置cr0 */
  subq $r8, %rsp    /* 16字节栈对齐 */
  pushq %rcx        /* 保存原来栈的指针 */
  movq %rcx,%gs:CPU_INT_STATE /* 保存中断状态 */

  TIME_INT_ENTRY /* 定时处理 */

  incl %gs:CPU_PREEMPTION_LEVEL
  incl %gs:CPU_INTERRUPT_LEVEL

  CCALL(interrupt)  /* 调用通用中断处理程序 */

  cli               /* 以往万一返回时中断被允许 */
  xor %rax,%rax     
  movq %rax,%gs:CPU_INT_STATE /* 清除中断状态指针 */

//贯穿到return_to_iret执行，即通过iret返回用户态
