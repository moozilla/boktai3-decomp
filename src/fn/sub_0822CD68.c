#include "global.h"
extern u8 *gUnk_02000710;
void sub_0822CD68(u32 i) { *(u16 *)(gUnk_02000710 + i * 2 + 0x100) &= 0x7FFF; }
