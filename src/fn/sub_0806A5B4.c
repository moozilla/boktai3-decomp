#include "spark_particle.h"
#define SparkParticle_CreatePool sub_0806A5B4
struct SparkParticlePool *SparkParticle_CreatePool(void)
{
    struct SparkParticlePool *pool;
    if (gUnk_02000168)
        return gUnk_02000168;
    pool = Task_Create(9, sizeof(*pool));
    if (pool) {
        Task_SetCallbacks(pool, (void *)sub_08069FA4, (void *)sub_0806A138);
        if (sub_0806A520(pool) < 0) {
            Task_Kill(pool);
            return NULL;
        }
    }
    return pool;
}
