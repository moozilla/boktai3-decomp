#include "global.h"
#include "engine.h"
#include "player.h"
struct PlayerPhaseEffect {
    u8 unknown0[0x1c];
    struct Player *owner;
    u8 effect[0x90];
    u8 active;
    u8 unknownB1[7];
    void *callback;
};
void PlayerEffect_UpdatePhaseCallback(struct PlayerPhaseEffect *effect)
{
    if (!effect->active) {
        if (effect->owner->tetherMode == 9 && effect->owner->tetherPhase == 2)
            effect->active = 1;
        if (!effect->active)
            return;
    }
    {
        void **callback = &effect->callback;
        Callback_InvokeActor(effect->effect, *callback);
    }
}
