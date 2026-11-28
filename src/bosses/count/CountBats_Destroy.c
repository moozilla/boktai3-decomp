#include "scene/collision/contact_swarm.h"
void CountBats_DestroySlot(struct ContactSwarm *, struct ContactSwarmSlot *, s32);
u32 CountBats_Destroy(struct ContactSwarm *pool)
{
    struct ContactSwarmSlot *slot = pool->slots;
    s32 index;
    for (index = 0; index < 16; index++, slot++)
        CountBats_DestroySlot(pool, slot, index);
    return 0;
}
