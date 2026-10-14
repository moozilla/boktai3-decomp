#include "contact_swarm.h"
u32 ContactSwarm_HideSlot(struct ContactSwarm *pool, struct ContactSwarmSlot *slot, s32 index)
{
    u32 one;
    u32 flags = slot->display.animation.draw.sprite.flags;
    struct ContactCollider *collider;
    one = 1;
    slot->display.animation.draw.sprite.flags = flags | one;
    collider = &slot->display.healthCollider;
    collider->flags |= 4;
    ActorShadow_Disable(slot->shadow);
    pool->activeMask &= ~(one << index);
}
