#include "global.h"
struct H { u32 lo:16; u32 hi:16; };
struct Z { struct H a, b; };
void sub_08074BF4(u8 *, struct Z *, struct Z *, void (*)(void), u8 *);
void sub_08074D34(u8 *, struct Z *, struct Z *, void (*)(void), u32, u8 *);
void sub_080C58DC(void);
void sub_080C553C(void);
void sub_080C5314(u8 *p)
{
    struct Z s1, s2, s3, s4;
    s1.a.lo = 0x40;
    s1.a.hi = 0x80;
    s1.b.lo = 0x40;
    *(u32 *)&s2.a = 0x800000;
    *(u32 *)&s2.b &= 0xFFFF0000;
    sub_08074BF4(p, &s1, &s2, sub_080C58DC, p);
    s3.a.lo = 0x40;
    s3.a.hi = 0x64;
    s3.b.lo = 0x40;
    *(u32 *)&s4.a = 0x800000;
    *(u32 *)&s4.b &= 0xFFFF0000;
    sub_08074D34(p, &s3, &s4, sub_080C553C, 0, p);
}
