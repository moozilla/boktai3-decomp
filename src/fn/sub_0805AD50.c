#include "global.h"
struct H { u32 lo:16; u32 hi:16; };
struct Z { struct H a, b; };
void sub_0821FEB4(u8 *, u32, u32, u32, u32, struct Z *, struct Z *);
void sub_0821FF7C(u8 *, u32, u32, u32);
void sub_0821FF84(u8 *, void (*)(void), u8 *);
void sub_0821FF24(u8 *, u8 *, u32);
void sub_0821FE40(u8 *);
void sub_08059428(void);
void sub_0805AD50(u8 *p, u32 x)
{
    struct Z s1, s2;
    u8 *q = p + 0x620;
    s1.a.lo = 0x80;
    s1.a.hi = 0x80;
    s1.b.lo = 0x80;
    *(u32 *)&s2.a = 0;
    *(u32 *)&s2.b &= 0xFFFF0000;
    sub_0821FEB4(q, 0, 0x4001, 0, 0x10, &s1, &s2);
    sub_0821FF7C(q, x, 0, 0);
    sub_0821FF24(q, p + 0x618, 0);
    sub_0821FF84(q, sub_08059428, p);
    sub_0821FE40(q);
}
