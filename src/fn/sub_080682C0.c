#include "scene_particle_burst.h"
extern struct SceneParticleBurstPool *gUnk_02000130;
void SceneParticleBurst_ResetSingleton(void)
{
    gUnk_02000130 = 0;
}
