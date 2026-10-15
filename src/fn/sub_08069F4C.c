#include "burst_emitter.h"
struct BurstEmitterPool *BurstEmitter_CreatePool(void)
{
    struct BurstEmitterPool *pool;
    if (gUnk_02000160)
        return gUnk_02000160;
    pool = Task_Create(10, sizeof(*pool));
    if (pool) {
        Task_SetCallbacks(pool, (void *)BurstEmitter_UpdatePool, (void *)BurstEmitter_DestroyPool);
        if (BurstEmitter_InitPool(pool, 0) < 0) {
            Task_Kill(pool);
            return NULL;
        }
    }
    return pool;
}
