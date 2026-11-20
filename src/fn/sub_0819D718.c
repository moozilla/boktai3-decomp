#include "hresvelgr_travel.h"

void Hresvelgr_UpdateActionTwoRequest(struct HresvelgrTravelActor *p)
{
    struct HresvelgrTravelActor *q = p;
    if (Hresvelgr_EnterVulnerableBehavior(q)) {
        s32 kind = 2;
        u8 *field = &p->requestedAnimation;
        *field = kind;
    }
    if (p->finished)
        q->behaviorFinished = 1;
}
