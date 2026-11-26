#include "monsters/axe_armor/axe_armor.h"
struct H {
    u32 lo : 16;
    u32 hi : 16;
};
struct Z {
    struct H a, b;
};
void AxeArmor_ApplyHitReaction(struct ContactCollider *, struct ContactCollider *,
                               struct AxeArmor *);
void Collider_SetBodyProperties(void *, u32, u32, u32);
void AxeArmor_InitCollider(struct AxeArmor *p)
{
    struct ContactPackedVector a;
    struct ContactPackedVector b;
    struct ContactCollider *collider = &p->collider;
    a.x = 100;
    a.y = 120;
    a.z = 100;
    b.x = 0;
    b.y = 120;
    b.z = 0;
    Collider_Init(collider, 0, 0x5009, 0, 0x10, &a, &b);
    Collider_SetPosition(collider, &p->sprite.position, 0);
    Collider_SetCallback(collider, AxeArmor_ApplyHitReaction, p);
    Collider_SetBodyProperties(collider, 0, (u16)p->rangeA, (u16)p->rangeB);
    Collider_Refresh(collider);
    collider->flags |= 4;
}
