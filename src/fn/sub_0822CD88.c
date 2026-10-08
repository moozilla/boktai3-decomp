#include "global.h"
extern u8 *gUnk_02000710;
u32 sub_0822CD88(u32 i) { return *(s16 *)(gUnk_02000710 + i * 2 + 0x100) & 0x8000; }
