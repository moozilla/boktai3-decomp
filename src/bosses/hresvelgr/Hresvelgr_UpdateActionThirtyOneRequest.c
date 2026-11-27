#include "bosses/state_actor_action_entry.h"
void Hresvelgr_UpdateActionThirtyOneRequest(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0x1f);
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
}
