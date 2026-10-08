#include "global.h"
struct H { u32 lo:16; u32 hi:16; };
struct Z { struct H a, b; };
void sub_08074BF4(u8 *, struct Z *, struct Z *, void (*)(void), u8 *);
void sub_08074D34(u8 *, struct Z *, struct Z *, void (*)(void), u32, u8 *);
void sub_0809ED70(void);
void sub_0809ED00(void);
void sub_080A05A0(u8 *p)
{
    struct Z s1, s2, s3, s4;
    s1.a.lo = 0x2a;
    s1.a.hi = 0x80;
    s1.b.lo = 0x2a;
    *(u32 *)&s2.a = 0;
    *(u32 *)&s2.b &= 0xFFFF0000;
    sub_08074BF4(p, &s1, &s2, sub_0809ED70, p);
    s3.a.lo = 0x10;
    s3.a.hi = 0x5c;
    s3.b.lo = 0x10;
    *(u32 *)&s4.a = 0;
    *(u32 *)&s4.b &= 0xFFFF0000;
    sub_08074D34(p, &s3, &s4, sub_0809ED00, 0, p);
}
