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
void Collider_SetCallback(u8 *, u32, u8 *);
void TetherActor_InitSecondaryCollider(u8 *p)
{
    struct Z s1, s2;
    u8 *q = p + 0x184;
    s1.a.lo = 0x80;
    s1.a.hi = 0x20;
    s1.b.lo = 0x80;
    *(u32 *)&s2.a = 0x200000;
    *(u32 *)&s2.b &= 0xFFFF0000;
    Collider_Init(q, *(u16 *)(p + 0x18), 0x2001, 0, 0x10, &s1, &s2);
    sub_0821FF58(q, *(u16 *)(p + 0x20), *(u16 *)(p + 0x22), 0, 0x40000, 0x1e);
    Collider_SetCallback(q, 0, p);
}
