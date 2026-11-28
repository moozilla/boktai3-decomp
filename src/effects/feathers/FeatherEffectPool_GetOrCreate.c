#include "effects/feathers/feather_effect_pool.h"
#include "engine/engine.h"

struct FeatherPool *FeatherEffectPool_GetOrCreate(void)
{
    struct FeatherPool *pool;

    if (gFadingSpritePool != NULL)
        return gFadingSpritePool;
    pool = (struct FeatherPool *)Task_Create(8, sizeof(*pool));
    if (pool != NULL) {
        Task_SetCallbacks(&pool->task, FeatherEffectPool_UpdateTask, FeatherEffectPool_DestroyTask);
        if (FeatherEffectPool_Init(pool) < 0) {
            Task_Kill(&pool->task);
            return NULL;
        }
    }
    return pool;
}
