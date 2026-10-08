#include "global.h"
extern u8 *gUnk_020003C8;
u8 *sub_081EA730(s32, s32);
struct G { u8 f0[0xE2C]; u16 e2c; u8 f1[3]; u16 e32; u8 f2[0xF5A - 0xE34]; u8 xa:4; u8 xb:4; u8 y; u16 b; u8 f3[0xF64 - 0xF5E]; u8 c; u8 f4[0xF86 - 0xF65]; u8 d; u8 e; };
void sub_081E4920(void)
{
    struct G *g = (struct G *)gUnk_020003C8;
    u32 three = 3;
    if (g == 0) {
        g = (struct G *)sub_081EA730(0xBB34, 0);
        if (g == 0) return;
    }
    g->b = g->xa;
    g->xa = three;
    *(u16 *)((u8 *)g + 0xF5A) = (*(u16 *)((u8 *)g + 0xF5A) & 0xF) | 0x70;
    if (g->b == 3 && g->xa == 3) {
        g->d = g->e2c;
        g->e = g->e32;
    }
    { u8 one = 1; g->c |= one; }
}
