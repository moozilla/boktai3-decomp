#include "global.h"
#include "scene_callback_pool.h"
#include "ram.h"
void Memory_Clear(void *, u32);
s32 SceneCallbackPool_Initialize(struct SceneCallbackPool *pool)
{
    s32 i;
    for (i = 0; i < 16; i++) {
        Memory_Clear(pool->slots[i].sprite, 8);
        pool->slots[i].active = 0;
    }
    pool->frame = 0;
    gSceneCallbackPool = pool;
    return 0;
}
