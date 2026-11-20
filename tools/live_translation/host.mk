include tools/emu/Makefile
build/live_translation/runtime.so: tools/live_translation/host.c
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -DM_CORE_GBA -DENABLE_VFS -shared -fPIC -o $@ $< $(LDFLAGS) -lmgba

build/live_translation/player.so: tools/live_translation/player.c
	@mkdir -p $(@D)
	$(CC) -Wall -O2 -shared -fPIC $(shell pkg-config --cflags sdl2) -o $@ $< $(shell pkg-config --libs sdl2)
