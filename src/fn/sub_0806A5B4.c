#include "spark_particle.h"
struct SparkParticlePool *SparkParticle_CreatePool(void)
{
    struct SparkParticlePool *pool;
    if (gSparkParticlePool)
        return gSparkParticlePool;
    pool = Task_Create(9, sizeof(*pool));
    if (pool) {
        Task_SetCallbacks(pool, (void *)SparkParticle_UpdatePool,
                          (void *)SparkParticle_DestroyPool);
        if (SparkParticle_InitPool(pool) < 0) {
            Task_Kill(pool);
            return NULL;
        }
    }
    return pool;
}
