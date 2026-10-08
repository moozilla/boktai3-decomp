#include "global.h"
extern u8 *gUnk_02000710; void sub_0822E078(u32);
void sub_0822E0DC(u32 i, u32 v) { sub_0822E078(v); *(u16 *)(gUnk_02000710 + i * 2 + 0x160) = v; }
