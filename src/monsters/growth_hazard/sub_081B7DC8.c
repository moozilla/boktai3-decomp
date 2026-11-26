#include "global.h"

// confidence=low: RepeatedEffect_InitCollider for an effect with its collider at 0x48 and its
// position at 0xE4: a 0x80 by 0x100 attack box calling GrowthHazard_ApplyAnimationEvent.
struct H {
    u32 lo : 16;
    u32 hi : 16;
};
struct Z {
    struct H a, b;
};
struct GrowthEffect {
    u8 unknown0[0x48];
    u8 collider[0xe4 - 0x48];
    u8 position[0xee - 0xe4];
    u16 tag;
};
void Collider_Init(u8 *, u32, u32, u32, u32, struct Z *, struct Z *);
void Collider_SetCallback(u8 *, void *, void *);
void Collider_SetPosition(u8 *, u8 *, u32);
s32 Collider_Refresh(u8 *);
void GrowthHazard_ApplyAnimationEvent(void *, void *, void *);
#define GrowthEffect_InitCollider sub_081B7DC8
void GrowthEffect_InitCollider(struct GrowthEffect *p)
{
    struct Z s1, s2;
    u8 *q = p->collider;
    s1.a.lo = 0x80;
    s1.a.hi = 0x100;
    s1.b.lo = 0x80;
    *(u32 *)&s2.a = 0;
    *(u32 *)&s2.b &= 0xFFFF0000;
    Collider_Init(q, p->tag, 0x4001, 0, 0x20, &s1, &s2);
    Collider_SetCallback(q, GrowthHazard_ApplyAnimationEvent, p);
    Collider_SetPosition(q, p->position, 0);
    Collider_Refresh(q);
}
