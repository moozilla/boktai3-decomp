#include "scene_particle_pool.h"
extern struct SceneParticlePool *gUnk_02000134;
void SceneParticlePool_ResetSingleton(void)
{
    gUnk_02000134 = 0;
}
