#include "effects/healing/healing_effect.h"
#include "engine/video/render.h"

void Callback_InvokeActor(void *, void *);

u32 HealthRestoreEffect_OnUpdate(struct HealthRestoreEffect *effect)
{
    HealthRestoreEffect_UpdateParticles(effect);
    Callback_InvokeActor(effect, effect->state);
    gGrayscaleControl = 2;
    return 0;
}
