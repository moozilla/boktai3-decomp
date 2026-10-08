#include "global.h"
struct A { u8 f0[6]; u8 b6; u8 f7; u8 b8; u8 b9; u8 fa[0x1c-0xa]; u16 w1c; u16 w1e; u16 w20; u8 g0[0xF2-0x22]; u16 wF2; u8 g1[6]; u16 wFA; u8 g2[0x13C-0xFC]; u16 w13c; u16 w13e; u16 w140; u8 g3[0x1D0-0x142]; void (*fn)(void); u16 w1d4; u16 w1d6; u8 g4[0x1DD-0x1D8]; u8 b1dd; u8 b1de; u8 b1df; };
extern u32 gUnk_03005308;
extern u8 gUnk_0203B400[];
void sub_0818CB30(void);
void sub_0818CAF0(struct A *p)
{
    u32 v;
    gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
    v = *(u16 *)(gUnk_0203B400 + gUnk_03005308 * 2);
    v &= 0x40;
    p->w1d6 = v + 0x1e;
    p->fn = sub_0818CB30;
}
