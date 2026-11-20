#include "state_actor_action_entry.h"
void Hresvelgr_UpdateActionThirtyRequest(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        u8 k = 0x1e;
        p->actionAnimation = k;
        Trigger_ConsumePending(p->receiver2, 1);
    }
}
