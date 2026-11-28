#include "global.h"
#include "monsters/poison_hand.h"
#include "engine/engine.h"

void PoisonHand_ApplyHitReaction(void);
void Collider_SetBodyProperties(void *, u32, u32, u32);

void PoisonHand_InitCollider(struct PoisonHand *hand)
{
    struct ContactCollider *collider = &hand->collider;
    struct ContactPosition bounds, offset;

    bounds.x = 100;
    bounds.y = 120;
    bounds.z = 100;
    offset.x = 0;
    offset.y = 120;
    offset.z = 0;
    Collider_Init(collider, 0, 0x5009, 0, 0x10, &bounds, &offset);
    Collider_SetPosition(collider, &hand->sprite.position, 0);
    Collider_SetCallback(collider, PoisonHand_ApplyHitReaction, hand);
    Collider_SetBodyProperties(collider, 0, (u16)hand->bodyReactionFlags, (u16)hand->sparkMask);
    Collider_Refresh(collider);
    collider->flags |= 4;
}
