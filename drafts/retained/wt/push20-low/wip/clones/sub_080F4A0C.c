#include "global.h"
struct A { u8 f0[6]; u8 b6; u8 f7; u8 b8; u8 b9; u8 fa[0x1c-0xa]; u16 w1c; u16 w1e; u16 w20; u8 g0[0xF2-0x22]; u16 wF2; u8 g1[6]; u16 wFA; u8 g2[0x13C-0xFC]; u16 w13c; u16 w13e; u16 w140; u8 g3[0x1D0-0x142]; void (*fn)(void); u16 w1d4; u16 w1d6; u8 g4[0x1DD-0x1D8]; u8 b1dd; u8 b1de; u8 b1df; };
void sub_0822B2F8(s32);
void sub_080F4A4C(void);
void sub_080F4A0C(struct A *p)
{
    p->b6 = 0x80;
    p->b8 = 0x7f;
    p->b9 = 1;
    p->b1de = 0xf8;
    p->b1df = 0x10;
    p->fn = sub_080F4A4C;
    sub_0822B2F8(0x16d);
}
