#include "effects/healing/healing_effect.h"
#include "engine/engine_pause_flags.h"

void HealthBurstEffect_UpdateBursts(struct HealthBurstEffect *effect)
{
    u32 timer = effect->timer;
    s32 i;

    if (timer == 0 || timer == 18 || timer == 36 || timer == 54 || timer == 72 || timer == 90)
        HealthBurstEffect_StartBurst(effect);
    HealthBurstEffect_UpdateBurstSprites(effect);
    effect->timer++;
    if (effect->timer > 121) {
        for (i = 0; i < 2; i++)
            effect->sprites[i].flags |= 1;
        Engine_ClearPauseFlags(4);
        effect->state = HealthBurstEffect_UpdateFadeOut;
        effect->timer = 0;
    }
}
