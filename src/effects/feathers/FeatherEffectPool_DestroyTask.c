#include "effects/feathers/feather_effect_pool.h"
#include "engine/engine.h"

u32 FeatherEffectPool_DestroyTask(struct FeatherPool *pool)
{
    s32 i;

    for (i = 0; i < 20; i++) {
        if (FeatherPool_IsUsed(pool, i)) {
            if (pool->slots[i].sprite.linked)
                Sprite_Remove(&pool->slots[i].sprite);
        }
    }
    gFadingSpritePool = NULL;
    return 0;
}
