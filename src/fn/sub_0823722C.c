#include "global.h"
void sub_082371F0(void *);
u32 sub_0823722C(u8 *s) { u8 *p = s + 0x18; u8 *q = s + 0x44; s32 i = 7; do { if (*q != 0) sub_082371F0(p); p += 0x30; q += 0x30; i--; } while (i >= 0); return 0; }
