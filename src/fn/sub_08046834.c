#include "contact_sequence_actor.h"
#define ContactSequencePool_BeginAllDeaths sub_08046834
#define ContactSequenceActor_SetState      sub_08045D30
void ContactSequenceActor_SetState(struct ContactSequenceActor *, u32);
s32 ContactSequencePool_BeginAllDeaths(struct ContactSequenceOwner *owner)
{
    struct ContactSequenceActor *actor;
    s32 remaining;
    if (!owner)
        return -1;
    actor = owner->actors;
    remaining = 7;
    do {
        if (actor->active && actor->state <= 2)
            ContactSequenceActor_SetState(actor, 3);
        --remaining;
        ++actor;
    } while (remaining >= 0);
    return 0;
}
