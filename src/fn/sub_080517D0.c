#include "global.h"
struct H { u32 lo:16; u32 hi:16; };
struct Z { struct H a, b; };
void sub_0821FEB4(u8 *, u16, u32, u32, u32, struct Z *, struct Z *);
void sub_0821FF84(u8 *, void (*)(void), u8 *);
void sub_0821FF24(u8 *, u8 *, u32);
void sub_0821FE40(u8 *);
void sub_080515C0(void);
void sub_080517D0(u8 *p, u32 x, u8 *y)
{
    struct Z s1, s2;
    u8 *q = p + 0x60;
    s1.a.lo = 0x80;
    s1.a.hi = 0x80;
    s1.b.lo = 0x80;
    *(u32 *)&s2.a = 0x800000;
    *(u32 *)&s2.b &= 0xFFFF0000;
    sub_0821FEB4(q, x, 0x4001, 0, 0x20, &s1, &s2);
    sub_0821FF84(q, sub_080515C0, p);
    sub_0821FF24(q, y, 0);
    sub_0821FE40(q);
}
