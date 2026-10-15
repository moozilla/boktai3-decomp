#include "scene_particle_burst.h"
#define SceneParticleBurst_Initialize sub_08068258
void sub_08068230(struct SceneParticleBurstPool *);
s32 SceneParticleBurst_Initialize(struct SceneParticleBurstPool *p, u32 mode)
{
    p->mode = mode;
    sub_08068230(p);
    return 0;
}
