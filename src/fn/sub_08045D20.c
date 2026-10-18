#include "contact_sequence_actor.h"
/* ContactSequenceActor_ClearFlags: Clears the specified mask bits from the actor flags. */
#define ContactSequenceActor_ClearFlags sub_08045D20
#define ContactSequenceActor_ClearFlags sub_08045D20
void ContactSequenceActor_ClearFlags(struct ContactSequenceActor *actor, u32 mask)
{
    actor->flags &= ~mask;
}
