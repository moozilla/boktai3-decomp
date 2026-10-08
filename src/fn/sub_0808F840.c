#include "global.h"
struct H { u32 lo:16; u32 hi:16; };
struct Z { struct H a, b; };
struct W { u16 a, b, c; };
void sub_08074BF4(u8 *, struct Z *, struct Z *, void (*)(void), u8 *);
void sub_08074D34(u8 *, struct Z *, struct Z *, void (*)(void), u32, u8 *);
void sub_0808E2CC(void);
void sub_0808E208(void);
void sub_0808F840(u8 *p)
{
    struct Z s1, s2;
    s1.a.lo = 0x40;
    s1.a.hi = 0x80;
    s1.b.lo = 0x40;
    *(u32 *)&s2.a = 0x800000;
    *(u32 *)&s2.b &= 0xFFFF0000;
    sub_08074BF4(p, &s1, &s2, sub_0808E2CC, p);
    ((struct W *)&s1)->a = 0x40;
    ((struct W *)&s1)->b = 0x64;
    ((struct W *)&s1)->c = 0x40;
    ((struct W *)&s2)->a = 0;
    ((struct W *)&s2)->b = 0x80;
    ((struct W *)&s2)->c = 0;
    sub_08074D34(p, &s1, &s2, sub_0808E208, 0, p);
}
