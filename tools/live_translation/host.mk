include tools/emu/Makefile
build/live_translation/runtime.so: tools/live_translation/host.c
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -DM_CORE_GBA -DENABLE_VFS -shared -fPIC -o $@ $< $(LDFLAGS) -lmgba
