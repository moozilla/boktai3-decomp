#include "engine/shared/global.h"
#include "engine/shared/state_actor_action_entry.h"

void Ratatosk_UpdateTriggerState35(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        SceneActorState_SetAnimation(p, 0x23);
    }
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
}
