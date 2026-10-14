#include "contact_sequence_actor.h"
#define ContactSequenceActor_TakeEntryFlag sub_08045D3C
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
