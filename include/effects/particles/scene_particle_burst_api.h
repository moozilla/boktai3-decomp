#ifndef GUARD_SCENE_PARTICLE_BURST_API_H
#define GUARD_SCENE_PARTICLE_BURST_API_H

#include "scene/collision/contact_position.h"

s32 SceneParticleBurst_CreateWithPause(const struct ContactPosition *position,
                                       const struct ContactPosition *velocityRange,
                                       const struct ContactPosition *jitter, s32 count,
                                       u32 priority, u32 flip);

#endif
