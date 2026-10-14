#include "contact_sequence_actor.h"
#define ContactSequencePool_GetActiveCount sub_08046870
u32 ContactSequencePool_GetActiveCount(struct ContactSequenceOwner *owner)
{
    if (!owner)
        return 0;
    return owner->activeCount;
}
