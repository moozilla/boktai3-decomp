#include "contact_sequence_actor.h"
#define ContactSequenceActor_ClearFlags sub_08045D20
void ContactSequenceActor_ClearFlags(struct ContactSequenceActor *actor, u32 mask)
{
    actor->flags &= ~mask;
}
