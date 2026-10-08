#include "global.h"
void sub_0822B358(u32);
struct S { u8 p0[0x90]; u32 w90; u8 p1[0xae - 0x94]; u8 bae; u8 p2; u8 bb0; u8 p3[0xc4 - 0xb1]; u32 wc4; u8 p4[0x17c - 0xc8]; u32 w17c; u8 p5[0]; u32 w180; u8 p6[0xee8 - 0x184]; u32 e8, ec, f0, f4; };
void sub_081F7998(struct S *p)
{
    u32 v, z;
    p->e8 = p->w17c;
    p->ec = p->w90;
    p->f0 = p->bae;
    p->f4 = p->w180;
    v = 8; z = 0;
    p->bae = v;
    p->wc4 = z;
    p->bb0 = 1;
    sub_0822B358(0x570);
}
