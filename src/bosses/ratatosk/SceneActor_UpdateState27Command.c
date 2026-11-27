#include "global.h"
#include "bosses/state_actor_action_entry.h"

void SceneActor_UpdateState27Command(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        SceneActorState_SetAnimation(p, 0x1B);
        Trigger_ConsumePending(p->receiver2, 1);
    }
    p->actionTimer++;
}
