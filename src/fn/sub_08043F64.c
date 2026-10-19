#include "contact_swarm.h"
void ContactSwarm_ApplyHealthContact(struct ContactCollider *source, struct ContactCollider *other,
                                     struct ContactSwarmSlot *slot)
{
    s16 k2 = 0;
    u32 direction;
    u32 mask = 0x100;
    if (source->flags & mask) {
        direction = source->direction;
    } else {
        direction = Math_Atan2(other->x - source->x, other->z - source->z);
    }
    slot->direction = direction;
    {
        u32 state = 4;
        u32 zero;
        zero = k2;
        slot->state = state;
        slot->enter = 1;
        slot->stateFrame = zero;
    }
}
