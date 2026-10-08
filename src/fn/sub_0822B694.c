#include "global.h"
extern u8 *gUnk_030025FC;
u16 sub_0822B694(u16 *o, u8 i) { u8 *e = gUnk_030025FC; u8 *q; u16 a, z; if (e == 0) return 0; q = e + ((s32)(i << 24) >> 20); a = *(u16 *)(q + 0x18); z = 0; o[0] = a; o[1] = z; o[2] = *(u16 *)(q + 0x1c); return *(u16 *)(q + 0x26); }
