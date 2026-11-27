#include "monsters/axe_armor/axe_armor.h"
void AxeArmor_ApplyHitReaction(struct ContactCollider *, struct ContactCollider *,
                               struct AxeArmor *);
void Collider_SetBodyProperties(void *, u32, u32, u32);
void AxeArmor_InitCollider(struct AxeArmor *p)
{
    struct ContactPackedVector bounds, offset;
    struct ContactCollider *collider = &p->collider;
    bounds.x = 100;
    bounds.y = 120;
    bounds.z = 100;
    offset.x = 0;
    offset.y = 120;
    offset.z = 0;
    Collider_Init(collider, 0, 0x5009, 0, 0x10, &bounds, &offset);
    Collider_SetPosition(collider, &p->sprite.position, 0);
    Collider_SetCallback(collider, AxeArmor_ApplyHitReaction, p);
    Collider_SetBodyProperties(collider, 0, (u16)p->rangeA, (u16)p->rangeB);
    Collider_Refresh(collider);
    collider->flags |= 4;
}
