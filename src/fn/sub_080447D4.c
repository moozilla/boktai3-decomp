#include "contact_swarm.h"
u32 ContactSwarm_InitializeSlot(struct ContactSwarm *, struct ContactSwarmSlot *, s32);
s32 ContactSwarm_Initialize(struct ContactSwarm *pool)
{
    struct ContactSwarmSlot *slot;
    s32 index;
    pool->activeMask = 0;
    pool->sequenceResource = ScriptProc_Start(0x922E, 0x5BB7);
    slot = &pool->slots[0];
    index = 0;
    do {
        ContactSwarm_InitializeSlot(pool, slot, index);
        index++;
        slot++;
    } while (index <= 15);
    return 0;
}
