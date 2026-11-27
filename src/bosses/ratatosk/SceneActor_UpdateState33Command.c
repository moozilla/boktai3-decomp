#include "global.h"
#include "bosses/state_actor_action_entry.h"

void SceneActor_UpdateState33Command(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        SceneActorState_SetAnimation(p, 0x21);
    }
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}
