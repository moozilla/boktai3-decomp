#include "effects/healing/healing_effect.h"

void Callback_InvokeActor(void *, void *);

void HealthRestoreEffect_UpdateParticles(struct HealthRestoreEffect *effect)
{
    s32 i;

    for (i = 0; i < 8; i++) {
        if (effect->particles[i].active)
            Callback_InvokeActor(&effect->particles[i], effect->particles[i].update);
    }
}
