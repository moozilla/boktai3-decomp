#include "effects/particles/burst_emitter.h"
struct BurstEmitterPool *SparkExplosion_CreatePool(void)
{
    struct BurstEmitterPool *pool;
    if (gBurstEmitterPool)
        return gBurstEmitterPool;
    pool = Task_Create(10, sizeof(*pool));
    if (pool) {
        Task_SetCallbacks(pool, (void *)SparkExplosion_UpdatePool, (void *)SparkExplosion_DestroyPool);
        if (SparkExplosion_InitPool(pool, 0) < 0) {
            Task_Kill(pool);
            return NULL;
        }
    }
    return pool;
}
