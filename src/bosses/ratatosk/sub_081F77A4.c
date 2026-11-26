#include "engine/shared/global.h"
#include "engine/shared/state_actor_action_entry.h"

void SceneActor_UpdateState31AndRestartCommand(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0x1F);
    if (p->actionFinished) {
        SceneActorState_StartAction(p, 2, 0, 0);
        Trigger_ConsumePending(p->receiver2, 1);
    } else
        p->actionTimer++;
}
