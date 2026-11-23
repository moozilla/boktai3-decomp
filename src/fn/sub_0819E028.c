#include "hresvelgr_travel.h"

void Hresvelgr_UpdateActionTwentySevenRequest(struct HresvelgrTravelActor *p)
{
    struct HresvelgrTravelActor *q = p;
    if (Hresvelgr_EnterVulnerableBehavior(q)) {
        Hresvelgr_SetActionAnimation(p, 27);
    }
    if (p->finished)
        q->behaviorFinished = 1;
}
