#include "contact_sequence_actor.h"
#define ContactSequenceActor_SetFlags sub_08045D18
void ContactSequenceActor_SetFlags(struct ContactSequenceActor *actor, u32 flags)
{
    actor->flags |= flags;
}
