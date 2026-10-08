#include "global.h"
extern u8 *gUnk_02000710;
u8 *sub_0822E288(s32);
u8 sub_0822E0B8(u32 i) { return sub_0822E288(*(s16 *)(gUnk_02000710 + i * 2 + 0x160))[0xc]; }
