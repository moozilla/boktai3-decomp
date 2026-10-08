#include "global.h"
extern u8 *gUnk_02000710; s32 sub_0822E104(u32);
u32 sub_0822E254(s32 a) { s32 i; for (i = 0; i <= 3; i++) { if (sub_0822E104(*(s16 *)(gUnk_02000710 + i * 2 + 0x58)) == a) return 1; } return 0; }
