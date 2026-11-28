#include "global.h"
#include "engine/input.h"
#include "bosses/hresvelgr/hresvelgr_travel.h"

void sub_0819D190(struct HresvelgrTravelActor *actor)
{
    if (gPadState[1] & 0x100) {
        s32 animation = (actor->currentAnimation + 1) % 39;
        if (!animation)
            animation = 1;
        actor->requestedAnimation = animation;
    }
    actor->actionTimer++;
}
