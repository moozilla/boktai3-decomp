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
void FanCannon_ApplyHitReaction(void);
void Collider_SetBodyProperties(void *, u32, u32, u32);
struct Q {
    u8 pad[6];
    u16 flags;
};
// confidence=low: VulcanCannon_InitCollider's setup for the fan cannon; the signed flag at 0x11A
// swaps which extent is 128 wide.
void sub_082093F8(u8 *p)
{
    struct Z a, b;
    u8 *q = p + 0x58;
    if (*(s8 *)(p + 0x11a) == 0) {
        a.a.lo = 0x80;
        a.a.hi = 120;
        a.b.lo = 100;
    } else {
        a.a.lo = 100;
        a.a.hi = 120;
        a.b.lo = 0x80;
    }
    {
        u32 mask = 0xffff0000;
        *(u32 *)&b.a = 0x800000;
        *(u32 *)&b.b &= mask;
    }
    Collider_Init(q, 0, 0x5009, 0, 0x10, &a, &b);
    Collider_SetPosition(q, p + 0x2c, 0);
    Collider_SetCallback(q, FanCannon_ApplyHitReaction, p);
    Collider_SetBodyProperties(q, 0, *(u16 *)(p + 0x12e), *(u16 *)(p + 0x12c));
    Collider_Refresh(q);
    ((struct Q *)q)->flags |= 4;
}
