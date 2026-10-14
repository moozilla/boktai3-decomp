#include "pooled_motion.h"
struct MotionPool *MotionPool_Create(void)
{
    struct MotionPool *pool = Task_Create(8, sizeof(*pool));
    if (pool) {
        Task_SetCallbacks(pool, (void (*)(void *))MotionPool_Update,
                          (void (*)(void *))MotionPool_Destroy);
        if (MotionPool_Initialize(pool) >= 0)
            goto ready;
        Task_Kill(pool);
        return 0;
    }
ready:
    return pool;
}
