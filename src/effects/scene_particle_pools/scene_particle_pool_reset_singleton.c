#include "effects/scene_particle_pools/scene_particle_pool.h"
#include "engine/shared/ram.h"
void SceneParticlePool_ResetSingleton(void)
{
    gSceneParticleEmitterPool = 0;
}
