#include "global.h"
#include "bosses/state_actor_action_entry.h"

void SceneActor_UpdateState18Command(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        SceneActorState_SetAnimation(p, 0x12);
    }
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}
