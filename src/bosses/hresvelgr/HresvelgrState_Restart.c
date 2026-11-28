#include "global.h"
#include "debug.h"
#include "bosses/hresvelgr/hresvelgr_travel.h"

void HresvelgrState_Restart(struct HresvelgrTravelActor *actor)
{
    struct HresvelgrTravelActor *self = actor;
    if (Hresvelgr_ConsumeActionEntry(actor)) {
        DEBUG_PRINT();
    }
    if (actor->finished) {
        u8 phase = self->savedPhase;
        u32 zero = 0;
        u8 animation;
        void *callback;
        actor->actionMode = zero;
        actor->actionIndex = zero;
        actor->actionPhase = phase;
        animation = self->savedAnimation;
        callback = self->savedCallback;
        actor->currentAnimation = animation;
        actor->actionCallback = callback;
        actor->behaviorFlags |= 1;
    }
}
