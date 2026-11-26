#include "bosses/hresvelgr/hresvelgr_travel.h"

void Hresvelgr_UpdateActionTwoRequest(struct HresvelgrTravelActor *p)
{
    struct HresvelgrTravelActor *q = p;
    if (Hresvelgr_EnterVulnerableBehavior(q)) {
        Hresvelgr_SetActionAnimation(p, 2);
    }
    if (p->finished)
        q->behaviorFinished = 1;
}
