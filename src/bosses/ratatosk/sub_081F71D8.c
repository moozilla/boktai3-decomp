#include "global.h"
#include "bosses/state_actor_action_entry.h"

void SceneActor_UpdateState29Command(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        SceneActorState_SetAnimation(p, 0x1D);
        Trigger_ConsumePending(p->receiver2, 1);
    }
    p->actionTimer++;
}
