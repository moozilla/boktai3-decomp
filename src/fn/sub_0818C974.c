#include "global.h"
struct A { u8 f0[6]; u8 b6; u8 f7; u8 b8; u8 b9; u8 fa[0x1c-0xa]; u16 w1c; u16 w1e; u16 w20; u8 g0[0xF2-0x22]; u16 wF2; u8 g1[6]; u16 wFA; u8 g2[0x13C-0xFC]; u16 w13c; u16 w13e; u16 w140; u8 g3[0x1D0-0x142]; void (*fn)(void); u16 w1d4; u16 w1d6; u8 g4[0x1DD-0x1D8]; u8 b1dd; u8 b1de; u8 b1df; };
void sub_0818C974(struct A *p)
{
    u16 *c = &p->w1d4;
    if ((*c & 3) == 0) p->wF2 += p->wFA;
    *c -= 1;
    if (*c == 0) { p->wFA = -p->wFA; *c = 0x28; }
    p->w1e += p->wF2;
}
