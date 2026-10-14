// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#define ContactSwarm_ApplyHealthContact sub_08043F64
void ContactSwarm_ApplyHealthContact(struct ContactCollider *source, struct ContactCollider *other,
                                     struct ContactSwarmSlot *slot)
{
    u32 direction;
    u32 mask = 0x100;
    if (source->flags & mask)
        direction = source->direction;
    else
        direction = Math_Atan2(other->x - source->x, other->z - source->z);
    slot->direction = direction;
    {
        u32 state = 4;
        register u32 zero asm("r1");
        zero = 0;
        slot->state = state;
        slot->enter = 1;
        slot->stateFrame = zero;
    }
}
