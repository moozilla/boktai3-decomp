#include "global.h"
struct H { u32 lo:16; u32 hi:16; };
struct Q { u8 f[0x3e]; u16 v; };
struct Z { struct H a, b; };
void sub_0821FEB4(u8 *, u32, u32, u32, u32, struct Z *, struct Z *);
void sub_0821FF84(u8 *, void (*)(void), u8 *);
void sub_0821FF24(u8 *, u8 *, u32);
void sub_0821FE40(u8 *);
void sub_0804F7E8(void);
void sub_0804FE20(u8 *p)
{
    struct Z s1, s2;
    u8 *q = p + 0x90;
    s1.a.lo = 0x80;
    s1.a.hi = 0x32;
    s1.b.lo = 0x80;
    *(u32 *)&s2.a = 0;
    *(u32 *)&s2.b &= 0xFFFF0000;
    sub_0821FEB4(q, *(u16 *)(p + 0xe8), 0x4003, 0, 0x10, &s1, &s2);
    sub_0821FF84(q, sub_0804F7E8, p);
    sub_0821FF24(q, p + 0x1c, 0);
    sub_0821FE40(q);
    ((struct Q *)q)->v = 0x258;
}
