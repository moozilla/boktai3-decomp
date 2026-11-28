#include "global.h"
#include "bosses/hresvelgr/hresvelgr_travel.h"

// confidence=low: enters animation 38 and notifies the linked actor once.
void Player_AddExperience(void *);

void Hresvelgr_BeginLinkedAnimation(struct HresvelgrTravelActor *actor)
{
    struct HresvelgrTravelActor *self = actor;
    if (Hresvelgr_ConsumeActionEntry(actor)) {
        if (actor->currentAnimation != 38)
            Hresvelgr_SetActionAnimation(actor, 38);
        Player_AddExperience(self->linked);
    }
    actor->actionTimer++;
}
