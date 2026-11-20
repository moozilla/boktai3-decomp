#include "state_actor_action_entry.h"

void SceneActorState_WaitForFlagClear(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        u32 state = 12;
        p->actionAnimation = state;
    } else {
        u32 one = 1;
        if (!(p->actionFlags & one)) {
            p->actionMode = 0;
            p->actionIndex = 0;
            p->actionPhase = 0;
            p->actionTimer = 0;
            p->actionEnter = one;
            p->actionAnimation = one;
        }
    }
    p->actionTimer++;
}
