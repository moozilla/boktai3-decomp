#include "global.h"
extern u8 *gUnk_03006A88;
u32 sub_082468C8(u8 *a) { u8 *p; u32 r; *a = 0xff; p = *(u8 **)(gUnk_03006A88 + 0xdc); if ((u8)(p[0] + 0x4d) > 1) r = 0x10; else { *a = p[4]; r = 0; } return r; }
