#include "bosses/state_actor_action_entry.h"
void Hresvelgr_UpdateActionTwentyNineRequest(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        SceneActorState_SetAnimation(p, 0x1d);
        Trigger_ConsumePending(p->receiver2, 1);
    }
}
