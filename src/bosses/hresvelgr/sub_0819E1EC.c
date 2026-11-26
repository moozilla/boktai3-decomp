#include "global.h"
#include "bosses/hresvelgr/hresvelgr_travel.h"

void HresvelgrState_Hold37(struct HresvelgrTravelActor *p)
{
    struct HresvelgrTravelActor *base = p;
    if (Hresvelgr_ConsumeActionEntry(p))
        Hresvelgr_SetActionAnimation(p, 0x25);
    if (p->finished) {
        Hresvelgr_StartAction(p, 0, 0, base->savedPhase);
        base->behaviorFinished = 1;
    }
    p->actionTimer++;
}
