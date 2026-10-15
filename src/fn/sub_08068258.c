#include "scene_particle_burst.h"
void SceneParticleBurst_LoadResources(struct SceneParticleBurstPool *);
s32 SceneParticleBurst_Initialize(struct SceneParticleBurstPool *p, u32 mode)
{
    p->mode = mode;
    SceneParticleBurst_LoadResources(p);
    return 0;
}
