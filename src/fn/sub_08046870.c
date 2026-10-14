#include "contact_sequence_actor.h"
u32 ContactSequencePool_GetActiveCount(struct ContactSequenceOwner *owner)
{
    if (!owner)
        return 0;
    return owner->activeCount;
}
