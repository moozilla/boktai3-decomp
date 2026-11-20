#include "state_actor_action_entry.h"
void Hresvelgr_UpdateActionThirtyOneRequest(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        u32 k = 0x1f;
        p->actionAnimation = k;
    }
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
}
