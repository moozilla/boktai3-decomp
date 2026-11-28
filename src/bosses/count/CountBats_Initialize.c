#include "scene/collision/contact_swarm.h"
u32 CountBats_InitializeSlot(struct ContactSwarm *, struct ContactSwarmSlot *, s32);
s32 CountBats_Initialize(struct ContactSwarm *pool)
{
    struct ContactSwarmSlot *slot;
    s32 index;
    pool->activeMask = 0;
    pool->sequenceResource = ScriptProc_Start(0x922E, 0x5BB7);
    slot = pool->slots;
    for (index = 0; index < 16; index++, slot++)
        CountBats_InitializeSlot(pool, slot, index);
    return 0;
}
