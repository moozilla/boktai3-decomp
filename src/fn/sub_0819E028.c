#include "hresvelgr_travel.h"

void Hresvelgr_UpdateActionTwentySevenRequest(struct HresvelgrTravelActor *p)
{
    struct HresvelgrTravelActor *q = p;
    if (Hresvelgr_EnterVulnerableBehavior(q)) {
        s32 kind = 27;
        u8 *field = &p->requestedAnimation;
        *field = kind;
    }
    if (p->finished)
        q->behaviorFinished = 1;
}
