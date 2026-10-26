#include "spark_particle.h"
s32 SparkParticle_InitPool(struct SparkParticlePool *pool)
{
    u8 *active;
    struct SparkParticleSprite *sprite;
    s32 i;
    pool->sequenceResource = ScriptProc_Start(0x922E, 0xD1B8);
    if (pool->sequenceResource) {
        pool->graphics = AnimData_FindEntry(0x1C1A);
        if (pool->graphics)
            goto init;
    }
    return -1;
init:
    active = &pool->particles[0].active;
    sprite = &pool->particles[0].sprite;
    i = 47;
    do {
        *active = 0;
        ParticleSprite_Initialize(sprite, pool->graphics, 0);
        Effect_SetOrigin(sprite, -8, -10);
        sprite->renderMode = 2;
        sprite->flags |= 1;
        active += sizeof(struct SparkParticle);
        sprite = (struct SparkParticleSprite *)((u8 *)sprite + sizeof(struct SparkParticle));
        i--;
    } while (i >= 0);
    gSparkParticlePool = pool;
    return 0;
}
