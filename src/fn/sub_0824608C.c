#include "global.h"
extern u8 **gUnk_03006A88;
u32 sub_0824608C(u8 *a, u8 *b) { u8 *p; u32 r; *a = 0xff; p = *(u8 **)((u8 *)gUnk_03006A88 + 0xdc); if ((u8)(p[0] + 0x60) > 1) r = 0x10; else { p += 6; *b = p[0]; *a = p[1]; r = 0; } return r; }
