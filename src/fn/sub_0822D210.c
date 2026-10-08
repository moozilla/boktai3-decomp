#include "global.h"
extern u32 gUnk_03005308; extern u8 gUnk_0203B400[]; s32 Mod(s32, s32);
u32 sub_0822D210(void) { s32 m; u32 i = (gUnk_03005308 + 1) & 0x3ff; gUnk_03005308 = i; m = Mod(*(u16 *)((u8 *)(i * 2) + (u32)gUnk_0203B400), 100); if (m <= 1) return 0x1f; return 0x1e; }
