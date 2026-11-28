#include "scene/collision/contact_swarm.h"
u32 CountBats_HideSlot(struct ContactSwarm *pool, struct ContactSwarmSlot *slot, s32 index)
{
    slot->display.animation.draw.sprite.flags |= 1;
    ContactCollider_SetFlags(&slot->display.healthCollider, 4);
    ActorShadow_Disable(slot->shadow);
    pool->activeMask &= ~(1 << index);
}
