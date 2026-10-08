#include "global.h"
extern u8 *gUnk_02000710; u8 *sub_0822E288(s32);
void sub_0822E2A4(u32 a) { *(u16 *)(gUnk_02000710 + sub_0822E288(*(s16 *)(gUnk_02000710 + a * 2 + 0x160))[0xc] * 2 + 0x58) = a; }
