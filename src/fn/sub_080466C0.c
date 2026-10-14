#include "contact_sequence_actor.h"
struct ContactSequenceActor *
ContactSequencePool_FindInactiveActor(struct ContactSequenceOwner *owner)
{
    struct ContactSequenceActor *actor = owner->actors;
    s32 index = 0;
    struct ContactSequenceActor *result;
    do {
        if (!actor->active) {
            result = actor;
            goto done;
        }
        ++index;
        ++actor;
    } while (index <= 7);
    result = NULL;
done:
    return result;
}
