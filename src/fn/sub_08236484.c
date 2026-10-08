#include "global.h"
void sub_08217EAC(void *);
void sub_08236484(u8 *s) { u8 *p = s + 0x140; s32 i = 11; do { sub_08217EAC(p); p += 0x44; i--; } while (i >= 0); }
