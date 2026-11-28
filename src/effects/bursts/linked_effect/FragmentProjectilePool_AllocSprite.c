#include "effects/bursts/linked_effect.h"

struct LinkedEffect *LargeEffectPool_AllocateSlot(struct LinkedEffectPool *);

// &effect->sprite == effect, but agbcc keeps the null test for the address expression
struct FragmentEffectSprite *FragmentProjectilePool_AllocSprite(struct LinkedEffectPool *pool)
{
    struct LinkedEffect *effect = LargeEffectPool_AllocateSlot(pool);

    if (!effect)
        return NULL;
    return &effect->sprite;
}
