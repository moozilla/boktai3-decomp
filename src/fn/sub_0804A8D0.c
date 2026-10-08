#include "global.h"
struct H { u32 lo:16; u32 hi:16; };
struct Z { struct H a, b; };
void sub_0821FEB4(u8 *, u32, u32, u32, u32, struct Z *, struct Z *);
void sub_0821FF7C(u8 *, u32, u32, u32);
void sub_0821FF84(u8 *, void (*)(void), u8 *);
void sub_0821FF24(u8 *, u8 *, u32);
void sub_0821FE40(u8 *);
void sub_0804ACA8(void);
void sub_0804A8D0(u8 *p)
{
    struct Z s1, s2;
    u8 *q = p + 0x130;
    u32 t;
    s1.a.lo = 0x5a;
    s1.a.hi = 0x20;
    s1.b.lo = 0x5a;
    s2.a.lo = 0x64;
    s2.a.hi = 0xc8;
    s2.b.lo = 0x64;
    if (p[0x1a] == 3) t = 0x10;
    else if (p[0x1a] == 9) t = 1;
    else if (*(u16 *)(p + 0x1c) == 0) t = 0x10;
    else t = 2;
    sub_0821FEB4(q, *(u16 *)(p + 0x18), 0x4081, 0, t, &s1, &s2);
    sub_0821FF7C(q, 0, 0, 0);
    sub_0821FF84(q, sub_0804ACA8, p);
    sub_0821FF24(q, p + 0x50, 0);
    sub_0821FE40(q);
}
