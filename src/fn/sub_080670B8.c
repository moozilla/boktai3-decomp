#include "global.h"
struct H { u32 lo:16; u32 hi:16; };
struct Z { struct H a, b; };
void sub_0821FEB4(u8 *, u32, u32, u32, u32, struct Z *, struct Z *);
void sub_0821FF84(u8 *, void (*)(void), u8 *);
void sub_0821FF24(u8 *, u8 *, u32);
void sub_0821FE40(u8 *);
void sub_080670A0(void);
void sub_080670B8(u8 *p)
{
    struct Z s1, s2;
    u8 *q;
    s1.a.lo = 0x80;
    s1.a.hi = 0x80;
    s1.b.lo = 0x52;
    s2.a.lo = 0x40;
    s2.a.hi = 0x80;
    s2.b.lo = 0x2a;
    q = p + 0x50;
    sub_0821FEB4(q, 0, 0x4001, 0, 0x10, &s2, &s1);
    sub_0821FF24(q, p + 0xa4, 0);
    sub_0821FF84(q, sub_080670A0, p);
    sub_0821FE40(q);
}
