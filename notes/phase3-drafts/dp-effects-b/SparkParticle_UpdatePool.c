#include "spark_particle.h"
#include "scene_particle_step.h"
s32 SparkParticle_UpdatePool(struct SparkParticlePool *pool)
{
    if (pool->activeCount) {
        s32 i;
        struct SparkParticle *particle = pool->particles;
        for (i = 0; i < 48; i++, particle++) {
            if (particle->active) {
                particle->sprite.position.x += particle->velocity.x;
                particle->sprite.position.y += particle->velocity.y;
                particle->sprite.position.z += particle->velocity.z;
                if (SceneParticle_StepSprite(&particle->sequence,
                       (struct SceneParticleEffect *)&particle->sprite,
                       (const u16 *)pool->graphics)) {
                    particle->active = 0;
                    particle->sprite.flags |= 1;
                    pool->activeCount--;
                }
            }
        }
    }
    return 0;
}
