#include "global.h"
extern u8 *gUnk_020004B4;
u8 *sub_0823AC94(u8 *s) { s32 i = *(s32 *)(s + 0x18); u8 *g = gUnk_020004B4; u8 *r; if (g == 0 || g[0x53] != 0 || i < 0) r = 0; else r = g + (i * 68 + 0x1c0); return r; }
