#include "global.h"
struct H { u32 lo:16; u32 hi:16; };
struct Z { struct H a, b; };
void sub_0821FEB4(u8 *, u32, u32, u32, u32, struct Z *, struct Z *);
void sub_0821FF84(u8 *, void (*)(void), u8 *);
void sub_0821FF24(u8 *, u8 *, u32);
void sub_0821FE40(u8 *);
void sub_0805431C(void);
void sub_080547B0(u8 *p)
{
    struct Z s1, s2;
    u8 *q = p + 0x48;
    s1.a.lo = 0x80;
    s1.a.hi = 0x78;
    s1.b.lo = 0x80;
    *(u32 *)&s2.a = 0x7f0000;
    *(u32 *)&s2.b &= 0xFFFF0000;
    sub_0821FEB4(q, *(u16 *)(p + 0xee), 0x4000, 0, 0x10, &s1, &s2);
    sub_0821FF84(q, sub_0805431C, p);
    sub_0821FF24(q, p + 0xe4, 0);
    sub_0821FE40(q);
}
