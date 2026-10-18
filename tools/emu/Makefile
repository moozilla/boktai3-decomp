# Needs libmgba-dev (Debian/Ubuntu: apt install libmgba-dev)
CFLAGS ?= -O2 -Wall
# Prefers the mGBA 0.11 build from tools/mgba/build_mgba.sh when it exists, so
# `loadstate` reads savestates written by Centaur (0.10 cannot).
MGBA_SRC ?= $(HOME)/git/mgba
ifneq ($(wildcard $(MGBA_SRC)/build/libmgba.*),)
# the library's own compile definitions: its struct layouts depend on them
# (ENABLE_DIRECTORIES is not in the generated flags.h)
CPPFLAGS += -I$(MGBA_SRC)/include -I$(MGBA_SRC)/build/include -I$(MGBA_SRC)/src $(shell pkg-config --cflags libavformat 2>/dev/null) \
	$(filter-out -Dmgba_EXPORTS -DLUA_VERSION_ONLY%,$(shell sed -n 's/^C_DEFINES = //p' $(MGBA_SRC)/build/CMakeFiles/mgba.dir/flags.make))
LDFLAGS += -L$(MGBA_SRC)/build -Wl,-rpath,$(MGBA_SRC)/build
else ifeq ($(shell uname -s),Darwin)
MGBA_PREFIX ?= $(shell brew --prefix mgba)
CPPFLAGS += -I$(MGBA_PREFIX)/include
LDFLAGS += -L$(MGBA_PREFIX)/lib
endif
harness: harness.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DM_CORE_GBA -DENABLE_VFS -o $@ $< $(LDFLAGS) -lmgba
