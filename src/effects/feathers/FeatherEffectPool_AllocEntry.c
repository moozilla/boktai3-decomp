#include "effects/feathers/feather_effect_pool.h"
#include "engine/engine.h"

struct Feather *FeatherEffectPool_AllocEntry(struct FeatherPool *pool)
{
    s32 i;

    for (i = 0; i < 20; i++) {
        if (!FeatherPool_IsUsed(pool, i)) {
            struct Feather *feather = &pool->slots[i];

            pool->used |= 1 << i;
            Memory_Clear(feather, sizeof(*feather));
            feather->slot = i;
            return feather;
        }
    }
    return NULL;
}
