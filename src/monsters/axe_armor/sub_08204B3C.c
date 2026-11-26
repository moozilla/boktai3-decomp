#include "monsters/axe_armor/axe_armor.h"
struct H {
    u32 lo : 16;
    u32 hi : 16;
};
struct Z {
    struct H a, b;
};
void AxeArmor_ApplyHitReaction(void);
void Collider_SetBodyProperties(void *, u32, u32, u32);
void AxeArmor_InitCollider(struct AxeArmor *p)
{
    struct Z a, b;
    struct ContactCollider *collider = &p->collider;
    a.a.lo = 100;
    a.a.hi = 120;
    a.b.lo = 100;
    *(u32 *)&b.a = 0x780000;
    *(u32 *)&b.b &= 0xFFFF0000;
    Collider_Init(collider, 0, 0x5009, 0, 0x10, &a, &b);
    Collider_SetPosition(collider, &p->sprite.position, 0);
    Collider_SetCallback(collider, AxeArmor_ApplyHitReaction, p);
    Collider_SetBodyProperties(collider, 0, (u16)p->rangeA, (u16)p->rangeB);
    Collider_Refresh(collider);
    collider->flags |= 4;
}
