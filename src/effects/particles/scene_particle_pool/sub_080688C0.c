#include "effects/particles/scene_particle_pool.h"
#include "engine/ram.h"
void SceneParticlePool_ResetSingleton(void)
{
    gSceneParticleEmitterPool = 0;
}
