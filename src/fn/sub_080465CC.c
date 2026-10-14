#include "contact_sequence_actor.h"
void ContactSequenceActor_Destroy(struct ContactSequenceOwner *, struct ContactSequenceActor *);
s32 ContactSequencePool_Destroy(struct ContactSequenceOwner *owner)
{
    struct ContactSequenceActor *actor = owner->actors;
    s32 remaining = 7;
    do {
        ContactSequenceActor_Destroy(owner, actor);
        --remaining;
        ++actor;
    } while (remaining >= 0);
    return 0;
}
