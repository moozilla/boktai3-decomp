#include "scene_particle_pool.h"
#define SceneParticlePool_ResetSingleton sub_080688C0
extern struct SceneParticlePool *gUnk_02000134;
void SceneParticlePool_ResetSingleton(void)
{
    gUnk_02000134 = 0;
}
