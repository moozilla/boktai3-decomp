#include "effects/healing/healing_effect.h"
#include "engine/engine.h"

s32 HealthRestoreEffect_OnDestroy(struct HealthRestoreEffect *effect)
{
    s32 i;

    Anim_Remove(&effect->icon);
    for (i = 0; i < 8; i++)
        Effect_Remove(&effect->particles[i]);
    return 0;
}
