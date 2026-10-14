#include "contact_sequence_actor.h"
void ContactSequenceActor_SetState(struct ContactSequenceActor *actor, u32 state)
{
    u32 zero = 0;
    actor->state = state;
    actor->stateFrame = zero;
    actor->enter = 1;
}
