#include "engine/shared/global.h"
#include "engine/shared/state_actor_action_entry.h"

void SceneActor_UpdateState3Command(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        SceneActorState_SetAnimation(p, 0x3);
        Trigger_ConsumePending(p->receiver2, 1);
    }
    p->actionTimer++;
}
