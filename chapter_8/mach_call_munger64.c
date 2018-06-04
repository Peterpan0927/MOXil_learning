void mach_call_munger64(x86_saved_state_t *state)
{
	int call_number;
	int argc;
	mach_call_t mach_call;
	struct mach_call_args args = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	x86_saved_state64_t	*regs;

	struct uthread *ut = get_bsdthread_info(current_thread());
	uthread_reset_proc_refcount(ut);

	assert(is_saved_state64(state));
	regs = saved_state64(state);
  
  //在32bit版本中，call_number = -(regs->eax);
	call_number = (int)(regs->rax & SYSCALL_NUMBER_MASK);

	DEBUG_KPRINT_SYSCALL_MACH(
		"mach_call_munger64: code=%d(%s)\n",
		call_number, mach_syscall_name_table[call_number]);

	KERNEL_DEBUG_CONSTANT_IST(KDEBUG_TRACE, 
		MACHDBG_CODE(DBG_MACH_EXCP_SC,(call_number)) | DBG_FUNC_START,
		regs->rdi, regs->rsi, regs->rdx, regs->r10, 0);
	
	if (call_number < 0 || call_number >= mach_trap_count) {
	        i386_exception(EXC_SYSCALL, regs->rax, 1);
		/* NOTREACHED */
	}
	mach_call = (mach_call_t)mach_trap_table[call_number].mach_trap_function;

	if (mach_call == (mach_call_t)kern_invalid) {
	        i386_exception(EXC_SYSCALL, regs->rax, 1);
		/* NOTREACHED */
	}
	argc = mach_trap_table[call_number].mach_trap_arg_count;
	if (argc) {
		int args_in_regs = MIN(6, argc);

		memcpy(&args.arg1, &regs->rdi, args_in_regs * sizeof(syscall_arg_t));

		if (argc > 6) {
	        int copyin_count;

			assert(argc <= 9);
			copyin_count = (argc - 6) * (int)sizeof(syscall_arg_t);

	        if (copyin((user_addr_t)(regs->isf.rsp + sizeof(user_addr_t)), (char *)&args.arg7, copyin_count)) {
		        regs->rax = KERN_INVALID_ARGUMENT;
			
				thread_exception_return();
				/* NOTREACHED */
			}
		}
	}

#ifdef MACH_BSD
	mach_kauth_cred_uthread_update();
#endif

	regs->rax = (uint64_t)mach_call((void *)&args);
	
	DEBUG_KPRINT_SYSCALL_MACH( "mach_call_munger64: retval=0x%llx\n", regs->rax);

	KERNEL_DEBUG_CONSTANT_IST(KDEBUG_TRACE, 
		MACHDBG_CODE(DBG_MACH_EXCP_SC,(call_number)) | DBG_FUNC_END, 
		regs->rax, 0, 0, 0, 0);

#if DEBUG || DEVELOPMENT
	kern_allocation_name_t
	prior __assert_only = thread_get_kernel_state(current_thread())->allocation_name;
	assertf(prior == NULL, "thread_set_allocation_name(\"%s\") not cleared", kern_allocation_get_name(prior));
#endif /* DEBUG || DEVELOPMENT */

	throttle_lowpri_io(1);

#if PROC_REF_DEBUG
	if (__improbable(uthread_get_proc_refcount(ut) != 0)) {
		panic("system call returned with uu_proc_refcount != 0");
	}
#endif

	thread_exception_return();
	/* 不会运行到这里 */
}
