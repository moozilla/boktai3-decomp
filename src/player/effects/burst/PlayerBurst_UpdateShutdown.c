#include "player/effects/orbit_effect.h"
#include "engine/engine_globals.h"

static inline void andm(u32 *a, u32 m)
{
    *a &= m;
}

void PlayerBurst_UpdateShutdown(struct PlayerOrbitEffect *effect)
{
    effect->drawable.flags |= 1;
    if (effect->phaseFrame == 0) {
        andm(&gEnginePauseFlags, ~4);
        gEngineFlags &= ~1;
    }
    if (++effect->phaseFrame <= 5)
        Collider_Refresh(&effect->collider);
    else
        Task_Kill(effect);
}
