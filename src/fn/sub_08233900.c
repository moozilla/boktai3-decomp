#include "global.h"
void sub_08217EAC(void *);
u32 sub_08233900(u8 *s) { u8 *p; s32 i; sub_08217EAC(s + 0x74); p = s + 0xa4; i = 7; do { sub_08217EAC(p); p += 0x2c; i--; } while (i >= 0); return 0; }
