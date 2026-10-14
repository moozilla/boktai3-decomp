#include "contact_sequence_actor.h"
u32 ContactSequenceActor_GetFlags(struct ContactSequenceActor *actor, u32 mask)
{
    return actor->flags & mask;
}
