#include "contact_sequence_actor.h"
void ContactSequenceActor_SetFlags(struct ContactSequenceActor *actor, u32 flags)
{
    actor->flags |= flags;
}
