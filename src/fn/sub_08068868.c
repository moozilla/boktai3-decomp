#include "scene_particle_pool.h"
void SceneParticlePool_LoadResources(struct SceneParticlePool *);
s32 SceneParticlePool_Initialize(struct SceneParticlePool *pool)
{
    SceneParticlePool_LoadResources(pool);
    return 0;
}
