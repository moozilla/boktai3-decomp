#include "player/effects/orbit_effect.h"

void PlayerOrbitEffect_UpdateParticles(struct PlayerOrbitEffect *);

s32 PlayerBurst_Update(struct PlayerOrbitEffect *effect)
{
    PlayerOrbitEffect_UpdateParticles(effect);
    Callback_InvokeActor(effect, effect->updateState);
    return 0;
}
