# Needs libmgba-dev (Debian/Ubuntu: apt install libmgba-dev)
CFLAGS ?= -O2 -Wall
ifeq ($(shell uname -s),Darwin)
MGBA_PREFIX ?= $(shell brew --prefix mgba)
CPPFLAGS += -I$(MGBA_PREFIX)/include
LDFLAGS += -L$(MGBA_PREFIX)/lib
endif
harness: harness.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DM_CORE_GBA -DENABLE_VFS -o $@ $< $(LDFLAGS) -lmgba
