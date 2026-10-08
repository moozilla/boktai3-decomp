#include "global.h"
extern u8 *gUnk_030025FC;
void sub_0822BAA0(void) { u8 *p = gUnk_030025FC; if (p != 0) { s32 lo = (s32)(p + 0x98); u32 *q; u32 z = 0; q = (u32 *)(p + 0xd4); do { *q = z; q--; } while ((s32)q >= lo); } }
