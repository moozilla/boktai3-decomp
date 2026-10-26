#include "spark_particle.h"
s32 SparkParticle_DestroyPool(struct SparkParticlePool *pool)
{
    struct SparkParticleSprite *sprite = &pool->particles[0].sprite;
    s32 i = 47;
    do {
        Effect_Remove(sprite);
        sprite = (struct SparkParticleSprite *)((u8 *)sprite + sizeof(struct SparkParticle));
        i--;
    } while (i >= 0);
    {
        u32 zero = 0;
        gSparkParticlePool = (struct SparkParticlePool *)zero;
    }
    return 0;
}
