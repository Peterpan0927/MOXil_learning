void bsdinit_task(void)
{
	proc_t p = current_proc();
	struct uthread *ut;
	thread_t thread;

	process_name("init", p);

	ux_handler_init();

	thread = current_thread();
	(void) host_set_exception_ports(host_priv_self(),
					EXC_MASK_ALL & ~(EXC_MASK_RPC_ALERT),//pilotfish (shark) needs this port
					(mach_port_t) ux_exception_port,
					EXCEPTION_DEFAULT| MACH_EXCEPTION_CODES,
					0);

	ut = (uthread_t)get_bsdthread_info(thread);

#if CONFIG_MACF
	mac_cred_label_associate_user(p->p_ucred);
#endif

    vm_init_before_launchd();


	bsd_init_kprintf("bsd_do_post - done");

	load_init_program(p);
	lock_trace = 1;
}
