#include "hresvelgr_travel.h"

void ActionState_Reset(struct BossActionState *);
void ActionState_AdvanceState(struct BossActionState *);
void Hresvelgr_PlayDelayedReposition(struct HresvelgrTravelActor *p)
{
    struct BossActionState *action = &p->action;
    {
        u32 mask = 0x1c;
        p->flags |= mask;
    }
    if (p->changed) {
        p->changed = 0;
        ActionState_Reset(action);
    }
    switch (action->state) {
        case 0:
            if (++action->frame > 119) {
                Hresvelgr_RefreshTarget(p);
                p->position = p->target;
                p->position.y = 0x2000;
                ActionState_AdvanceState(action);
            }
            break;
        case 1:
            p->finished = 1;
            break;
    }
}
