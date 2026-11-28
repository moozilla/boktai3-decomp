#include "effects/bursts/linked_effect.h"

void FragmentProjectilePool_RequestRemovalAll(void)
{
    struct LinkedEffectPool *pool = (struct LinkedEffectPool *)gActorEffectManager;
    struct LinkedEffect *effect;
    s32 i;

    if (!pool)
        return;
    for (i = 0; i < 8; i++) {
        if (LinkedEffectPool_IsOccupied(pool, i)) {
            effect = &pool->effects[i];
            if (effect->state != 3) {
                effect->state = 3;
                effect->entered = 1;
            }
        }
    }
}
