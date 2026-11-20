#include "spark_particle.h"
s32 SparkParticle_DestroyPool(struct SparkParticlePool *pool)
{
    s32 i;
    for (i = 0; i < 48; i++)
        Effect_Remove(&pool->particles[i].sprite);
    gSparkParticlePool = NULL;
    return 0;
}
