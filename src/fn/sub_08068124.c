#include "scene_particle_burst.h"
#define SceneParticleBurst_CreateWhilePaused sub_08068124
s32 sub_08067E3C(const struct ContactPosition *, const struct ContactPosition *,
                 const struct ContactPosition *, s32, u32, u32, u32);
s32 SceneParticleBurst_CreateWhilePaused(const struct ContactPosition *position,
                                         const struct ContactPosition *range,
                                         const struct ContactPosition *jitter, s32 count,
                                         u32 priority, u32 flip)
{
    return sub_08067E3C(position, range, jitter, count, priority, flip, 0);
}
