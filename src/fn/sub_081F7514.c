#include "global.h"
#include "state_actor_action_entry.h"

void SceneActor_UpdateState28AndReturnCommand(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        SceneActorState_SetAnimation(p, 0x1c);
    }
    if (p->actionFinished) {
        SceneActorState_SetAnimation(p, 1);
        Trigger_ConsumePending(p->receiver2, 1);
    } else
        p->actionTimer++;
}
