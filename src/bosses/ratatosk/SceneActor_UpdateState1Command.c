#include "global.h"
#include "bosses/state_actor_action_entry.h"

void SceneActor_UpdateState1Command(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p) == TRUE) {
        SceneActorState_SetAnimation(p, 1);
        Trigger_ConsumePending(p->receiver2, 1);
    }
    p->actionTimer++;
}
