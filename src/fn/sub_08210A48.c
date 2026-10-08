#include "global.h"
extern u8 *gUnk_02000710;
u32 sub_08210A48(u32 i) { return *(u32 *)(gUnk_02000710 + 0x728) & (1 << i); }
