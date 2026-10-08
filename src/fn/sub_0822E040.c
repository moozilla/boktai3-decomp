#include "global.h"
extern u8 *gUnk_02000710;
u32 sub_0822E040(s32 i) { u32 *p; if (i <= 0x1f) p = (u32 *)(gUnk_02000710 + 0x720); else { p = (u32 *)(gUnk_02000710 + 0x724); i -= 0x20; } return *p & (1 << i); }
