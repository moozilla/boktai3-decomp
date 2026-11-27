#include "scene/collision/contact_swarm.h"
void CountBats_DestroySlot(struct ContactSwarm *, struct ContactSwarmSlot *, s32);
u32 CountBats_Destroy(struct ContactSwarm *pool)
{
    struct ContactSwarmSlot *slot = pool->slots;
    s32 index = 0;
    do {
        CountBats_DestroySlot(pool, slot, index);
        index++;
        slot++;
    } while (index <= 15);
    return 0;
}
