#include "bosses/hresvelgr/hresvelgr_travel.h"
void Hresvelgr_UpdateActionTwoRequest(struct HresvelgrTravelActor *actor)
{
    struct HresvelgrTravelActor *self = actor;
    if (Hresvelgr_EnterVulnerableBehavior(self)) {
        Hresvelgr_SetActionAnimation(actor, 2);
    }
    if (actor->finished)
        self->behaviorFinished = 1;
}
