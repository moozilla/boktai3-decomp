#include "contact_swarm.h"
#define ContactSwarm_HideSlot sub_0804413C
u32 ContactSwarm_HideSlot(struct ContactSwarm *pool, struct ContactSwarmSlot *slot, s32 index)
{
    u32 one;
    u32 flags = slot->animation.sprite.flags;
    struct ContactCollider *collider;
    one = 1;
    slot->animation.sprite.flags = flags | one;
    collider = &slot->healthCollider;
    collider->flags |= 4;
    ActorShadow_Disable(slot->shadow);
    pool->activeMask &= ~(one << index);
}
