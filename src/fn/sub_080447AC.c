#include "contact_swarm.h"
void ContactSwarm_DestroySlot(struct ContactSwarm *, struct ContactSwarmSlot *, s32);
u32 ContactSwarm_Destroy(struct ContactSwarm *pool)
{
    struct ContactSwarmSlot *slot = pool->slots;
    s32 index = 0;
    do {
        ContactSwarm_DestroySlot(pool, slot, index);
        index++;
        slot++;
    } while (index <= 15);
    return 0;
}
