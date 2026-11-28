#include "effects/healing/healing_effect.h"
#include "engine/engine.h"

s32 SceneEffectGroup_Remove(struct HealthBurstEffect *effect)
{
    s32 i;

    Anim_Remove(&effect->icon);
    for (i = 0; i < 8; i++)
        Effect_Remove(&effect->particles[i]);
    if (effect->mode == 0) {
        for (i = 0; i < 2; i++)
            Sprite_Remove(&effect->sprites[i]);
    }
    return 0;
}
