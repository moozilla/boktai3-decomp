#include "global.h"
struct H { u32 lo:16; u32 hi:16; };
struct Z { struct H a, b; };
void sub_08074BF4(u8 *, struct Z *, struct Z *, void (*)(void), u8 *);
void sub_08074E38(u8 *, u8 *, struct Z *, struct Z *, void (*)(void), u32, u32, u8 *);
void sub_080829C0(void);
void sub_08082648(void);
void sub_08083AEC(u8 *p)
{
    struct Z s1, s2, s3, s4;
    s1.a.lo = 0x20;
    s1.a.hi = 0x52;
    s1.b.lo = 0x20;
    *(u32 *)&s2.a = 0;
    *(u32 *)&s2.b &= 0xFFFF0000;
    sub_08074BF4(p, &s1, &s2, sub_080829C0, p);
    s3.a.lo = 0x20;
    s3.a.hi = 0x2a;
    s3.b.lo = 0x20;
    *(u32 *)&s4.a = 0;
    *(u32 *)&s4.b &= 0xFFFF0000;
    sub_08074E38(p, p + 0x188, &s3, &s4, sub_08082648, 0, 0x200000, p);
}
