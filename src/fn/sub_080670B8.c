#include "global.h"
struct H {
    u32 lo : 16;
    u32 hi : 16;
};
struct Z {
    struct H a, b;
};
void Collider_Init(u8 *, u32, u32, u32, u32, struct Z *, struct Z *);
void Collider_SetCallback(u8 *, void (*)(void), u8 *);
void Collider_SetPosition(u8 *, u8 *, u32);
void Collider_Refresh(u8 *);
void sub_080670A0(void);
void sub_080670B8(u8 *p)
{
    struct Z s1, s2;
    u8 *q;
    s1.a.lo = 0x80;
    s1.a.hi = 0x80;
    s1.b.lo = 0x52;
    s2.a.lo = 0x40;
    s2.a.hi = 0x80;
    s2.b.lo = 0x2a;
    q = p + 0x50;
    Collider_Init(q, 0, 0x4001, 0, 0x10, &s2, &s1);
    Collider_SetPosition(q, p + 0xa4, 0);
    Collider_SetCallback(q, sub_080670A0, p);
    Collider_Refresh(q);
}
