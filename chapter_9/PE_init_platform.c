void PE_init_platform(boolean_t vm_initialized, void * _args)
{
    boot_args *args = (boot_args *)_args;

    if (PE_state.initialized == FALSE) {
	    PE_state.initialized        = TRUE;

        // New EFI-style
        PE_state.bootArgs           = _args;
        PE_state.deviceTreeHead	    = (void *) ml_static_ptovirt(args->deviceTreeP);
        if (args->Video.v_baseAddr) {
            PE_state.video.v_baseAddr   = args->Video.v_baseAddr; // remains physical address
            PE_state.video.v_rowBytes   = args->Video.v_rowBytes;
            PE_state.video.v_width	    = args->Video.v_width;
            PE_state.video.v_height	    = args->Video.v_height;
            PE_state.video.v_depth	    = args->Video.v_depth;
            PE_state.video.v_display    = args->Video.v_display;
            strlcpy(PE_state.video.v_pixelFormat, "PPPPPPPP",
                sizeof(PE_state.video.v_pixelFormat));
        } else {
            PE_state.video.v_baseAddr   = args->VideoV1.v_baseAddr; // remains physical address
            PE_state.video.v_rowBytes   = args->VideoV1.v_rowBytes;
            PE_state.video.v_width	    = args->VideoV1.v_width;
            PE_state.video.v_height	    = args->VideoV1.v_height;
            PE_state.video.v_depth	    = args->VideoV1.v_depth;
            PE_state.video.v_display    = args->VideoV1.v_display;
            strlcpy(PE_state.video.v_pixelFormat, "PPPPPPPP",
                    sizeof(PE_state.video.v_pixelFormat));
        }

#ifdef  kBootArgsFlagHiDPI
	if (args->flags & kBootArgsFlagHiDPI)
                PE_state.video.v_scale = kPEScaleFactor2x;
	else
                PE_state.video.v_scale = kPEScaleFactor1x;
#else
	PE_state.video.v_scale = kPEScaleFactor1x;
#endif
    }

    if (!vm_initialized) {

        if (PE_state.deviceTreeHead) {
            DTInit(PE_state.deviceTreeHead);
        }

        pe_identify_machine(args);
        pe_init_debug();
    }

}
