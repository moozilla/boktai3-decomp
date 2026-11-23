#include "global.h"
#include "debug.h"
#include "hresvelgr_travel.h"

void ActorState_Restart(struct HresvelgrTravelActor *p)
{
    struct HresvelgrTravelActor *base = p;
    if (Hresvelgr_ConsumeActionEntry(p)) {
        DEBUG_PRINT();
    }
    if (p->finished) {
        u8 phase = base->savedPhase;
        u32 zero = 0;
        u8 animation;
        void *callback;
        p->actionMode = zero;
        p->actionIndex = zero;
        p->actionPhase = phase;
        animation = base->savedAnimation;
        callback = base->savedCallback;
        p->currentAnimation = animation;
        p->actionCallback = callback;
        p->behaviorFlags |= 1;
    }
}
