#include "scene/collision/contact_swarm.h"
void CountBats_ApplyHealthContact(struct ContactCollider *source, struct ContactCollider *other,
                                  struct ContactSwarmSlot *slot)
{
    u32 mask = 0x100;
    if (source->flags & mask)
        slot->direction = source->direction;
    else
        slot->direction = Math_Atan2(other->x - source->x, other->z - source->z);
    {
        u32 state = 4;
        slot->state = state;
        slot->enter = 1;
        slot->stateFrame = 0;
    }
}
