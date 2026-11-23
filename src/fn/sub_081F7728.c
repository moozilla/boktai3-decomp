#include "global.h"
#include "state_actor_action_entry.h"

void SceneActor_UpdateState30AndRestartCommand(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        SceneActorState_SetAnimation(p, 0x1E);
        Trigger_ConsumePending(p->receiver2, 1);
    }
    if (p->actionFinished) {
        SceneActorState_StartAction(p, 2, 0, 1);
        Trigger_ConsumePending(p->receiver2, 1);
    } else
        p->actionTimer++;
}
