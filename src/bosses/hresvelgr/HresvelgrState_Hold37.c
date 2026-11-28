#include "global.h"
#include "bosses/hresvelgr/hresvelgr_travel.h"

void HresvelgrState_Hold37(struct HresvelgrTravelActor *actor)
{
    struct HresvelgrTravelActor *self = actor;
    if (Hresvelgr_ConsumeActionEntry(actor))
        Hresvelgr_SetActionAnimation(actor, 0x25);
    if (actor->finished) {
        Hresvelgr_StartAction(actor, 0, 0, self->savedPhase);
        self->behaviorFinished = 1;
    }
    actor->actionTimer++;
}
