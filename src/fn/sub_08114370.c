#include "global.h"
#include "scene_callback_pool.h"
s32 SceneCallbackPool_Update(struct SceneCallbackPool *pool)
{
    s32 i;
    for (i = 0; i < 16; i++) {
        if (pool->slots[i].active && pool->slots[i].callback)
            pool->slots[i].callback(&pool->slots[i], i * sizeof(pool->slots[i]));
    }
    pool->frame++;
    return 0;
}
