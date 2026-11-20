#ifndef GUARD_STATE_ACTOR_ACTION_ENTRY_H
#define GUARD_STATE_ACTOR_ACTION_ENTRY_H

#include "state_actor.h"

static inline u8 StateActor_ConsumeActionEntry(struct StateActor *actor)
{
    if (actor->actionEnter) {
        actor->actionEnter = 0;
        actor->actionFinished = 0;
        return TRUE;
    }
    return FALSE;
}

#endif
