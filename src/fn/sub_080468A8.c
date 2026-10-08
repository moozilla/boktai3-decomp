#include "global.h"
struct P { u8 pad[0x18]; u16 h18; u8 b1a; u8 b1b; u8 b1c; u8 pad1[3]; u32 w20; u8 pad2[0x78]; u32 w9c; };
s32 Mod(s32, s32);
void sub_0822B3BC(u32);
void sub_080468A8(struct P *p)
{
    if (p->b1c != 0) {
        u32 m = -2;
        p->w9c &= ~1;
    }
    p->b1a += 6;
    p->h18 += 7;
    if (Mod(p->w20, 0x14) == 8) sub_0822B3BC(0x3bc);
    if (p->w20 > 0x3b) {
        p->b1b = 2;
        p->b1c = 1;
        p->w20 = 0;
    }
    p->w20++;
}
