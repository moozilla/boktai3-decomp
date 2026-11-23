#include "global.h"
#include "hresvelgr_travel.h"

// confidence=low: enters animation 38 and notifies the linked actor once.
void sub_08159164(void *);

void Hresvelgr_BeginLinkedAnimation(struct HresvelgrTravelActor *arg)
{
    struct HresvelgrTravelActor *p = arg;
    struct HresvelgrTravelActor *base = p;
    if (Hresvelgr_ConsumeActionEntry(p)) {
        if (p->currentAnimation != 38)
            Hresvelgr_SetActionAnimation(p, 38);
        sub_08159164(base->linked);
    }
    p->actionTimer++;
}
