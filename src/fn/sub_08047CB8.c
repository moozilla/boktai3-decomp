#include "temporary_animation_pool.h"
s32 TemporaryAnimationPool_Update(struct TemporaryAnimationPool *);
s32 TemporaryAnimationPool_Destroy(struct TemporaryAnimationPool *);
s32 TemporaryAnimationPool_Initialize(struct TemporaryAnimationPool *);
struct TemporaryAnimationPool *TemporaryAnimationPool_Create(void)
{
    struct TemporaryAnimationPool *pool = gTemporaryAnimationPool;
    if (!pool) {
        pool = Task_Create(10, sizeof(*pool));
        if (pool) {
            Task_SetCallbacks(pool, (void (*)(void *))TemporaryAnimationPool_Update,
                              (void (*)(void *))TemporaryAnimationPool_Destroy);
            if (TemporaryAnimationPool_Initialize(pool) < 0) {
                Task_Kill(pool);
                return NULL;
            }
        }
    }
    return pool;
}
