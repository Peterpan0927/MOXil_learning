static char init_program_name[128]="/sbin/launchd";
struct execve_args init_exec_args;

/*
 * load_init_program
 *
 * Description:	Load the "init" program; in most cases, this will be "launchd"
 *
 * Parameters:	p			Process to call execve() to create
 *					the "init" program
 *
 * Returns:	(void)
 *
 * Notes:	The process that is passed in is the first manufactured
 *		process on the system, and gets here via bsd_ast() firing
 *		for the first time.  This is done to ensure that bsd_init()
 *		has run to completion.
 *
 *		In DEBUG & DEVELOPMENT builds, the launchdsuffix boot-arg
 *		may be used to select a specific launchd executable. As with
 *		the kcsuffix boot-arg, setting launchdsuffix to "" or "release"
 *		will force /sbin/launchd to be selected.
 *
 *              Search order by build:
 *
 * DEBUG	DEVELOPMENT	RELEASE		PATH
 * ----------------------------------------------------------------------------------
 * 1		1		NA		/usr/local/sbin/launchd.$LAUNCHDSUFFIX
 * 2		NA		NA		/usr/local/sbin/launchd.debug
 * 3		2		NA		/usr/local/sbin/launchd.development
 * 4		3		1		/sbin/launchd
 */
void
load_init_program(proc_t p)
{
	uint32_t i;
	int error;
	vm_map_t map = current_map();
	mach_vm_offset_t scratch_addr = 0;
	mach_vm_size_t map_page_size = vm_map_page_size(map);

	(void) mach_vm_allocate_kernel(map, &scratch_addr, map_page_size, VM_FLAGS_ANYWHERE, VM_KERN_MEMORY_NONE);
#if CONFIG_MEMORYSTATUS
	(void) memorystatus_init_at_boot_snapshot();
#endif /* CONFIG_MEMORYSTATUS */

#if DEBUG || DEVELOPMENT
	/* Check for boot-arg suffix first */
	char launchd_suffix[64];
	if (PE_parse_boot_argn("launchdsuffix", launchd_suffix, sizeof(launchd_suffix))) {
		char launchd_path[128];
		boolean_t is_release_suffix = ((launchd_suffix[0] == 0) ||
					       (strcmp(launchd_suffix, "release") == 0));

		if (is_release_suffix) {
			printf("load_init_program: attempting to load /sbin/launchd\n");
			error = load_init_program_at_path(p, (user_addr_t)scratch_addr, "/sbin/launchd");
			if (!error)
				return;

			panic("Process 1 exec of launchd.release failed, errno %d", error);
		} else {
			strlcpy(launchd_path, "/usr/local/sbin/launchd.", sizeof(launchd_path));
			strlcat(launchd_path, launchd_suffix, sizeof(launchd_path));

			printf("load_init_program: attempting to load %s\n", launchd_path);
			error = load_init_program_at_path(p, (user_addr_t)scratch_addr, launchd_path);
			if (!error) {
				return;
			} else {
				printf("load_init_program: failed loading %s: errno %d\n", launchd_path, error);
			}
		}
	}
#endif

	error = ENOENT;
	for (i = 0; i < sizeof(init_programs)/sizeof(init_programs[0]); i++) {
		printf("load_init_program: attempting to load %s\n", init_programs[i]);
		error = load_init_program_at_path(p, (user_addr_t)scratch_addr, init_programs[i]);
		if (!error) {
			return;
		} else {
			printf("load_init_program: failed loading %s: errno %d\n", init_programs[i], error);
		}
	}

	panic("Process 1 exec of %s failed, errno %d", ((i == 0) ? "<null>" : init_programs[i-1]), error);
}
