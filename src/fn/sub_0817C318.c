#include "global.h"
#include "state_actor.h"

static inline u8 TakeFlag(struct StateActor *p)
{
    if (p->actionEnter) {
        p->actionEnter = 0;
        p->actionFinished = 0;
        return TRUE;
    }
    return FALSE;
}

// confidence=low: SceneActorState_UpdateAction22 for animation 13, unless the actor is
// already in state 13.
void sub_0817C318(struct StateActor *p)
{
    if (TakeFlag(p) && p->actionState != 13) {
        u32 v = 13;
        p->actionAnimation = v;
    }
    p->actionTimer++;
}
