#include "scene/collision/contact_swarm.h"
void CountBats_SetDirectionalSequence(struct ContactSwarm *, struct ContactSwarmSlot *, u32);
void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);

s32 CountBats_SpawnCharge(struct ContactSwarm *pool, const struct ContactPosition *position,
                             u32 direction, u32 speed, u32 resourceId, u32 cost, u32 option)
{
    struct ContactSwarmSlot *slot = pool->slots;
    s32 index;
    for (index = 0; index <= 15; index++, slot++) {
        if (pool->activeMask & (1 << index))
            continue;
        slot->state = 3;
        slot->enter = 1;
        slot->stateFrame = 0;
        slot->direction = direction;
        slot->speed = speed;
        slot->display.animation.draw.sprite.position = *position;
        AnimData_SelectPaletteBank(&slot->display.animation.draw.graphics, 0x2D3);
        slot->display.animation.draw.sprite.flags &= ~1;
        CountBats_SetDirectionalSequence(pool, slot, 0);
        ActorShadow_Enable(slot->shadow);
        Collider_SetPosition(&slot->resourceCollider, &slot->display.animation.draw.sprite.position,
                             0);
        Collider_SetAttackProperties(&slot->resourceCollider, resourceId, cost, 0x8000, 0, option);
        Collider_SetPosition(&slot->display.healthCollider,
                             &slot->display.animation.draw.sprite.position, 0);
        slot->display.healthCollider.flags |= 4;
        pool->activeMask |= 1 << index;
        break;
    }
    return 0;
}
