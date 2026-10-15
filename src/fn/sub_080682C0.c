#include "scene_particle_burst.h"
#define SceneParticleBurst_ResetSingleton sub_080682C0
extern struct SceneParticleBurstPool *gUnk_02000130;
void SceneParticleBurst_ResetSingleton(void)
{
    gUnk_02000130 = 0;
}
