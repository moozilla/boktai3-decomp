#include "global.h"
struct H {
    u32 lo : 16;
    u32 hi : 16;
};
struct Z {
    struct H a, b;
};
void Collider_Init(u8 *, u32, u32, u32, u32, struct Z *, struct Z *);
void sub_0821FF58(u8 *, u32, u32, u32, u32, u32);
void Collider_SetCallback(u8 *, void (*)(void), u8 *);
void Collider_SetPosition(u8 *, u8 *, u32);
void sub_0806A6CC(void);
void sub_0806A6D0(u8 *p, u32 a, u32 b, u32 c, u32 d)
{
    struct Z s1, s2;
    u8 *q;
    s1.a.lo = 0x10;
    s1.a.hi = 0x20;
    s1.b.lo = 0x10;
    *(u32 *)&s2.a = 0;
    *(u32 *)&s2.b &= 0xFFFF0000;
    q = p + 0x1c;
    Collider_Init(q, 0, 0x2001, 0, 0x10, &s1, &s2);
    sub_0821FF58(q, a, b, 0, c, d);
    Collider_SetCallback(q, sub_0806A6CC, p);
    if (*(u16 *)(p + 0x70) & 8)
        Collider_SetPosition(q, *(u8 **)p + 0x1c, 0);
    else
        Collider_SetPosition(q, *(u8 **)p + 0x18, 0);
}
