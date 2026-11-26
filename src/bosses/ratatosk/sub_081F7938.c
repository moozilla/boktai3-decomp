#include "global.h"
#include "bosses/state_actor_action_entry.h"

void Ratatosk_UpdateTriggerState36(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        SceneActorState_SetAnimation(p, 0x24);
    }
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
}
