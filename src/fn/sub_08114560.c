#include "global.h"
struct H { u32 lo:16; u32 hi:16; };
struct Z { struct H a, b; };
void sub_0811D024(void *, u32, void *);
void sub_0811B278(void *, void *, struct Z *, struct Z *, u32, u32, u32, u32, u32, u32, u32);
void sub_08114560(u8 *s, u32 y, u32 x)
{
    u32 l[2];
    struct Z s1, s2;
    sub_0811D024(s + 0xa0, *(u32 *)(s + 0xc0), l);
    s1.a.lo = 0xe;
    s1.a.hi = 0xe;
    s1.b.lo = 8;
    *(u32 *)&s2.a = 0xFFFC0000;
    *(u32 *)&s2.b &= 0xFFFF0000;
    sub_0811B278(s + 4, l, &s1, &s2, 0xc, *(u8 *)(s + 0xcc), 5, x, 0xc, y, (u32)s);
}
