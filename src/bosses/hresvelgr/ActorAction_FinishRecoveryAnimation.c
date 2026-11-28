#include "global.h"
#include "bosses/hresvelgr/hresvelgr_travel.h"

void ActorAction_FinishRecoveryAnimation(struct HresvelgrTravelActor *actor)
{
    struct HresvelgrTravelActor *self = actor;
    if (Hresvelgr_ConsumeActionEntry(actor))
        Hresvelgr_SetActionAnimation(actor, 0x22);
    if (actor->finished) {
        Hresvelgr_StartAction(actor, 0, 0, self->savedPhase);
        self->behaviorFinished = 1;
        self->recoveryFrames = 60;
    }
    actor->actionTimer++;
}
