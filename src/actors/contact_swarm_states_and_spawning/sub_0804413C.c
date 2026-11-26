#include "engine/shared/contact_swarm.h"
u32 ContactSwarm_HideSlot(struct ContactSwarm *pool, struct ContactSwarmSlot *slot, s32 index)
{
    u32 flags = slot->display.animation.draw.sprite.flags;
    struct ContactCollider *collider;
    slot->display.animation.draw.sprite.flags = flags | 1;
    collider = &slot->display.healthCollider;
    collider->flags |= 4;
    ActorShadow_Disable(slot->shadow);
    pool->activeMask &= ~(1 << index);
}
