#include "engine/shared/global.h"
#include "engine/shared/state_actor_action_entry.h"

void SceneActor_UpdateState22Command(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        SceneActorState_SetAnimation(p, 0x16);
    }
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}
