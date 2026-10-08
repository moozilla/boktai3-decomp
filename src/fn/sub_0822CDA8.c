#include "global.h"
extern u8 *gUnk_02000710; u32 sub_0822CD88(u32);
s32 sub_0822CDA8(u32 i) { s32 r; if (sub_0822CD88(i) == 0) r = *(s16 *)(gUnk_02000710 + i * 2 + 0xa0); else r = 9; return r; }
