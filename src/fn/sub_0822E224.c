#include "global.h"
extern u8 *gUnk_02000710;
u32 sub_0822E224(s32 a) { s32 i = 0; s16 *p = (s16 *)(gUnk_02000710 + 0x58); for (; i <= 3; p++, i++) { if (*p == a) return 1; } return 0; }
