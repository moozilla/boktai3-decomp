#include "global.h"
struct H {
    u32 lo : 16;
    u32 hi : 16;
};
struct Z {
    struct H a, b;
};
void Collider_Init(u8 *, u16, u32, u32, u32, struct Z *, struct Z *);
void Collider_SetCallback(u8 *, void (*)(void), u8 *);
void Collider_SetPosition(u8 *, u8 *, u32);
s32 Collider_Refresh(u8 *);
void OrangeSpine_ApplyHitReaction(void);
void Collider_SetBodyProperties(void *, u32, u32, u32);
struct Q {
    u8 pad[6];
    u16 flags;
};
void OrangeSpine_InitCollider(u8 *p)
{
    struct Z a, b;
    u8 *q = p + 0x6c;
    a.a.lo = 100;
    a.a.hi = 120;
    a.b.lo = 100;
    *(u32 *)&b.a = 0x780000;
    *(u32 *)&b.b &= 0xffff0000;
    Collider_Init(q, 0, 0x5009, 0, 0x10, &a, &b);
    Collider_SetPosition(q, p + 0x40, 0);
    Collider_SetCallback(q, OrangeSpine_ApplyHitReaction, p);
    Collider_SetBodyProperties(q, 0, *(u16 *)(p + 0x13c), *(u16 *)(p + 0x13e));
    Collider_Refresh(q);
    ((struct Q *)q)->flags |= 4;
}
