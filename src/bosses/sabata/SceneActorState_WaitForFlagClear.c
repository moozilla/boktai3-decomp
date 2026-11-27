#include "bosses/state_actor_action_entry.h"

void SceneActorState_WaitForFlagClear(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        SceneActorState_SetAnimation(p, 12);
    } else {
        if (!SceneActorState_TestFlags(p, 1)) {
            SceneActorState_StartAction(p, 0, 0, 0);
            SceneActorState_SetAnimation(p, 1);
        }
    }
    p->actionTimer++;
}
