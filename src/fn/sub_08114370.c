#define SceneCallbackPool_Update sub_08114370
#include "global.h"
#include "scene_callback_pool.h"
s32 sub_08249240(void *, u32, void *);
s32 sub_08114370(struct SceneCallbackPool *pool)
{
    s32 i;
    for (i = 0; i < 16; i++) {
        if (pool->slots[i].active && pool->slots[i].callback)
            sub_08249240(&pool->slots[i], i * 0xD8, pool->slots[i].callback);
    }
    pool->frame++;
    return 0;
}
