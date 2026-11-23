#include "state_actor_action_entry.h"

// confidence=low: SceneActorState_UpdateAction22 for animation 13, unless the actor is
// already in state 13.
void sub_0817C318(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p) && p->actionState != 13) {
        SceneActorState_SetAnimation(p, 13);
    }
    p->actionTimer++;
}
