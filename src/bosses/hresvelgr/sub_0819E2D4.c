#include "global.h"
#include "bosses/hresvelgr/hresvelgr_travel.h"

void ActorState_Hold35(struct HresvelgrTravelActor *p)
{
    struct HresvelgrTravelActor *base = p;
    if (Hresvelgr_ConsumeActionEntry(p))
        Hresvelgr_SetActionAnimation(p, 0x23);
    if (p->finished) {
        Hresvelgr_StartAction(p, 0, 0, base->savedPhase);
        base->behaviorFinished = 1;
    }
    p->actionTimer++;
}
