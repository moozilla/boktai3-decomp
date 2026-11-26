#include "global.h"
#include "bosses/hresvelgr/hresvelgr_travel.h"

// confidence=low: enters animation 38 and notifies the linked actor once.
void Player_AddExperience(void *);

void Hresvelgr_BeginLinkedAnimation(struct HresvelgrTravelActor *arg)
{
    struct HresvelgrTravelActor *p = arg;
    struct HresvelgrTravelActor *base = p;
    if (Hresvelgr_ConsumeActionEntry(p)) {
        if (p->currentAnimation != 38)
            Hresvelgr_SetActionAnimation(p, 38);
        Player_AddExperience(base->linked);
    }
    p->actionTimer++;
}
