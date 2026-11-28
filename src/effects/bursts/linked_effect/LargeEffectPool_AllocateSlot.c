#include "effects/bursts/linked_effect.h"

struct LinkedEffect *LargeEffectPool_AllocateSlot(struct LinkedEffectPool *pool)
{
    s32 i;
    struct LinkedEffect *effect;

    for (i = 0; i < 8; i++) {
        if (!LinkedEffectPool_IsOccupied(pool, i)) {
            effect = &pool->effects[i];
            Memory_Clear(effect, sizeof(*effect));
            pool->occupied |= 1 << i;
            effect->slot = i;
            return effect;
        }
    }
    return NULL;
}
