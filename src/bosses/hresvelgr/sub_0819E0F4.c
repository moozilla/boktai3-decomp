#include "engine/shared/state_actor_action_entry.h"
void Hresvelgr_UpdateActionThirtyRequest(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        SceneActorState_SetAnimation(p, 0x1e);
        Trigger_ConsumePending(p->receiver2, 1);
    }
}
