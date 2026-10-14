#include "contact_sequence_actor.h"
u32 ContactSequenceActor_TakeEntryFlag(struct ContactSequenceActor *actor)
{
    u32 result;
    if (!actor->enter) {
        result = 0;
    } else {
        actor->enter = 0;
        result = 1;
    }
    return result;
}
