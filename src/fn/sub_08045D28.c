#include "contact_sequence_actor.h"
#define ContactSequenceActor_GetFlags sub_08045D28
u32 ContactSequenceActor_GetFlags(struct ContactSequenceActor *actor, u32 mask)
{
    return actor->flags & mask;
}
