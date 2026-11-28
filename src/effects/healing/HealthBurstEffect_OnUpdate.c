#include "effects/healing/healing_effect.h"

void Callback_InvokeActor(void *, void *);

u32 HealthBurstEffect_OnUpdate(struct HealthBurstEffect *effect)
{
    FlickerParticle_UpdatePool(effect);
    Callback_InvokeActor(effect, effect->state);
    return 0;
}
