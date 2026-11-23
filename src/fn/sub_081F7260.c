#include "global.h"
#include "state_actor_action_entry.h"

void SceneActor_UpdateState2Command(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        SceneActorState_SetAnimation(p, 0x2);
        Trigger_ConsumePending(p->receiver2, 1);
    }
    p->actionTimer++;
}
