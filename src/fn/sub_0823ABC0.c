#include "global.h"
extern u8 *gUnk_020004B4;
u32 sub_0823ABC0(u8 *s, s32 m) { s32 i = *(s32 *)(s + 0x18); s32 r; if (gUnk_020004B4 == 0 || i < 0) r = 0; else { u8 *q = gUnk_020004B4 + 0x5c; r = q[i]; } if (r < m) return 0; return 1; }
