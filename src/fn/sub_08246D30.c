#include "global.h"
extern u8 *gUnk_03006A80; u32 sub_08246D64(u32, u32, u32, u32, u32);
u16 sub_08246D30(u8 a, u8 b) { return sub_08246D64(0x40, (0x1000000u << a) >> 24, b, (u32)(gUnk_03006A80 + 0x98), 0x1a); }
