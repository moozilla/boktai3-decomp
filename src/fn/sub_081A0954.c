#include "hresvelgr_travel.h"
void ActionState_Reset(u8 *);
void ActionState_AdvanceState(u8 *);
void Hresvelgr_PlayDelayedReposition(struct HresvelgrTravelActor *p)
{
    u8 *action = p->action;
    {
        u32 mask = 0x1c;
        p->flags |= mask;
    }
    if (p->changed) {
        p->changed = 0;
        ActionState_Reset(action);
    }
    switch (*action) {
        case 0:
            if (++*(u32 *)(action + 4) > 119) {
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
