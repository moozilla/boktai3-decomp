#include "global.h"
s32 sub_0822B2F8(u32);
void sub_08014CA4(u32, u32, void *, u32, u32, u32, u32, u32, u32, u32, u32, u32);

struct W { u32 x : 16; u32 y : 16; u32 z : 16; };
void sub_08014634(u32, u32, struct W *, struct W *, struct W *, u32, u32);

void sub_08004A34(u8 *p)
{
    struct W a;
    struct W b;
    struct W c;
    u32 d;
    u8 *q;
    sub_0822B2F8(0x135);
    q = p + 0x38;
    d = 0x1e;
    sub_08014CA4(4, 0, q, 0x3c, d, 0x14, 8, 8, 0, 0x100, 0x18, 0x10);
    a.x = *(u16 *)(p + 0x38);
    a.y = *(u16 *)(p + 0x3a) + 0x80;
    a.z = *(u16 *)(p + 0x3c);
    b.x = 0xFFF1;
    b.y = 8;
    b.z = 0xFFF1;
    c.x = d;
    c.y = 0x16;
    c.z = d;
    sub_08014634(8, 2, &a, &b, &c, 0x3c, 0x3c);
}
